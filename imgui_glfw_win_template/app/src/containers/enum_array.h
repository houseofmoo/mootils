#include <cstdint>
#include <array>
#include <cassert>

template <typename Enum, typename T, std::size_t N>
class EnumArray {
    static_assert(std::is_enum_v<Enum>, "EnumArray index type must be an enum");

    private:
        std::array<T, N> m_data;

        static constexpr std::size_t to_index(Enum e) noexcept {
            return static_cast<std::size_t>(
                static_cast<std::underlying_type_t<Enum>>(e)
            );
        }

    public:
        constexpr std::size_t size() const noexcept {
            return N;
        }

        constexpr T* data() noexcept {
            return m_data.data();
        }

        constexpr const T* data() const noexcept {
            return m_data.data();
        }

        constexpr auto begin() noexcept {
            return m_data.begin();
        }

        constexpr auto end() noexcept {
            return m_data.end();
        }

        constexpr auto begin() const noexcept {
            return m_data.begin();
        }

        constexpr auto end() const noexcept {
            return m_data.end();
        }

        constexpr T& operator[](Enum e) noexcept {
            auto index = to_index(e);
            assert(index < N);
            return m_data[index];
        }

        constexpr const T& operator[](Enum e) const noexcept {
            auto index = to_index(e);
            assert(index < N);
            return m_data[index];
        }
};