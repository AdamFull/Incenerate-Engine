#pragma once
#include <type_traits>

#if (201711 <= __cpp_impl_three_way_comparison) && __has_include(<compare> )
#define HAS_SPACESHIP_OPERATOR
#endif
#if defined(HAS_SPACESHIP_OPERATOR)
#include <compare>
#endif

namespace utl
{
    template <typename FlagBitsType>
    struct FlagTraits
    {
    };

    template <typename BitType>
    class flags
    {
    public:
        using MaskType = typename std::underlying_type<BitType>::type;

        // constructors
        constexpr flags() noexcept : m_mask(0) {}

        constexpr flags(BitType bit) noexcept : m_mask(static_cast<MaskType>(bit)) {}

        constexpr flags(flags<BitType> const &rhs) noexcept = default;

        constexpr flags(MaskType flags) noexcept : m_mask(flags) {}

        // relational operators
#if defined(HAS_SPACESHIP_OPERATOR)
        auto operator<=>(flags<BitType> const &) const = default;
#else
        constexpr bool operator<(flags<BitType> const &rhs) const noexcept
        {
            return m_mask < rhs.m_mask;
        }

        constexpr bool operator<=(flags<BitType> const &rhs) const noexcept
        {
            return m_mask <= rhs.m_mask;
        }

        constexpr bool operator>(flags<BitType> const &rhs) const noexcept
        {
            return m_mask > rhs.m_mask;
        }

        constexpr bool operator>=(flags<BitType> const &rhs) const noexcept
        {
            return m_mask >= rhs.m_mask;
        }

        constexpr bool operator==(flags<BitType> const &rhs) const noexcept
        {
            return m_mask == rhs.m_mask;
        }

        constexpr bool operator!=(flags<BitType> const &rhs) const noexcept
        {
            return m_mask != rhs.m_mask;
        }
#endif

        // logical operator
        constexpr bool operator!() const noexcept
        {
            return !m_mask;
        }

        // bitwise operators
        constexpr flags<BitType> operator&(flags<BitType> const &rhs) const noexcept
        {
            return flags<BitType>(m_mask & rhs.m_mask);
        }

        constexpr flags<BitType> operator|(flags<BitType> const &rhs) const noexcept
        {
            return flags<BitType>(m_mask | rhs.m_mask);
        }

        constexpr flags<BitType> operator^(flags<BitType> const &rhs) const noexcept
        {
            return flags<BitType>(m_mask ^ rhs.m_mask);
        }

        constexpr flags<BitType> operator~() const noexcept
        {
            return flags<BitType>(m_mask ^ FlagTraits<BitType>::allflags);
        }

        // assignment operators
        constexpr flags<BitType> &operator=(flags<BitType> const &rhs) noexcept = default;

        constexpr flags<BitType> &operator|=(flags<BitType> const &rhs) noexcept
        {
            m_mask |= rhs.m_mask;
            return *this;
        }

        constexpr flags<BitType> &operator&=(flags<BitType> const &rhs) noexcept
        {
            m_mask &= rhs.m_mask;
            return *this;
        }

        constexpr flags<BitType> &operator^=(flags<BitType> const &rhs) noexcept
        {
            m_mask ^= rhs.m_mask;
            return *this;
        }

        // cast operators
        explicit constexpr operator bool() const noexcept
        {
            return !!m_mask;
        }

        explicit constexpr operator MaskType() const noexcept
        {
            return m_mask;
        }

    private:
        MaskType m_mask;
    };
}

#if !defined(HAS_SPACESHIP_OPERATOR)
// relational operators only needed for pre C++20
template <typename BitType>
constexpr bool operator<(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator>(bit);
}

template <typename BitType>
constexpr bool operator<=(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator>=(bit);
}

template <typename BitType>
constexpr bool operator>(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator<(bit);
}

template <typename BitType>
constexpr bool operator>=(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator<=(bit);
}

template <typename BitType>
constexpr bool operator==(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator==(bit);
}

template <typename BitType>
constexpr bool operator!=(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator!=(bit);
}
#endif

// bitwise operators
template <typename BitType>
constexpr utl::flags<BitType> operator&(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator&(bit);
}

template <typename BitType>
constexpr utl::flags<BitType> operator|(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator|(bit);
}

template <typename BitType>
constexpr utl::flags<BitType> operator^(BitType bit, utl::flags<BitType> const &flags) noexcept
{
    return flags.operator^(bit);
}
