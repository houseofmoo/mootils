#pragma once

#include <memory>
#include <cstdint>
#include <windows.h>
#include "imgui/imgui.h"
#include "containers/enum_array.hpp"

enum class FontKind : std::size_t {
    Roboto,
    Consolas,
    COUNT
};

struct UiConfig {
    int width = 1280;
    int height = 925;
    const wchar_t* title = L"ImGui App";
    bool vsync = true;
    ImVec4 clear_color = ImVec4{0.45f, 0.55f, 0.60f, 1.00f};
};

class UiContext {
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    using FontsArray = EnumArray<FontKind, ImFont*, static_cast<std::size_t>(FontKind::COUNT)>;
    FontsArray m_fonts;

public:
    explicit UiContext(const UiConfig& config);
    ~UiContext();

    UiContext(const UiContext&) = delete;
    UiContext& operator=(const UiContext&) = delete;
    UiContext(UiContext&&) = delete;
    UiContext& operator=(UiContext&&) = delete;

    bool should_close() const noexcept;
    HWND window() const noexcept;
    ImGuiIO& io() noexcept;
    float scale() const noexcept;

    bool begin_frame();
    void end_frame();
    void load_fonts();
    ImFont* get_font(FontKind kind);
};
