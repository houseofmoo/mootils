#include "imgui/imgui.h"

namespace elem {
    class Window {
    public:
        Window(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None) {
            ImGui::Begin(name, nullptr, flags);
        }

        ~Window() {
            ImGui::End();
        }
    };

    class Child {
    public:
        Child(const char* name, const ImVec2& size, ImGuiWindowFlags flags = ImGuiWindowFlags_None) {
            ImGui::BeginChild(name, size, ImGuiChildFlags_AutoResizeY, flags);
        }

        ~Child() {
            ImGui::EndChild();
        }
    };
}