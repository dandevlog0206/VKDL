#pragma once

#ifdef _WIN32
#  define VKDL_PLATFORM_WINDOWS
#endif

#if defined( __cpp_constexpr )
#  define VKDL_CONSTEXPR constexpr
#  if 201304 <= __cpp_constexpr
#    define VKDL_CONSTEXPR_14 constexpr
#  else
#    define VKDL_CONSTEXPR_14
#  endif
#  if ( 201907 <= __cpp_constexpr ) && ( !defined( __GNUC__ ) || ( 110400 < GCC_VERSION ) )
#    define VKDL_CONSTEXPR_20 constexpr
#  else
#    define VKDL_CONSTEXPR_20
#  endif
#  define VKDL_CONST_OR_CONSTEXPR constexpr
#else
#  define VKDL_CONSTEXPR
#  define VKDL_CONSTEXPR_14
#  define VKDL_CONST_OR_CONSTEXPR const
#endif

#define VKDL_INLINE inline
#define VKDL_NOEXCEPT noexcept

#define VKDL_NAMESPACE_NAME vkdl
#define VKDL_PRIV_NAMESPACE_NAME priv

#define VKDL_BEGIN namespace VKDL_NAMESPACE_NAME {
#define VKDL_END }
#define VKDL_PRIV_BEGIN namespace VKDL_PRIV_NAMESPACE_NAME {
#define VKDL_PRIV_END }

#define VKDL_NOCOPY(cls) cls(const cls&) = delete;
#define VKDL_NOMOVE(cls) cls(cls&&) = delete;
#define VKDL_NOCOPYASS(cls) cls& operator=(const cls&) = delete;
#define VKDL_NOMOVEASS(cls) cls& operator=(cls&&) = delete;