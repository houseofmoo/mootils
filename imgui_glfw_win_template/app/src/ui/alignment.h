#include "imgui/imgui.h"

namespace align {
    inline void Empty(float width, float height) {
        ImGui::Dummy(ImVec2(width, height));
    }

    inline void NextItemLeftX() {
        // no op as left is default
    }

    inline void NextItemCenterX(float item_width) {
        float avail = ImGui::GetContentRegionAvail().x;
        float x = ImGui::GetCursorPosX() + (avail - item_width) * 0.5f;

        if (x > ImGui::GetCursorPosX()) {
            ImGui::SetCursorPosX(x);
        }
    }

    inline void NextItemRightX(float item_width) {
        float avail = ImGui::GetContentRegionAvail().x;
        float x = ImGui::GetCursorPosX() + avail - item_width;

        if (x > ImGui::GetCursorPosX()) {
            ImGui::SetCursorPosX(x);
        }
    }

    inline void ShiftItemRight(float item_width, float spacing = 0.0f) {
        float x = ImGui::GetCursorPosX() + item_width + spacing;
        ImGui::SetCursorPosX(x);
    }

    inline void NextItemTopY() {
        // no op as top is default
    }

    inline void NextItemBottomY(float item_height) {
        float avail = ImGui::GetContentRegionAvail().y;
        float y = ImGui::GetCursorPosY() + avail - item_height;

        if (y > ImGui::GetCursorPosY()) {
            ImGui::SetCursorPosY(y);
        }
    }

    inline void NextItemCenterY(float item_height) {
        float avail = ImGui::GetContentRegionAvail().y;
        float y = ImGui::GetCursorPosY() + (avail - item_height) * 0.5f;

        if (y > ImGui::GetCursorPosY()) {
            ImGui::SetCursorPosY(y);
        }
    }

    inline void ShiftItemDown(float item_height, float spacing = 0.0f) {
        float y = ImGui::GetCursorPosY() + item_height + spacing;
        ImGui::SetCursorPosY(y);
    }

    enum class HAlign {
        Left,
        Center,
        Right
    };

    enum class VAlign {
        Top,
        Center,
        Bottom
    };

    inline void AlignNextX(float item_width, HAlign align) {
        switch (align) {
            case HAlign::Left:
                NextItemLeftX();
                break;

            case HAlign::Center:
                NextItemCenterX(item_width);
                break;

            case HAlign::Right:
                NextItemRightX(item_width);
                break;
        }
    }

    inline void AlignNextY(float item_height, VAlign align) {
        switch (align) {
            case VAlign::Top:
                NextItemTopY();
                break;

            case VAlign::Center:
                NextItemCenterY(item_height);
                break;

            case VAlign::Bottom:
                NextItemBottomY(item_height);
                break;
        }
    }

    inline void AlignNext(ImVec2 item_size, HAlign h_align, VAlign v_align) {
        const ImVec2 cursor = ImGui::GetCursorPos();
        const ImVec2 avail = ImGui::GetContentRegionAvail();

        ImVec2 pos = cursor;

        switch (h_align) {
            case HAlign::Left:
                pos.x = cursor.x;
                break;

            case HAlign::Center:
                pos.x = cursor.x + (avail.x - item_size.x) * 0.5f;
                break;

            case HAlign::Right:
                pos.x = cursor.x + avail.x - item_size.x;
                break;
        }

        switch (v_align) {
            case VAlign::Top:
                pos.y = cursor.y;
                break;

            case VAlign::Center:
                pos.y = cursor.y + (avail.y - item_size.y) * 0.5f;
                break;

            case VAlign::Bottom:
                pos.y = cursor.y + avail.y - item_size.y;
                break;
        }

        if (pos.x < cursor.x) {
            pos.x = cursor.x;
        }

        if (pos.y < cursor.y) {
            pos.y = cursor.y;
        }

        ImGui::SetCursorPos(pos);
    }
}