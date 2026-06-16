#include "ui_context.hpp"

#include <cassert>
#include <stdexcept>
#include <string>

#include <dwmapi.h>
#include "imgui/backends/imgui_impl_win32.h"

#if defined(IMGUI_USE_DX9)
    #include <d3d9.h>
    #include "imgui/backends/imgui_impl_dx9.h"
#elif defined(IMGUI_USE_DX10)
    #include <d3d10.h>
    #include <dxgi.h>
    #include "imgui/backends/imgui_impl_dx10.h"
#elif defined(IMGUI_USE_DX11)
    #include <d3d11.h>
    #include <dxgi.h>
    #include "imgui/backends/imgui_impl_dx11.h"
#elif defined(IMGUI_USE_DX12)
    #include <d3d12.h>
    #include <dxgi1_4.h>
    #include "imgui/backends/imgui_impl_dx12.h"
#else
    #error "No DirectX backend selected"
#endif

constexpr wchar_t WINDOW_CLASS_NAME[] = L"MooImGuiDirectXTemplateWindow";

[[noreturn]] void throw_last_error(const char* what) {
    throw std::runtime_error(what);
}

#if !defined(IMGUI_USE_DX9)
UINT present_interval(bool vsync) noexcept {
    return vsync ? 1U : 0U;
}
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct UiContext::Impl {
    UiConfig config{};
    HWND hwnd = nullptr;
    WNDCLASSEXW wc{};
    bool done = false;
    bool minimized = false;
    bool imgui_context_created = false;
    bool backends_initialized = false;
    float main_scale = 1.0f;

#if defined(IMGUI_USE_DX9)
    LPDIRECT3D9 d3d = nullptr;
    LPDIRECT3DDEVICE9 device = nullptr;
    D3DPRESENT_PARAMETERS present_params{};
#elif defined(IMGUI_USE_DX10)
    ID3D10Device* device = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D10RenderTargetView* render_target_view = nullptr;
#elif defined(IMGUI_USE_DX11)
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* device_context = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11RenderTargetView* render_target_view = nullptr;
#elif defined(IMGUI_USE_DX12)
    static constexpr UINT NUM_FRAMES_IN_FLIGHT = 3;
    struct FrameContext {
        ID3D12CommandAllocator* command_allocator = nullptr;
        UINT64 fence_value = 0;
    };

    FrameContext frame_context[NUM_FRAMES_IN_FLIGHT]{};
    UINT frame_index = 0;
    ID3D12Device* device = nullptr;
    ID3D12DescriptorHeap* rtv_desc_heap = nullptr;
    ID3D12DescriptorHeap* srv_desc_heap = nullptr;
    ID3D12CommandQueue* command_queue = nullptr;
    ID3D12GraphicsCommandList* command_list = nullptr;
    IDXGISwapChain3* swap_chain = nullptr;
    HANDLE fence_event = nullptr;
    ID3D12Fence* fence = nullptr;
    UINT64 fence_last_signaled_value = 0;
    ID3D12Resource* main_render_target_resource[NUM_FRAMES_IN_FLIGHT]{};
    D3D12_CPU_DESCRIPTOR_HANDLE main_render_target_descriptor[NUM_FRAMES_IN_FLIGHT]{};
#endif

    explicit Impl(const UiConfig& cfg) : config{cfg} {
        init_window();
        init_device();
        init_imgui();
    }

    ~Impl() {
        shutdown();
    }

    static LRESULT WINAPI wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
            return true;
        }

        Impl* self = reinterpret_cast<Impl*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

        switch (msg) {
            case WM_NCCREATE: {
                auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
                auto* impl = reinterpret_cast<Impl*>(cs->lpCreateParams);
                SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(impl));
                return DefWindowProcW(hWnd, msg, wParam, lParam);
            }
            case WM_SIZE:
                if (self != nullptr) {
                    self->minimized = (wParam == SIZE_MINIMIZED);
                    if (!self->minimized) {
                        self->resize(static_cast<UINT>(LOWORD(lParam)), static_cast<UINT>(HIWORD(lParam)));
                    }
                }
                return 0;
            case WM_SYSCOMMAND:
                if ((wParam & 0xfff0U) == SC_KEYMENU) {
                    return 0;
                }
                break;
            case WM_DESTROY:
                if (self != nullptr) {
                    self->done = true;
                }
                PostQuitMessage(0);
                return 0;
            default:
                break;
        }

        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    void init_window() {
        wc = {
            static_cast<UINT>(sizeof(WNDCLASSEXW)),
            CS_CLASSDC,
            Impl::wnd_proc,
            0L,
            0L,
            GetModuleHandleW(nullptr),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            WINDOW_CLASS_NAME,
            nullptr
        };

        if (RegisterClassExW(&wc) == 0) {
            throw_last_error("RegisterClassExW failed");
        }

        RECT rect{0, 0, static_cast<LONG>(config.width), static_cast<LONG>(config.height)};
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        hwnd = CreateWindowW(
            wc.lpszClassName,
            config.title,
            WS_OVERLAPPEDWINDOW,
            100,
            100,
            static_cast<int>(rect.right - rect.left),
            static_cast<int>(rect.bottom - rect.top),
            nullptr,
            nullptr,
            wc.hInstance,
            this
        );

        if (hwnd == nullptr) {
            throw_last_error("CreateWindowW failed");
        }

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);
    }

    void init_imgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imgui_context_created = true;

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        setup_styles();

        const bool platform_ok = ImGui_ImplWin32_Init(hwnd);
        assert(platform_ok);
        (void)platform_ok;

#if defined(IMGUI_USE_DX9)
        const bool renderer_ok = ImGui_ImplDX9_Init(device);
#elif defined(IMGUI_USE_DX10)
        const bool renderer_ok = ImGui_ImplDX10_Init(device);
#elif defined(IMGUI_USE_DX11)
        const bool renderer_ok = ImGui_ImplDX11_Init(device, device_context);
#elif defined(IMGUI_USE_DX12)
        ImGui_ImplDX12_InitInfo init_info{};
        init_info.Device = device;
        init_info.CommandQueue = command_queue;
        init_info.NumFramesInFlight = static_cast<int>(NUM_FRAMES_IN_FLIGHT);
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
        init_info.SrvDescriptorHeap = srv_desc_heap;
        init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
            auto* self = static_cast<Impl*>(info->UserData);
            *out_cpu_handle = self->srv_desc_heap->GetCPUDescriptorHandleForHeapStart();
            *out_gpu_handle = self->srv_desc_heap->GetGPUDescriptorHandleForHeapStart();
        };
        init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE) {};
        init_info.UserData = this;
        const bool renderer_ok = ImGui_ImplDX12_Init(&init_info);
#endif
        assert(renderer_ok);
        (void)renderer_ok;
        backends_initialized = true;
    }

    void setup_styles() {
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        style.FontScaleDpi = main_scale;
        style.WindowRounding = 6.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarSize = 12.0f;
    }

    void init_device() {
#if defined(IMGUI_USE_DX9)
        d3d = Direct3DCreate9(D3D_SDK_VERSION);
        if (d3d == nullptr) {
            throw std::runtime_error("Direct3DCreate9 failed");
        }

        ZeroMemory(&present_params, sizeof(present_params));
        present_params.Windowed = TRUE;
        present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        present_params.BackBufferFormat = D3DFMT_UNKNOWN;
        present_params.EnableAutoDepthStencil = TRUE;
        present_params.AutoDepthStencilFormat = D3DFMT_D16;
        present_params.PresentationInterval = config.vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

        if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_params, &device) < 0) {
            throw std::runtime_error("IDirect3D9::CreateDevice failed");
        }
#elif defined(IMGUI_USE_DX10)
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT create_device_flags = 0;
        if (D3D10CreateDeviceAndSwapChain(nullptr, D3D10_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags,
            D3D10_SDK_VERSION, &sd, &swap_chain, &device) != S_OK) {
            throw std::runtime_error("D3D10CreateDeviceAndSwapChain failed");
        }
        create_render_target();
#elif defined(IMGUI_USE_DX11)
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT create_device_flags = 0;
        D3D_FEATURE_LEVEL feature_level{};
        constexpr D3D_FEATURE_LEVEL feature_level_array[2] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0,
        };

        if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags,
            feature_level_array, 2, D3D11_SDK_VERSION, &sd, &swap_chain, &device,
            &feature_level, &device_context) != S_OK) {
            throw std::runtime_error("D3D11CreateDeviceAndSwapChain failed");
        }
        create_render_target();
#elif defined(IMGUI_USE_DX12)
        DXGI_SWAP_CHAIN_DESC1 sd{};
        sd.BufferCount = NUM_FRAMES_IN_FLIGHT;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;

        if (D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)) != S_OK) {
            throw std::runtime_error("D3D12CreateDevice failed");
        }

        D3D12_DESCRIPTOR_HEAP_DESC rtv_desc{};
        rtv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtv_desc.NumDescriptors = NUM_FRAMES_IN_FLIGHT;
        rtv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (device->CreateDescriptorHeap(&rtv_desc, IID_PPV_ARGS(&rtv_desc_heap)) != S_OK) {
            throw std::runtime_error("CreateDescriptorHeap RTV failed");
        }

        D3D12_DESCRIPTOR_HEAP_DESC srv_desc{};
        srv_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srv_desc.NumDescriptors = 1;
        srv_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (device->CreateDescriptorHeap(&srv_desc, IID_PPV_ARGS(&srv_desc_heap)) != S_OK) {
            throw std::runtime_error("CreateDescriptorHeap SRV failed");
        }

        for (FrameContext& frame : frame_context) {
            if (device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame.command_allocator)) != S_OK) {
                throw std::runtime_error("CreateCommandAllocator failed");
            }
        }

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if (device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&command_queue)) != S_OK) {
            throw std::runtime_error("CreateCommandQueue failed");
        }

        if (device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frame_context[0].command_allocator, nullptr, IID_PPV_ARGS(&command_list)) != S_OK) {
            throw std::runtime_error("CreateCommandList failed");
        }
        command_list->Close();

        IDXGIFactory4* dxgi_factory = nullptr;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory)) != S_OK) {
            throw std::runtime_error("CreateDXGIFactory1 failed");
        }

        IDXGISwapChain1* swap_chain1 = nullptr;
        if (dxgi_factory->CreateSwapChainForHwnd(command_queue, hwnd, &sd, nullptr, nullptr, &swap_chain1) != S_OK) {
            dxgi_factory->Release();
            throw std::runtime_error("CreateSwapChainForHwnd failed");
        }
        dxgi_factory->Release();

        if (swap_chain1->QueryInterface(IID_PPV_ARGS(&swap_chain)) != S_OK) {
            swap_chain1->Release();
            throw std::runtime_error("IDXGISwapChain3 QueryInterface failed");
        }
        swap_chain1->Release();
        swap_chain->SetMaximumFrameLatency(NUM_FRAMES_IN_FLIGHT);
        frame_index = swap_chain->GetCurrentBackBufferIndex();

        if (device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)) != S_OK) {
            throw std::runtime_error("CreateFence failed");
        }
        fence_event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (fence_event == nullptr) {
            throw std::runtime_error("CreateEvent failed");
        }

        create_render_targets();
#endif
    }

#if defined(IMGUI_USE_DX10)
    void create_render_target() {
        ID3D10Texture2D* back_buffer = nullptr;
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        if (back_buffer) { back_buffer->Release(); }
    }

    void cleanup_render_target() {
        if (render_target_view) { render_target_view->Release(); render_target_view = nullptr; }
    }
#elif defined(IMGUI_USE_DX11)
    void create_render_target() {
        ID3D11Texture2D* back_buffer = nullptr;
        swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
        device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        if (back_buffer) { back_buffer->Release(); }
    }

    void cleanup_render_target() {
        if (render_target_view) { render_target_view->Release(); render_target_view = nullptr; }
    }
#elif defined(IMGUI_USE_DX12)
    void create_render_targets() {
        const UINT rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = rtv_desc_heap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; ++i) {
            main_render_target_descriptor[i] = rtv_handle;
            swap_chain->GetBuffer(i, IID_PPV_ARGS(&main_render_target_resource[i]));
            device->CreateRenderTargetView(main_render_target_resource[i], nullptr, rtv_handle);
            rtv_handle.ptr += static_cast<SIZE_T>(rtv_descriptor_size);
        }
    }

    void cleanup_render_targets() {
        for (ID3D12Resource*& resource : main_render_target_resource) {
            if (resource) { resource->Release(); resource = nullptr; }
        }
    }

    void wait_for_last_submitted_frame() {
        FrameContext& frame = frame_context[frame_index % NUM_FRAMES_IN_FLIGHT];
        const UINT64 fence_value = frame.fence_value;
        if (fence_value == 0) { return; }
        frame.fence_value = 0;
        if (fence->GetCompletedValue() >= fence_value) { return; }
        fence->SetEventOnCompletion(fence_value, fence_event);
        WaitForSingleObject(fence_event, INFINITE);
    }

    FrameContext* wait_for_next_frame_resources() {
        frame_index = swap_chain->GetCurrentBackBufferIndex();
        FrameContext* frame = &frame_context[frame_index % NUM_FRAMES_IN_FLIGHT];
        const UINT64 fence_value = frame->fence_value;
        if (fence_value != 0) {
            frame->fence_value = 0;
            if (fence->GetCompletedValue() < fence_value) {
                fence->SetEventOnCompletion(fence_value, fence_event);
                WaitForSingleObject(fence_event, INFINITE);
            }
        }
        return frame;
    }
#endif

    void resize(UINT width, UINT height) {
        if (width == 0 || height == 0) {
            return;
        }

#if defined(IMGUI_USE_DX9)
        if (device == nullptr) { return; }
        present_params.BackBufferWidth = width;
        present_params.BackBufferHeight = height;
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = device->Reset(&present_params);
        if (hr == D3DERR_INVALIDCALL) { assert(false); }
        ImGui_ImplDX9_CreateDeviceObjects();
#elif defined(IMGUI_USE_DX10) || defined(IMGUI_USE_DX11)
        if (swap_chain == nullptr) { return; }
        cleanup_render_target();
        swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
        create_render_target();
#elif defined(IMGUI_USE_DX12)
        if (swap_chain == nullptr) { return; }
        wait_for_last_submitted_frame();
        cleanup_render_targets();
        swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
        frame_index = swap_chain->GetCurrentBackBufferIndex();
        create_render_targets();
#endif
    }

    bool begin_frame() {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_QUIT) {
                done = true;
            }
        }

        if (done) { return false; }
        if (minimized) {
            Sleep(10);
            return false;
        }

#if defined(IMGUI_USE_DX9)
        ImGui_ImplDX9_NewFrame();
#elif defined(IMGUI_USE_DX10)
        ImGui_ImplDX10_NewFrame();
#elif defined(IMGUI_USE_DX11)
        ImGui_ImplDX11_NewFrame();
#elif defined(IMGUI_USE_DX12)
        ImGui_ImplDX12_NewFrame();
#endif
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        return true;
    }

    void end_frame() {
        ImGui::Render();

#if defined(IMGUI_USE_DX9)
        device->SetRenderState(D3DRS_ZENABLE, FALSE);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(
            static_cast<int>(config.clear_color.x * config.clear_color.w * 255.0f),
            static_cast<int>(config.clear_color.y * config.clear_color.w * 255.0f),
            static_cast<int>(config.clear_color.z * config.clear_color.w * 255.0f),
            static_cast<int>(config.clear_color.w * 255.0f)
        );
        device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (device->BeginScene() >= 0) {
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            device->EndScene();
        }
        HRESULT result = device->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            resize(static_cast<UINT>(config.width), static_cast<UINT>(config.height));
        }
#elif defined(IMGUI_USE_DX10)
        const float clear_color[4] = {
            config.clear_color.x * config.clear_color.w,
            config.clear_color.y * config.clear_color.w,
            config.clear_color.z * config.clear_color.w,
            config.clear_color.w
        };
        device->OMSetRenderTargets(1, &render_target_view, nullptr);
        device->ClearRenderTargetView(render_target_view, clear_color);
        ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());
        swap_chain->Present(present_interval(config.vsync), 0);
#elif defined(IMGUI_USE_DX11)
        const float clear_color[4] = {
            config.clear_color.x * config.clear_color.w,
            config.clear_color.y * config.clear_color.w,
            config.clear_color.z * config.clear_color.w,
            config.clear_color.w
        };
        device_context->OMSetRenderTargets(1, &render_target_view, nullptr);
        device_context->ClearRenderTargetView(render_target_view, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swap_chain->Present(present_interval(config.vsync), 0);
#elif defined(IMGUI_USE_DX12)
        FrameContext* frame = wait_for_next_frame_resources();
        const UINT back_buffer_idx = swap_chain->GetCurrentBackBufferIndex();
        frame->command_allocator->Reset();

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = main_render_target_resource[back_buffer_idx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        command_list->Reset(frame->command_allocator, nullptr);
        command_list->ResourceBarrier(1, &barrier);
        command_list->OMSetRenderTargets(1, &main_render_target_descriptor[back_buffer_idx], FALSE, nullptr);
        const float clear_color[4] = {
            config.clear_color.x * config.clear_color.w,
            config.clear_color.y * config.clear_color.w,
            config.clear_color.z * config.clear_color.w,
            config.clear_color.w
        };
        command_list->ClearRenderTargetView(main_render_target_descriptor[back_buffer_idx], clear_color, 0, nullptr);
        ID3D12DescriptorHeap* heaps[] = { srv_desc_heap };
        command_list->SetDescriptorHeaps(1, heaps);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        command_list->ResourceBarrier(1, &barrier);
        command_list->Close();

        ID3D12CommandList* command_lists[] = { command_list };
        command_queue->ExecuteCommandLists(1, command_lists);
        swap_chain->Present(present_interval(config.vsync), 0);

        const UINT64 fence_value = ++fence_last_signaled_value;
        command_queue->Signal(fence, fence_value);
        frame->fence_value = fence_value;
#endif
    }

    void shutdown() noexcept {
        if (backends_initialized) {
#if defined(IMGUI_USE_DX9)
            ImGui_ImplDX9_Shutdown();
#elif defined(IMGUI_USE_DX10)
            ImGui_ImplDX10_Shutdown();
#elif defined(IMGUI_USE_DX11)
            ImGui_ImplDX11_Shutdown();
#elif defined(IMGUI_USE_DX12)
            ImGui_ImplDX12_Shutdown();
#endif
            ImGui_ImplWin32_Shutdown();
            backends_initialized = false;
        }

        if (imgui_context_created) {
            ImGui::DestroyContext();
            imgui_context_created = false;
        }

        cleanup_device();

        if (hwnd) {
            DestroyWindow(hwnd);
            hwnd = nullptr;
        }

        if (wc.lpszClassName) {
            UnregisterClassW(wc.lpszClassName, wc.hInstance);
            wc = {};
        }
    }

    void cleanup_device() noexcept {
#if defined(IMGUI_USE_DX9)
        if (device) { device->Release(); device = nullptr; }
        if (d3d) { d3d->Release(); d3d = nullptr; }
#elif defined(IMGUI_USE_DX10)
        cleanup_render_target();
        if (swap_chain) { swap_chain->Release(); swap_chain = nullptr; }
        if (device) { device->Release(); device = nullptr; }
#elif defined(IMGUI_USE_DX11)
        cleanup_render_target();
        if (swap_chain) { swap_chain->Release(); swap_chain = nullptr; }
        if (device_context) { device_context->Release(); device_context = nullptr; }
        if (device) { device->Release(); device = nullptr; }
#elif defined(IMGUI_USE_DX12)
        wait_for_last_submitted_frame();
        cleanup_render_targets();
        if (swap_chain) { swap_chain->Release(); swap_chain = nullptr; }
        if (command_queue) { command_queue->Release(); command_queue = nullptr; }
        if (command_list) { command_list->Release(); command_list = nullptr; }
        for (FrameContext& frame : frame_context) {
            if (frame.command_allocator) { frame.command_allocator->Release(); frame.command_allocator = nullptr; }
        }
        if (rtv_desc_heap) { rtv_desc_heap->Release(); rtv_desc_heap = nullptr; }
        if (srv_desc_heap) { srv_desc_heap->Release(); srv_desc_heap = nullptr; }
        if (fence) { fence->Release(); fence = nullptr; }
        if (fence_event) { CloseHandle(fence_event); fence_event = nullptr; }
        if (device) { device->Release(); device = nullptr; }
#endif
    }
};

UiContext::UiContext(const UiConfig& config) : m_impl{std::make_unique<Impl>(config)} {}
UiContext::~UiContext() = default;

bool UiContext::should_close() const noexcept {
    return m_impl->done;
}

HWND UiContext::window() const noexcept {
    return m_impl->hwnd;
}

ImGuiIO& UiContext::io() noexcept {
    return ImGui::GetIO();
}

float UiContext::scale() const noexcept {
    return m_impl->main_scale;
}

bool UiContext::begin_frame() {
    return m_impl->begin_frame();
}

void UiContext::end_frame() {
    m_impl->end_frame();
}

void UiContext::load_fonts() {
    m_fonts[FontKind::Roboto] = io().Fonts->AddFontFromFileTTF(".\\assets\\fonts\\Roboto-SemiBold.ttf", 16.0f);
    assert( m_fonts[FontKind::Roboto] != nullptr);
    m_fonts[FontKind::Consolas] = io().Fonts->AddFontFromFileTTF(".\\assets\\fonts\\consola.ttf", 16.0f);
    assert( m_fonts[FontKind::Consolas] != nullptr);
}

ImFont* UiContext::get_font(FontKind kind) {
    return m_fonts[kind];
}