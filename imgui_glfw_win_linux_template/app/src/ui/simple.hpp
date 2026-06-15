#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

namespace ui::simple {
    class SimpleButton {
    private:
        std::string m_title;
        std::function<void()> m_on_clicked;

    public:
        SimpleButton(std::string title)
            : m_title(std::move(title)) {}
        SimpleButton(std::string title, std::function<void()> on_clicked)
            : m_title(std::move(title)), m_on_clicked(std::move(on_clicked)) {}

        void draw() {
            if (ImGui::Button(m_title.c_str())) {
                if (m_on_clicked) {
                    m_on_clicked();
                }
            }
        }

        void set_on_clicked(std::function<void()> callback) {
            m_on_clicked = std::move(callback);
        }
    };

    class SimpleCheckbox {
    private:
        std::string m_title;
        bool& m_value;
        std::function<void(bool)> m_on_value_changed;

    public:
        SimpleCheckbox(std::string title, bool& value)
            : m_title(std::move(title)), m_value(value) {}
        SimpleCheckbox(std::string title, bool& value, std::function<void(bool)> on_value_changed)
            : m_title(std::move(title)), m_value(value), m_on_value_changed(std::move(on_value_changed)) {}

        void draw() {
            if (ImGui::Checkbox(m_title.c_str(), &m_value)) {
                if (m_on_value_changed) {
                    m_on_value_changed(m_value);
                }
            }
        }

        void set_on_value_changed(std::function<void(bool)> callback) {
            m_on_value_changed = std::move(callback);
        }
    };

    class SimpleInputText {
    private:
        std::string m_title;
        std::string& m_value;
        std::function<void(const std::string&)> m_on_text_changed;

    public:
    SimpleInputText(std::string title, std::string& value)
        : m_title(std::move(title)), m_value(value) {}
    SimpleInputText(std::string title, std::string& value, std::function<void(const std::string&)> on_text_changed)
        : m_title(std::move(title)), m_value(value), m_on_text_changed(std::move(on_text_changed)) {}

        void draw() {
            if (ImGui::InputText(m_title.c_str(), &m_value)) {
                if (m_on_text_changed) {
                    m_on_text_changed(m_value);
                }
            }
        }

        void set_on_text_changed(std::function<void(const std::string&)> callback) {
            m_on_text_changed = std::move(callback);
        }
    };

    class SimpleInputInt {
    private:
        std::string m_title;
        int& m_value;
        std::function<void(int)> m_on_value_changed;

    public:
        SimpleInputInt(std::string title, int& value)
            : m_title(std::move(title)), m_value(value) {}
        SimpleInputInt(std::string title, int& value, std::function<void(int)> on_value_changed)
            : m_title(std::move(title)), m_value(value), m_on_value_changed(std::move(on_value_changed)) {}

        void draw() {
            if (ImGui::InputInt(m_title.c_str(), &m_value)) {
                if (m_on_value_changed) {
                    m_on_value_changed(m_value);
                }
            }
        }

        void set_on_value_changed(std::function<void(int)> callback) {
            m_on_value_changed = std::move(callback);
        }
    };

    class SimpleInputUInt32 {
    private:
        std::string m_title;
        std::uint32_t& m_value;
        std::function<void(std::uint32_t)> m_on_value_changed;

    public:
        SimpleInputUInt32(std::string title, std::uint32_t& value)
            : m_title(std::move(title)), m_value(value) {}
        SimpleInputUInt32(std::string title, std::uint32_t& value, std::function<void(std::uint32_t)> callback)
            : m_title(std::move(title)), m_value(value), m_on_value_changed(std::move(callback)) {}

        void draw() {
            int val = static_cast<int>(m_value);
            if (ImGui::InputInt(m_title.c_str(), &val)) {
                if (val >= 0) {
                    m_value = static_cast<std::uint32_t>(val);
                    if (m_on_value_changed) {
                        m_on_value_changed(m_value);
                    }
                }
            }
        }

        void set_on_value_changed(std::function<void(std::uint32_t)> callback) {
            m_on_value_changed = std::move(callback);
        }
    };

    class SimpleInputFloat {
    private:
        std::string m_title;
        float& m_value;
        std::function<void(float)> m_on_value_changed;

    public:
        SimpleInputFloat(std::string title, float& value)
            : m_title(std::move(title)), m_value(value) {}
        SimpleInputFloat(std::string title, float& value, std::function<void(float)> on_value_changed)
            : m_title(std::move(title)), m_value(value), m_on_value_changed(std::move(on_value_changed)) {}

        void draw() {
            if (ImGui::InputFloat(m_title.c_str(), &m_value)) {
                if (m_on_value_changed) {
                    m_on_value_changed(m_value);
                }
            }
        }

        void set_on_value_changed(std::function<void(float)> callback) {
            m_on_value_changed = std::move(callback);
        }
    };

    class SimpleSliderInt {
    private:
        std::string m_title;
        int& m_value;
        int m_min;
        int m_max;
        std::function<void(int)> m_on_value_changed;

    public:
        SimpleSliderInt(std::string title, int& value, int min, int max)
            : m_title(std::move(title)),
              m_value(value),
              m_min(min),
              m_max(max) {}
        SimpleSliderInt(std::string title, int& value, int min, int max, std::function<void(int)> on_value_changed)
            : m_title(std::move(title)),
              m_value(value),
              m_min(min),
              m_max(max),
              m_on_value_changed(std::move(on_value_changed)) {}

        void draw() {
            if (ImGui::SliderInt(m_title.c_str(), &m_value, m_min, m_max)) {
                if (m_on_value_changed) {
                    m_on_value_changed(m_value);
                }
            }
        }

        void set_on_value_changed(std::function<void(int)> callback) {
            m_on_value_changed = std::move(callback);
        }
    };

    class SimpleSliderFloat {
    private:
        std::string m_title;
        float& m_value;
        float m_min;
        float m_max;
        std::function<void(float)> m_on_value_changed;

    public:
        SimpleSliderFloat(std::string title, float& value, float min, float max)
            : m_title(std::move(title)),
              m_value(value),
              m_min(min),
              m_max(max) {}
        SimpleSliderFloat(std::string title, float& value, float min, float max, std::function<void(float)> on_value_changed)
            : m_title(std::move(title)),
              m_value(value),
              m_min(min),
              m_max(max),
              m_on_value_changed(std::move(on_value_changed)) {}


        void draw() {
            if (ImGui::SliderFloat(m_title.c_str(), &m_value, m_min, m_max)) {
                if (m_on_value_changed) {
                    m_on_value_changed(m_value);
                }
            }
        }

        void set_on_value_changed(std::function<void(float)> callback) {
            m_on_value_changed = std::move(callback);
        }
    };

    class SimpleComboBox {
    private:
        std::string m_title;
        std::vector<std::string> m_items;
        std::size_t& m_selected_idx;
        std::function<void(std::size_t)> m_on_selected_index_changed;

    public:
        SimpleComboBox(
            std::string title,
            std::vector<std::string> items,
            std::size_t& selected_idx
        )
            : m_title(std::move(title)),
              m_items(std::move(items)),
              m_selected_idx(selected_idx) {}
        SimpleComboBox(
            std::string title,
            std::vector<std::string> items,
            std::size_t& selected_idx,
            std::function<void(std::size_t)> on_selected_index_changed
        )
            : m_title(std::move(title)),
              m_items(std::move(items)),
              m_selected_idx(selected_idx),
              m_on_selected_index_changed(std::move(on_selected_index_changed)) {}

        void draw() {
            const char* preview = "";

            if (!m_items.empty() && m_selected_idx < m_items.size()) {
                preview = m_items[m_selected_idx].c_str();
            }

            if (ImGui::BeginCombo(m_title.c_str(), preview)) {
                for (std::size_t i = 0; i < m_items.size(); ++i) {
                    const bool is_selected = m_selected_idx == i;

                    if (ImGui::Selectable(m_items[i].c_str(), is_selected)) {
                        if (m_selected_idx != i) {
                            m_selected_idx = i;

                            if (m_on_selected_index_changed) {
                                m_on_selected_index_changed(m_selected_idx);
                            }
                        }
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
        }

        void set_on_selected_index_changed(std::function<void(std::size_t)> callback) {
            m_on_selected_index_changed = std::move(callback);
        }
    };

    class SimpleListBox {
    private:
        std::string m_title;
        std::vector<std::string> m_items;
        std::size_t& m_selected_idx;
        std::function<void(std::size_t)> m_on_selected_index_changed;

    public:
        SimpleListBox(
            std::string title,
            std::vector<std::string> items,
            std::size_t& selected_idx
        )
            : m_title(std::move(title)),
              m_items(std::move(items)),
              m_selected_idx(selected_idx) {}
        SimpleListBox(
            std::string title,
            std::vector<std::string> items,
            std::size_t& selected_idx,
            std::function<void(std::size_t)> on_selected_index_changed
        )
            : m_title(std::move(title)),
              m_items(std::move(items)),
              m_selected_idx(selected_idx),
              m_on_selected_index_changed(std::move(on_selected_index_changed)) {}

        void draw() {
            if (ImGui::BeginListBox(m_title.c_str())) {
                for (std::size_t i = 0; i < m_items.size(); i++) {
                    const bool is_selected = m_selected_idx == i;

                    if (ImGui::Selectable(m_items[i].c_str(), is_selected)) {
                        if (m_selected_idx != i) {
                            m_selected_idx = i;

                            if (m_on_selected_index_changed) {
                                m_on_selected_index_changed(m_selected_idx);
                            }
                        }
                    }

                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndListBox();
            }
        }

        void set_on_selected_index_changed(std::function<void(std::size_t)> callback) {
            m_on_selected_index_changed = std::move(callback);
        }
    };

    class SimpleRadioButtonGroup {
    private:
        std::string m_title;
        std::vector<std::string> m_items;
        int& m_selected_idx;
        std::function<void(int)> m_on_selected_index_changed;

    public:
        SimpleRadioButtonGroup(
            std::string title,
            std::vector<std::string> items,
            int& selected_idx
        )
            : m_title(std::move(title)),
              m_items(std::move(items)),
              m_selected_idx(selected_idx) {}
        SimpleRadioButtonGroup(
            std::string title,
            std::vector<std::string> items,
            int& selected_idx,
            std::function<void(int)> on_selected_index_changed
        )
            : m_title(std::move(title)),
              m_items(std::move(items)),
              m_selected_idx(selected_idx),
              m_on_selected_index_changed(std::move(on_selected_index_changed)) {}

        void draw() {
            ImGui::TextUnformatted(m_title.c_str());

            for (int i = 0; i < static_cast<int>(m_items.size()); ++i) {
                std::size_t idx = static_cast<std::size_t>(i);
                if (ImGui::RadioButton(m_items[idx].c_str(), &m_selected_idx, i)) {
                    if (m_on_selected_index_changed) {
                        m_on_selected_index_changed(m_selected_idx);
                    }
                }
            }
        }

        void set_on_selected_index_changed(std::function<void(int)> callback) {
            m_on_selected_index_changed = std::move(callback);
        }
    };

    class SimpleText {
    private:
        std::string m_text;

    public:
        explicit SimpleText(std::string text)
            : m_text(std::move(text)) {}

        void draw() {
            ImGui::TextUnformatted(m_text.c_str());
        }

        void set_text(std::string text) {
            m_text = std::move(text);
        }
    };

    class SimpleSeparator {
    public:
        void draw() {
            ImGui::Separator();
        }
    };
}