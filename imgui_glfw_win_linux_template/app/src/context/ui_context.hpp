#pragma once
#include <string>
#include <array>
#include <cstdint>
#include "containers/enum_array.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"

enum class FontKind : std::size_t {
    Roboto,
    Consolas,
    COUNT
};

struct UiConfig {
        float width = 1280.0f;
        float height = 925.0f;
        const char* title = "ImGui App";
        const char* glsl_version = "#version 130";

        int gl_major = 3;
        int gl_minor = 0;

        ImVec4 clear_color = ImVec4{0.45f, 0.55f, 0.60f, 1.00f};
};

class UiContext {
    private:
        UiConfig m_config{};

        GLFWwindow* m_window = nullptr;
        float m_main_scale = 1.0f;

        bool m_glfw_initialized = false;
        bool m_imgui_context_created = false;
        bool m_backends_initialized = false;

        using FontsArray = EnumArray<FontKind, ImFont*, static_cast<std::size_t>(FontKind::COUNT)>;
        FontsArray m_fonts;

        void init();
        void shutdown() noexcept;
        void setup_vsync(GLFWmonitor* monitor);
        void setup_styles();

    public:
        explicit UiContext(const UiConfig& config);
        ~UiContext();
        UiContext(const UiContext&) = delete;
        UiContext& operator=(const UiContext&) = delete;
        UiContext(UiContext&&) = delete;
        UiContext& operator=(UiContext&&) = delete;
        

        bool should_close() const noexcept;
        GLFWwindow* window() noexcept;
        ImGuiIO& io() noexcept;
        float scale() const noexcept;
        bool begin_frame();
        void end_frame();
        void load_fonts();
        ImFont* get_font(FontKind kind);
};