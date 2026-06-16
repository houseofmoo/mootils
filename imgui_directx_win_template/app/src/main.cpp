#include "context/ui_context.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include "ui/alignment.hpp"
#include "ui/style.hpp"
#include "ui/window.hpp"

#include <string>

void draw_ui(UiContext& context, ImGuiIO& io) {
    static bool show_demo_window = false;
    static std::string text = "Edit me";
    static float value = 0.5f;

    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Do thing A", nullptr, nullptr, false);
            ImGui::MenuItem("Exit", nullptr, nullptr, false);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    {
        ui::style::StyleFont font(context.get_font(FontKind::Roboto));
        ui::window::Child child("demo_panel", ImVec2(360, 140));
        ImGui::TextUnformatted("Win32 + DirectX Dear ImGui starter");
#if defined(IMGUI_USE_DX9)
        ImGui::TextUnformatted("Backend: DirectX 9");
#elif defined(IMGUI_USE_DX10)
        ImGui::TextUnformatted("Backend: DirectX 10");
#elif defined(IMGUI_USE_DX11)
        ImGui::TextUnformatted("Backend: DirectX 11");
#elif defined(IMGUI_USE_DX12)
        ImGui::TextUnformatted("Backend: DirectX 12");
#endif
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::InputText("Text", &text);
        ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);
    }

    {
        ui::style::StyleFont font(context.get_font(FontKind::Consolas));
        constexpr int height = 16;
        ui::align::NextItemBottomY(static_cast<float>(height));
        ImGui::Text("FPS: %.1f", static_cast<double>(io.Framerate));
        ImGui::SameLine();
#ifndef NDEBUG
        ImGui::TextUnformatted("DEBUG");
#else
        ImGui::TextUnformatted("RELEASE");
#endif
    }
}

int main() {
    UiContext context({
        1280,
        925,
        L"MainWindow",
        true,
        ImVec4{0.45f, 0.55f, 0.60f, 1.00f}
    });

    context.load_fonts();
    ImGuiIO& io = context.io();

    while (!context.should_close()) {
        if (!context.begin_frame()) {
            continue;
        }

        {
            ui::window::BaseWindow background{io};
            draw_ui(context, io);
        }

        context.end_frame();
    }

    return 0;
}
