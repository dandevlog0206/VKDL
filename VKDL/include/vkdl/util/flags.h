#pragma once

#include<type_traits>

template <typename FlagBitsType>
struct FlagTraits
{
    static VKDL_CONSTEXPR bool isBitmask = false;
};

template <typename BitType>
class Flags
{
public:
    using MaskType = typename std::underlying_type<BitType>::type;

    VKDL_CONSTEXPR Flags() VKDL_NOEXCEPT : mask(0) {}

    VKDL_CONSTEXPR Flags(BitType bit) VKDL_NOEXCEPT : mask(static_cast<MaskType>(bit)) {}

    VKDL_CONSTEXPR Flags(Flags<BitType> const& rhs) VKDL_NOEXCEPT = default;

    VKDL_CONSTEXPR explicit Flags(MaskType flags) VKDL_NOEXCEPT : mask(flags) {}

    VKDL_CONSTEXPR bool operator<(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return mask < rhs.mask;
    }

    VKDL_CONSTEXPR bool operator<=(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return mask <= rhs.mask;
    }

    VKDL_CONSTEXPR bool operator>(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return mask > rhs.mask;
    }

    VKDL_CONSTEXPR bool operator>=(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return mask >= rhs.mask;
    }

    VKDL_CONSTEXPR bool operator==(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return mask == rhs.mask;
    }

    VKDL_CONSTEXPR bool operator!=(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return mask != rhs.mask;
    }

    VKDL_CONSTEXPR bool operator!() const VKDL_NOEXCEPT
    {
        return !mask;
    }

    VKDL_CONSTEXPR Flags<BitType> operator&(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return Flags<BitType>(mask & rhs.mask);
    }

    VKDL_CONSTEXPR Flags<BitType> operator|(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return Flags<BitType>(mask | rhs.mask);
    }

    VKDL_CONSTEXPR Flags<BitType> operator^(Flags<BitType> const& rhs) const VKDL_NOEXCEPT
    {
        return Flags<BitType>(mask ^ rhs.mask);
    }

    VKDL_CONSTEXPR Flags<BitType> operator~() const VKDL_NOEXCEPT
    {
        return Flags<BitType>(mask ^ FlagTraits<BitType>::allFlags.mask);
    }

    VKDL_CONSTEXPR_14 Flags<BitType>& operator=(Flags<BitType> const& rhs) VKDL_NOEXCEPT = default;

    VKDL_CONSTEXPR_14 Flags<BitType>& operator|=(Flags<BitType> const& rhs) VKDL_NOEXCEPT
    {
        mask |= rhs.mask;
        return *this;
    }

    VKDL_CONSTEXPR_14 Flags<BitType>& operator&=(Flags<BitType> const& rhs) VKDL_NOEXCEPT
    {
        mask &= rhs.mask;
        return *this;
    }

    VKDL_CONSTEXPR_14 Flags<BitType>& operator^=(Flags<BitType> const& rhs) VKDL_NOEXCEPT
    {
        mask ^= rhs.mask;
        return *this;
    }

    explicit VKDL_CONSTEXPR operator bool() const VKDL_NOEXCEPT
    {
        return !!mask;
    }

    explicit VKDL_CONSTEXPR operator MaskType() const VKDL_NOEXCEPT
    {
        return mask;
    }

private:
    MaskType mask;
};