#include "imgui/imgui.h"

namespace ui::window {
    class BaseWindow {
        private:
            bool m_drawn;

        public:
        BaseWindow(const ImGuiIO& io) {
            // set the next window size to the full display size
            ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

            // set the next window position to the top-left corner
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
            m_drawn = ImGui::Begin(
                "MainWindow",
                nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoBringToFrontOnFocus |
                ImGuiWindowFlags_MenuBar
            );
        }

        ~BaseWindow() {
            if (m_drawn) ImGui::End();
        }

        BaseWindow(const BaseWindow&) = delete;
        BaseWindow& operator=(const BaseWindow&) = delete;
        BaseWindow(BaseWindow&&) = delete;
        BaseWindow& operator=(BaseWindow&&) = delete;

    };

    class Window {
    private:
            bool m_drawn;

    public:
        Window(const char* name, ImGuiWindowFlags flags = ImGuiWindowFlags_None) {
            m_drawn = ImGui::Begin(name, nullptr, flags);
        }

        ~Window() {
            if (m_drawn) ImGui::End();
        }

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;
    };

    class Child {
    private:
            bool m_drawn;
    public:
        Child(const char* name, const ImVec2& size, ImGuiWindowFlags flags = ImGuiWindowFlags_None) {
            m_drawn = ImGui::BeginChild(name, size, ImGuiChildFlags_AutoResizeY, flags);
        }

        ~Child() {
            if (m_drawn) ImGui::EndChild();
        }

        Child(const Child&) = delete;
        Child& operator=(const Child&) = delete;
        Child(Child&&) = delete;
        Child& operator=(Child&&) = delete;
    };
}