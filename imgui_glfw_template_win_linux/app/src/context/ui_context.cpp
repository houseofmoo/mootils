#include "ui_context.hpp"

#include <iostream>
#include <cassert>
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

static void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

UiContext::UiContext(const UiConfig& config) : m_config{config} {
    init();
}

UiContext::~UiContext() {
    shutdown();
}

void UiContext::init() {
    glfwSetErrorCallback(glfw_error_callback);

    const int glfw_ok = glfwInit();
    assert(glfw_ok != 0);
    m_glfw_initialized = true;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_config.gl_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_config.gl_minor);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    assert(monitor != nullptr);

    m_main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(monitor); // valid on GLFW 3.3+ only
    m_window = glfwCreateWindow(
        static_cast<int>(m_config.width * m_main_scale), 
        static_cast<int>(m_config.height * m_main_scale), 
        m_config.title, nullptr, nullptr
    );
    assert(m_window != nullptr);

    glfwMakeContextCurrent(m_window);
    setup_vsync(monitor);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_imgui_context_created = true;

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // enable keyboard controls

    setup_styles();

    const bool glfw_backend_ok = ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    assert(glfw_backend_ok);

    const bool opengl_backend_ok = ImGui_ImplOpenGL3_Init(m_config.glsl_version);
    assert(opengl_backend_ok);

    m_backends_initialized = true;
}

void UiContext::shutdown() noexcept {
    if (m_backends_initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        m_backends_initialized = false;
    }

    if (m_imgui_context_created) {
        ImGui::DestroyContext();
        m_imgui_context_created = false;
    }

    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    if (m_glfw_initialized) {
        glfwTerminate();
        m_glfw_initialized = false;
    }
}

void UiContext::setup_vsync(GLFWmonitor* monitor) {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    switch (mode->refreshRate) {
        case 240:
            glfwSwapInterval(4);
            break;

        case 144:
        case 120:
            glfwSwapInterval(2);
            break;

        case 60:
        default:
            // if we cannot detect refresh rate, set default of 1
            glfwSwapInterval(1);
            break;
    }
}

void UiContext::setup_styles() {
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.ScaleAllSizes(m_main_scale);
    style.FontScaleDpi = m_main_scale;

    style.WindowRounding = 6.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarSize = 12.0f;
}

bool UiContext::should_close() const noexcept {
    return glfwWindowShouldClose(m_window);
}

GLFWwindow* UiContext::window() noexcept {
    return m_window;
}

ImGuiIO& UiContext::io() noexcept {
    return ImGui::GetIO();
    }

float UiContext::scale() const noexcept {
    return m_main_scale;
}

bool UiContext::begin_frame() {
    glfwPollEvents();
    if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        return false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    return true;
}

void UiContext::end_frame() {
    ImGui::Render();

    int display_w = 0; 
    int display_h = 0;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(
        m_config.clear_color.x,
        m_config.clear_color.y,
        m_config.clear_color.z,
        m_config.clear_color.w
    );
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
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