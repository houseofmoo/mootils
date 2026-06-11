#pragma once

#include "imgui/imgui.h"

#define PANEL(name, size, code_block)                                                  \
    ImGui::BeginChild(name, size, ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None); \
    {                                                                                  \
        code_block                                                                     \
    }                                                                                  \
    ImGui::EndChild();

#define PADDING(padding, code_block)                          \
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding); \
    code_block                                                \
    ImGui::PopStyleVar();

// other padding styles
//ImGuiStyleVar_FramePadding — Padding inside widgets (buttons, input boxes, etc.)
//ImGuiStyleVar_ItemSpacing — Space between widgets
//ImGuiStyleVar_WindowPadding — Padding inside windows

#define WINDOW_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, roundness); \
    code_block                                                    \
    ImGui::PopStyleVar();

#define CHILD_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, roundness); \
    code_block                                                   \
    ImGui::PopStyleVar();

#define POPUP_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, roundness); \
    code_block                                                   \
    ImGui::PopStyleVar();

#define FRAME_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, roundness); \
    code_block                                                   \
    ImGui::PopStyleVar();

#define SCROLLBAR_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, roundness); \
    code_block                                                       \
    ImGui::PopStyleVar();

#define GRAB_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, roundness); \
    code_block                                                  \
    ImGui::PopStyleVar();

#define TAB_ROUNDING(roundness, code_block)                    \
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, roundness); \
    code_block                                                 \
    ImGui::PopStyleVar();

    // temporarily change fonts
#define CHANGE_FONT(font, code_block) \
    ImGui::PushFont(font);            \
    code_block                        \
    ImGui::PopFont();

    // align center horizontally
#define ALIGN_CENTER_X(child_width, code_block)                                                             \
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - child_width) * 0.5f); \
    code_block

    // align center vertically
#define ALIGN_CENTER_Y(child_height, code_block)                                                             \
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetContentRegionAvail().y - child_height) * 0.5f); \
    code_block

    // align right
#define ALIGN_RIGHT(child_width, code_block)                                                       \
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - child_width); \
    code_block

    // align bottom
#define ALIGN_BOTTOM(child_height, code_block)                                                      \
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - child_height); \
    code_block

    // shift element right by set width
#define SHIFT_RIGHT(width, code_block)                    \
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + width); \
    code_block

    // change the color of an element
#define COLOR(color, code_block)                 \
    ImGui::PushStyleColor(ImGuiCol_Text, color); \
    code_block                                   \
    ImGui::PopStyleColor();

#define SPACE_AVAILABLE ImGui::GetContentRegionAvail()
#define SPACE_AVAILABLE_X ImGui::GetContentRegionAvail().x
#define SPACE_AVAILABLE_Y ImGui::GetContentRegionAvail().y
#define SPLIT_SPACE_X(split) (SPACE_AVAILABLE.x * split)
#define SPLIT_SPACE_Y(split) (SPACE_AVAILABLE.y * split)
#define EMPTY_SPACE(width, height) ImGui::Dummy(ImVec2(width, height))