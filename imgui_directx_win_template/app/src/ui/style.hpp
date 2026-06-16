#include "imgui/imgui.h"
#include <cstdint>

namespace ui::style {
    // generic style font setter
    class StyleFont {
    public:
        explicit StyleFont(ImFont* font) {
            ImGui::PushFont(font);
        }

        ~StyleFont() {
            ImGui::PopFont();
        }

        StyleFont(const StyleFont&) = delete;
        StyleFont& operator=(const StyleFont&) = delete;
        StyleFont(StyleFont&&) = delete;
        StyleFont& operator=(StyleFont&&) = delete;
    };

    // generic style color setter
    class StyleColor {
    public:
        StyleColor(ImGuiCol idx, const ImVec4& color) {
            ImGui::PushStyleColor(idx, color);
        }

        StyleColor(ImGuiCol idx, ImU32 color) {
            ImGui::PushStyleColor(idx, color);
        }

        ~StyleColor() {
            ImGui::PopStyleColor();
        }

        StyleColor(const StyleColor&) = delete;
        StyleColor& operator=(const StyleColor&) = delete;
        StyleColor(StyleColor&&) = delete;
        StyleColor& operator=(StyleColor&&) = delete;
    };

    // generic style value setter
    class StyleVar {
    public:
        explicit StyleVar(std::int32_t idx, float val) {
            ImGui::PushStyleVar(idx, val);
        }

        explicit StyleVar(std::int32_t idx, ImVec2 val) {
            ImGui::PushStyleVar(idx, val);
        }

        ~StyleVar() {
            ImGui::PopStyleVar();
        }

        StyleVar(const StyleVar&) = delete;
        StyleVar& operator=(const StyleVar&) = delete;
        StyleVar(StyleVar&&) = delete;
        StyleVar& operator=(StyleVar&&) = delete;
    };

    // generic style value setter sets visibility based on enabled flag
    class OptionalStyleVar {
    public:
        OptionalStyleVar(bool enabled, ImGuiStyleVar idx, float value) : m_enabled(enabled) {
            if (m_enabled) {
                ImGui::PushStyleVar(idx, value);
            }
        }

        OptionalStyleVar(bool enabled, ImGuiStyleVar idx, ImVec2 value) : m_enabled(enabled) {
            if (m_enabled) {
                ImGui::PushStyleVar(idx, value);
            }
        }

        ~OptionalStyleVar() {
            if (m_enabled) {
                ImGui::PopStyleVar();
            }
        }

        void enable() {
            if (!m_enabled) {
                m_enabled = true;
            }
        }

        void disable() {
            if (m_enabled) {
                m_enabled = false;
            }
        }

        OptionalStyleVar(const OptionalStyleVar&) = delete;
        OptionalStyleVar& operator=(const OptionalStyleVar&) = delete;
        OptionalStyleVar(OptionalStyleVar&&) = delete;
        OptionalStyleVar& operator=(OptionalStyleVar&&) = delete;

    private:
        bool m_enabled = false;
    };
}

namespace ui::style {
    class FramePadding : public StyleVar {
    public:
        explicit FramePadding(const ImVec2& padding) : StyleVar(ImGuiStyleVar_FramePadding, padding) {}
    };

    class WindowRounding : public StyleVar {
    public:
        explicit WindowRounding(float roundness) : StyleVar(ImGuiStyleVar_WindowRounding, roundness) {}
    };

    class ChildRounding : public StyleVar {
    public:
        explicit ChildRounding(float roundness) : StyleVar(ImGuiStyleVar_ChildRounding, roundness) {}
    };

    class PopupRounding : public StyleVar {
    public:
        explicit PopupRounding(float roundness) : StyleVar(ImGuiStyleVar_PopupRounding, roundness) {}
    };

    class FrameRounding : public StyleVar {
    public:
        explicit FrameRounding(float roundness) : StyleVar(ImGuiStyleVar_FrameRounding, roundness) {}
    };

    class ScrollbarRounding : public StyleVar {
    public:
        explicit ScrollbarRounding(float roundness) : StyleVar(ImGuiStyleVar_ScrollbarRounding, roundness) {}
    };

    class GrabRounding : public StyleVar {
    public:
        explicit GrabRounding(float roundness) : StyleVar(ImGuiStyleVar_GrabRounding, roundness) {}
    };

    class TabRounding : public StyleVar {
    public:
        explicit TabRounding(float roundness) : StyleVar(ImGuiStyleVar_TabRounding, roundness) {}
    };
}
