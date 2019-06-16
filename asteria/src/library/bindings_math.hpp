// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_LIBRARY_BINDINGS_MATH_HPP_
#define ASTERIA_LIBRARY_BINDINGS_MATH_HPP_

#include "../fwd.hpp"

namespace Asteria {

extern G_real std_math_exp(const G_real& y, const Opt<G_real>& base);
extern G_real std_math_expm1(const G_real& y);
extern G_real std_math_pow(const G_real& x, const G_real& y);
extern G_real std_math_log(const G_real& x, const Opt<G_real>& base);
extern G_real std_math_log1p(const G_real& x);

extern G_real std_math_sin(const G_real& x);
extern G_real std_math_cos(const G_real& x);
extern G_real std_math_tan(const G_real& x);
extern G_real std_math_asin(const G_real& x);
extern G_real std_math_acos(const G_real& x);
extern G_real std_math_atan(const G_real& x);
extern G_real std_math_atan2(const G_real& y, const G_real& x);

extern G_real std_math_hypot(const Cow_Vector<Value>& values);

extern G_real std_math_sinh(const G_real& x);
extern G_real std_math_cosh(const G_real& x);
extern G_real std_math_tanh(const G_real& x);
extern G_real std_math_asinh(const G_real& x);
extern G_real std_math_acosh(const G_real& x);
extern G_real std_math_atanh(const G_real& x);

extern G_real std_math_erf(const G_real& x);
extern G_real std_math_cerf(const G_real& x);
extern G_real std_math_gamma(const G_real& x);
extern G_real std_math_lgamma(const G_real& x);

// Create an object that is to be referenced as `std.math`.
extern void create_bindings_math(G_object& result, API_Version version);

}  // namespace Asteria

#endif
