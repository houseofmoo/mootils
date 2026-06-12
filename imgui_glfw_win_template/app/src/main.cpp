#include <string>
#include <iostream>
#include <cassert>
#include "imgui/imgui.h"

#include "context/ui_context.hpp"
#include "ui/style.h"
#include "ui/alignment.h"
#include "ui/element.h"

void draw_ui(UiContext& context, ImGuiIO& io) {
    static bool show_demo_window = false;
    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    // menu bar example
    if (ImGui::BeginMenuBar()){
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Do thingA", nullptr, nullptr, false); // disabled example menu item
            ImGui::MenuItem("Exit", nullptr, nullptr, false);      // disabled example menu item
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // child element example
    {
        style::StyleFont font(context.get_font(FontKind::Roboto));
        elem::Child child("demo_panel", ImVec2(300, 100));
        ImGui::Checkbox("Demo Window", &show_demo_window);
    }

    // text child element example
    {
        style::StyleFont font(context.get_font(FontKind::Consolas));
        style::StyleColor color(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
        elem::Child child("consolas_panel", ImVec2(300, 100));
        ImGui::Text("this is a thing");
    }

    // alignment example
    {
        auto height = 16;
        align::NextItemBottomY(static_cast<float>(height));
        ImGui::Text("FPS: %.1f", static_cast<double>(io.Framerate));
        ImGui::SameLine();
#ifndef NDEBUG
        ImGui::Text("DEBUG");
#else
        ImGui::Text("RELEASE");
#endif
    }
}

int main() {
    {
        UiContext context({
            1280.0f,
            925.0f,
            "MainWindow"
        });

        context.load_fonts();
        ImGuiIO& io = context.io();

        while (!context.should_close()) {
            if (!context.begin_frame()) { continue; }

            {
                elem::BaseWindow background{io};
                draw_ui(context, io);
            }

            context.end_frame();
        }
    }
    return 0;
}