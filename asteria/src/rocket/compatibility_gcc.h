// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ROCKET_COMPATIBILITY_GCC_H_
#define ROCKET_COMPATIBILITY_GCC_H_

// Check for compiler support.
#define ROCKET_ATTRIBUTE_PRINTF(...)        __attribute__((__format__(__gnu_printf__, __VA_ARGS__)))
#define ROCKET_FUNCSIG                      __PRETTY_FUNCTION__
#define ROCKET_UNREACHABLE()                __builtin_unreachable()
#define ROCKET_SELECTANY                    __attribute__((__weak__))
#define ROCKET_EXPECT(...)                  __builtin_expect((bool)(__VA_ARGS__), 1)
#define ROCKET_UNEXPECT(...)                __builtin_expect((bool)(__VA_ARGS__), 0)
#define ROCKET_SECTION(...)                 __attribute__((__section__(__VA_ARGS__)))
#define ROCKET_NOINLINE                     __attribute__((__noinline__))
#define ROCKET_PURE_FUNCTION                __attribute__((__pure__))
#define ROCKET_ARTIFICIAL_FUNCTION          __attribute__((__artificial__, __always_inline__))
#define ROCKET_CONSTANT_P(...)              __builtin_constant_p(__VA_ARGS__)

// Check for libstdc++.
#if defined(_GLIBCXX_DEBUG)
#  define ROCKET_DEBUG                      1
#endif

// Check for libstdc++ from GCC 5.
#if defined(_GLIBCXX14_CONSTEXPR)
#  define ROCKET_HAS_TRIVIALITY_TRAITS      1
#endif

#endif
