// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "ascii_numput.hpp"
#include "assert.hpp"
#include <cmath>

namespace rocket {

ascii_numput& ascii_numput::put_TB(bool value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Get the template string literal. The string is immutable.
    if(value) {
      bp = const_cast<char*>("true");
      ep = bp + 4;
    }
    else {
      bp = const_cast<char*>("false");
      ep = bp + 5;
    }
    // Set the string. The internal storage is unused.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

    namespace {

    template<typename valueT>
        constexpr typename make_unsigned<valueT>::type do_cast_U(const valueT& value) noexcept
      {
        return static_cast<typename make_unsigned<valueT>::type>(value);
      }

    constexpr char do_pdigit_D(uint8_t dval) noexcept
      {
        return static_cast<char>('0' + dval);
      }
    constexpr char do_pdigit_X(uint8_t dval) noexcept
      {
        return static_cast<char>('0' + dval + (static_cast<uint8_t>(9 - dval) >> 5));
      }

    void do_xput_U_bkwd(char*& bp, const uint64_t& value, uint8_t base, size_t precision) noexcept
      {
        char* fp = bp - precision;
        uint64_t ireg = value;
        // Write digits backwards.
        while(ireg != 0) {
          // Shift a digit out.
          uint8_t dval = static_cast<uint8_t>(ireg % base);
          ireg /= base;
          // Write this digit.
          *(--bp) = do_pdigit_X(dval);
        }
        // Pad the string to at least the precision requested.
        while(bp > fp)
          *(--bp) = '0';
      }

    }

ascii_numput& ascii_numput::put_XP(const void* value) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Write digits backwards.
    do_xput_U_bkwd(bp, reinterpret_cast<uintptr_t>(value), 16, sizeof(value) * 2);
    // Prepend the hexadecimal prefix.
    *(--bp) = 'x';
    *(--bp) = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_BU(uint64_t value, size_t precision) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Write digits backwards.
    do_xput_U_bkwd(bp, value, 2, precision);
    // Prepend the binary prefix.
    *(--bp) = 'b';
    *(--bp) = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_XU(uint64_t value, size_t precision) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Write digits backwards.
    do_xput_U_bkwd(bp, value, 16, precision);
    // Prepend the hexadecimal prefix.
    *(--bp) = 'x';
    *(--bp) = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_DU(uint64_t value, size_t precision) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Write digits backwards.
    do_xput_U_bkwd(bp, value, 10, precision);
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_BI(int64_t value, size_t precision) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint64_t sign = do_cast_U(value >> 63);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd(bp, (do_cast_U(value) ^ sign) - sign, 2, precision);
    // Prepend the binary prefix.
    *(--bp) = 'b';
    *(--bp) = '0';
    // If the number is negative, prepend a minus sign.
    if(sign)
      *(--bp) = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_XI(int64_t value, size_t precision) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint64_t sign = do_cast_U(value >> 63);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd(bp, (do_cast_U(value) ^ sign) - sign, 16, precision);
    // Prepend the hexadecimal prefix.
    *(--bp) = 'x';
    *(--bp) = '0';
    // If the number is negative, prepend a minus sign.
    if(sign)
      *(--bp) = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_DI(int64_t value, size_t precision) noexcept
  {
    this->clear();
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Append a null terminator.
    *bp = 0;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint64_t sign = do_cast_U(value >> 63);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd(bp, (do_cast_U(value) ^ sign) - sign, 10, precision);
    // If the number is negative, prepend a minus sign.
    if(sign)
      *(--bp) = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

    namespace {

    template<typename valueT>
        char* do_check_special(char*& bp, char*& ep, const valueT& value) noexcept
      {
        // Note that this function returns a pointer to immutable strings.
        int cls = ::std::fpclassify(value);
        if(cls == FP_INFINITE) {
          bp = const_cast<char*>("-infinity");
          ep = bp + 9;
          return bp;
        }
        if(cls == FP_NAN) {
          bp = const_cast<char*>("-nan");
          ep = bp + 4;
          return bp;
        }
        if(cls == FP_ZERO) {
          bp = const_cast<char*>("-0");
          ep = bp + 2;
          return bp;
        }
        // Return a null pointer to indicate that the value is finite.
        return nullptr;
      }

    void do_xfrexp_F_bin(uint64_t& mant, int& exp, const double& value) noexcept
      {
        // Note if `value` is not finite then the behavior is undefined.
        int bexp;
        double frac = ::std::frexp(::std::fabs(value), &bexp);
        exp = bexp - 1;
        mant = static_cast<uint64_t>(static_cast<int64_t>(frac * 0x1p53)) << 11;
      }

    void do_xput_M_bin(char*& ep, const uint64_t& mant, const char* rdxp) noexcept
      {
        // Write digits in normal order.
        uint64_t ireg = mant;
        while(ireg != 0) {
          // Shift a digit out.
          uint8_t dval = static_cast<uint8_t>(ireg >> 63);
          ireg <<= 1;
          // Insert a decimal point before `rdxp`.
          if(ep == rdxp)
            *(ep++) = '.';
          // Write this digit.
          *(ep++) = do_pdigit_D(dval);
        }
        // If `rdxp` is set, fill zeroes until it is reached,
        // if no decimal point has been added so far.
        if(rdxp)
          while(ep < rdxp)
            *(ep++) = '0';
      }

    void do_xput_M_hex(char*& ep, const uint64_t& mant, const char* rdxp) noexcept
      {
        // Write digits in normal order.
        uint64_t ireg = mant;
        while(ireg != 0) {
          // Shift a digit out.
          uint8_t dval = static_cast<uint8_t>(ireg >> 60);
          ireg <<= 4;
          // Insert a decimal point before `rdxp`.
          if(ep == rdxp)
            *(ep++) = '.';
          // Write this digit.
          *(ep++) = do_pdigit_X(dval);
        }
        // If `rdxp` is set, fill zeroes until it is reached,
        // if no decimal point has been added so far.
        if(rdxp)
          while(ep < rdxp)
            *(ep++) = '0';
      }

    char* do_xput_I_exp(char*& ep, const int& exp) noexcept
      {
        // Append the sign symbol, always.
        if(exp < 0)
          *(ep++) = '-';
        else
          *(ep++) = '+';
        // Write the exponent in this temporary storage backwards.
        // The exponent shall contain at least two digits.
        char temps[8];
        char* tbp = end(temps);
        do_xput_U_bkwd(tbp, do_cast_U(::std::abs(exp)), 10, 2);
        // Append the exponent.
        noadl::ranged_for(tbp, end(temps), [&](const char* p) { *(ep++) = *p;  });
        return ep;
      }

    }  // namespace

ascii_numput& ascii_numput::put_BF(double value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += do_cast_U(sign + 1);
    }
    else {
      // Seek to the beginning of the internal buffer.
      bp = this->m_stor;
      ep = bp;
      // Prepend a minus sign if the number is negative.
      if(sign)
        *(ep++) = '-';
      // Prepend the binary prefix.
      *(ep++) = '0';
      *(ep++) = 'b';
      // Break the number down into fractional and exponential parts. This result is exact.
      uint64_t mant;
      int exp;
      do_xfrexp_F_bin(mant, exp, value);
      // Write the broken-down number...
      if((exp < -4) || (53 <= exp)) {
        // ... in scientific notation.
        do_xput_M_bin(ep, mant, ep + 1);
        *(ep++) = 'p';
        do_xput_I_exp(ep, exp);
      }
      else if(exp < 0) {
        // ... in plain format; the number starts with "0."; zeroes are prepended as necessary.
        *(ep++) = '0';
        *(ep++) = '.';
        noadl::ranged_for(exp, -1, [&](int) { *(ep++) = '0';  });
        do_xput_M_bin(ep, mant, nullptr);
      }
      else {
        // ... in plain format; the decimal point is inserted in the middle.
        do_xput_M_bin(ep, mant, ep + 1 + do_cast_U(exp));
      }
      // Append a null terminator.
      *ep = 0;
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_BE(double value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += do_cast_U(sign + 1);
    }
    else {
      // Seek to the beginning of the internal buffer.
      bp = this->m_stor;
      ep = bp;
      // Prepend a minus sign if the number is negative.
      if(sign)
        *(ep++) = '-';
      // Prepend the binary prefix.
      *(ep++) = '0';
      *(ep++) = 'b';
      // Break the number down into fractional and exponential parts. This result is exact.
      uint64_t mant;
      int exp;
      do_xfrexp_F_bin(mant, exp, value);
      // Write the broken-down number in scientific notation.
      do_xput_M_bin(ep, mant, ep + 1);
      *(ep++) = 'p';
      do_xput_I_exp(ep, exp);
      // Append a null terminator.
      *ep = 0;
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_XF(double value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += do_cast_U(sign + 1);
    }
    else {
      // Seek to the beginning of the internal buffer.
      bp = this->m_stor;
      ep = bp;
      // Prepend a minus sign if the number is negative.
      if(sign)
        *(ep++) = '-';
      // Prepend the hexadecimal prefix.
      *(ep++) = '0';
      *(ep++) = 'x';
      // Break the number down into fractional and exponential parts. This result is exact.
      uint64_t mant;
      int exp;
      do_xfrexp_F_bin(mant, exp, value);
      // Normalize the integral part so it is the maximum value in the range [0,16).
      mant >>= ~exp & 3;
      exp >>= 2;
      // Write the broken-down number...
      if((exp < -4) || (14 <= exp)) {
        // ... in scientific notation.
        do_xput_M_hex(ep, mant, ep + 1);
        *(ep++) = 'p';
        do_xput_I_exp(ep, exp * 4);
      }
      else if(exp < 0) {
        // ... in plain format; the number starts with "0."; zeroes are prepended as necessary.
        *(ep++) = '0';
        *(ep++) = '.';
        noadl::ranged_for(exp, -1, [&](int) { *(ep++) = '0';  });
        do_xput_M_hex(ep, mant, nullptr);
      }
      else {
        // ... in plain format; the decimal point is inserted in the middle.
        do_xput_M_hex(ep, mant, ep + 1 + do_cast_U(exp));
      }
      // Append a null terminator.
      *ep = 0;
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_XE(double value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += do_cast_U(sign + 1);
    }
    else {
      // Seek to the beginning of the internal buffer.
      bp = this->m_stor;
      ep = bp;
      // Prepend a minus sign if the number is negative.
      if(sign)
        *(ep++) = '-';
      // Prepend the hexadecimal prefix.
      *(ep++) = '0';
      *(ep++) = 'x';
      // Break the number down into fractional and exponential parts. This result is exact.
      uint64_t mant;
      int exp;
      do_xfrexp_F_bin(mant, exp, value);
      // Normalize the integral part so it is the maximum value in the range [0,16).
      mant >>= ~exp & 3;
      exp >>= 2;
      // Write the broken-down number in scientific notation.
      do_xput_M_hex(ep, mant, ep + 1);
      *(ep++) = 'p';
      do_xput_I_exp(ep, exp * 4);
      // Append a null terminator.
      *ep = 0;
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

    namespace {

#if 0
/* This program is used to generate the multiplier table for decimal
 * numbers. Each multiplier for the mantissa is split into two parts.
 * The base of exponents is 2.
 *
 * Compile with:
 *   gcc -std=c99 -W{all,extra,{sign-,}conversion} table.c -lquadmath
 */

#include <quadmath.h>
#include <stdio.h>

void do_print_one(int e)
  {
    __float128 value, frac;
    int bexp;
    long long mant;

    // Calculate the boundary.
    value = powq(10, e);
    if(e == -324) {
      // Truncate the underflowed value.
      printf("\t{ %23s, ", "0");
    }
    else {
      // Break it down into the fraction and exponent.
      frac = frexpq(value, &bexp);
      // Truncate the fraction to 53 bits. Do not round it.
      frac = ldexpq(frac, 53);
      mant = (long long)frac;
      bexp = bexp - 1;
      // Print the higher part in fixed-point format.
      printf("\t{ 0x1.%.13llXp%+.4d, ",
             mant & 0xFFFFFFFFFFFFF, bexp);
    }

    // Calculate the reciprocal of the boundary.
    // We only need 17 decimal digits.
    value = 1.0e17 / value;
    // Break it down into the fraction and exponent.
    frac = frexpq(value, &bexp);
    // The fraction is in 63-bit mantissa format.
    frac = ldexpq(frac, 63);
    mant = (long long)frac;
    bexp = bexp + 1;
    // Print the mantissa in fixed-point format.
    printf("  0x%.16llX, ", mant);
    // Print the exponent in binary.
    printf("%+5d },", bexp);

    // Print some comments.
    printf("  // 1.0e%+.3d\n", e);
  }

int main(void)
  {
    int e;

    for(e = -324; e <= +308; ++e)
      do_print_one(e);

    return 0;
  }
#endif

    // These are generated data. Do not edit by hand!
    struct decmult_F
      {
        double bound;
        uint64_t mant;
        int bexp;
      }
    constexpr s_decmult_F[] =
      {
        {                       0,   0x6DB4616541769502, +1134 },  // 1.0e-324
        { 0x1.03132B9CF541Cp-1073,   0x57C3811DCDF87735, +1131 },  // 1.0e-323
        { 0x1.43D7F68432923p-1070,   0x463600E4A4C6C5C4, +1128 },  // 1.0e-322
        { 0x1.94CDF4253F36Cp-1067,   0x705667D43AD7A2D3, +1124 },  // 1.0e-321
        { 0x1.FA01712E8F047p-1064,   0x59DEB97695794F0F, +1121 },  // 1.0e-320
        { 0x1.3C40E6BD1962Cp-1060,   0x47E5612BAAC77273, +1118 },  // 1.0e-319
        { 0x1.8B51206C5FBB7p-1057,   0x73089B79113F1D84, +1114 },  // 1.0e-318
        { 0x1.EE25688777AA5p-1054,   0x5C06E2C740FF4AD0, +1111 },  // 1.0e-317
        { 0x1.34D76154AACA7p-1050,   0x499F1BD29A65D573, +1108 },  // 1.0e-316
        { 0x1.820D39A9D57D1p-1047,   0x75CB5FB75D6FBBEC, +1104 },  // 1.0e-315
        { 0x1.E29088144ADC5p-1044,   0x5E3C4C92B1262FF0, +1101 },  // 1.0e-314
        { 0x1.2D9A550CAEC9Bp-1040,   0x4B6370755A84F326, +1098 },  // 1.0e-313
        { 0x1.7900EA4FDA7C2p-1037,   0x789F1A555DA1850A, +1094 },  // 1.0e-312
        { 0x1.D74124E3D11B2p-1034,   0x607F48444AE79DA2, +1091 },  // 1.0e-311
        { 0x1.2688B70E62B0Fp-1030,   0x4D32A036A252E481, +1088 },  // 1.0e-310
        { 0x1.702AE4D1FB5D3p-1027,   0x7B84338A9D516D9C, +1084 },  // 1.0e-309
        { 0x1.CC359E067A348p-1024,   0x62D0293BB10DF149, +1081 },  // 1.0e-308
        { 0x1.1FA182C40C60Dp-1020,   0x4F0CEDC95A718DD4, +1078 },  // 1.0e-307
        { 0x1.6789E3750F790p-1017,   0x7E7B160EF71C1621, +1074 },  // 1.0e-306
        { 0x1.C16C5C5253575p-1014,   0x652F44D8C5B011B4, +1071 },  // 1.0e-305
        { 0x1.18E3B9B374169p-1010,   0x50F29D7A37C00E29, +1068 },  // 1.0e-304
        { 0x1.5F1CA820511C3p-1007,   0x40C21794F96671BA, +1065 },  // 1.0e-303
        { 0x1.B6E3D22865634p-1004,   0x679CF287F570B5F7, +1061 },  // 1.0e-302
        { 0x1.124E63593F5E0p-1000,   0x52E3F5399126F7F9, +1058 },  // 1.0e-301
        { 0x1.56E1FC2F8F358p-0997,   0x424FF76140EBF994, +1055 },  // 1.0e-300
        { 0x1.AC9A7B3B7302Fp-0994,   0x6A198BCECE465C20, +1051 },  // 1.0e-299
        { 0x1.0BE08D0527E1Dp-0990,   0x54E13CA571D1E34D, +1048 },  // 1.0e-298
        { 0x1.4ED8B04671DA4p-0987,   0x43E763B78E4182A4, +1045 },  // 1.0e-297
        { 0x1.A28EDC580E50Dp-0984,   0x6CA56C58E39C043A, +1041 },  // 1.0e-296
        { 0x1.059949B708F28p-0980,   0x56EABD13E9499CFB, +1038 },  // 1.0e-295
        { 0x1.46FF9C24CB2F2p-0977,   0x458897432107B0C8, +1035 },  // 1.0e-294
        { 0x1.98BF832DFDFAFp-0974,   0x6F40F20501A5E7A7, +1031 },  // 1.0e-293
        { 0x1.FEEF63F97D79Bp-0971,   0x5900C19D9AEB1FB9, +1028 },  // 1.0e-292
        { 0x1.3F559E7BEE6C1p-0967,   0x4733CE17AF227FC7, +1025 },  // 1.0e-291
        { 0x1.8F2B061AEA071p-0964,   0x71EC7CF2B1D0CC72, +1021 },  // 1.0e-290
        { 0x1.F2F5C7A1A488Dp-0961,   0x5B2397288E40A38E, +1018 },  // 1.0e-289
        { 0x1.37D99CC506D58p-0957,   0x48E945BA0B66E93F, +1015 },  // 1.0e-288
        { 0x1.85D003F6488AEp-0954,   0x74A86F90123E41FE, +1011 },  // 1.0e-287
        { 0x1.E74404F3DAADAp-0951,   0x5D538C7341CB67FE, +1008 },  // 1.0e-286
        { 0x1.308A831868AC8p-0947,   0x4AA93D29016F8665, +1005 },  // 1.0e-285
        { 0x1.7CAD23DE82D7Ap-0944,   0x77752EA8024C0A3C, +1001 },  // 1.0e-284
        { 0x1.DBD86CD6238D9p-0941,   0x5F90F22001D66E96,  +998 },  // 1.0e-283
        { 0x1.29674405D6387p-0937,   0x4C73F4E667DEBEDE,  +995 },  // 1.0e-282
        { 0x1.73C115074BC69p-0934,   0x7A532170A6313164,  +991 },  // 1.0e-281
        { 0x1.D0B15A491EB84p-0931,   0x61DC1AC084F42783,  +988 },  // 1.0e-280
        { 0x1.226ED86DB3332p-0927,   0x4E49AF006A5CEC69,  +985 },  // 1.0e-279
        { 0x1.6B0A8E891FFFFp-0924,   0x7D42B19A43C7E0A8,  +981 },  // 1.0e-278
        { 0x1.C5CD322B67FFFp-0921,   0x64355AE1CFD31A20,  +978 },  // 1.0e-277
        { 0x1.1BA03F5B20FFFp-0917,   0x502AAF1B0CA8E1B3,  +975 },  // 1.0e-276
        { 0x1.62884F31E93FFp-0914,   0x402225AF3D53E7C2,  +972 },  // 1.0e-275
        { 0x1.BB2A62FE638FFp-0911,   0x669D0918621FD937,  +968 },  // 1.0e-274
        { 0x1.14FA7DDEFE39Fp-0907,   0x52173A79E8197A92,  +965 },  // 1.0e-273
        { 0x1.5A391D56BDC87p-0904,   0x41AC2EC7ECE12EDB,  +962 },  // 1.0e-272
        { 0x1.B0C764AC6D3A9p-0901,   0x69137E0CAE3517C6,  +958 },  // 1.0e-271
        { 0x1.0E7C9EEBC4449p-0897,   0x540F980A24F74638,  +955 },  // 1.0e-270
        { 0x1.521BC6A6B555Cp-0894,   0x433FACD4EA5F6B60,  +952 },  // 1.0e-269
        { 0x1.A6A2B85062AB3p-0891,   0x6B991487DD657899,  +948 },  // 1.0e-268
        { 0x1.0825B3323DAB0p-0887,   0x5614106CB11DFA14,  +945 },  // 1.0e-267
        { 0x1.4A2F1FFECD15Cp-0884,   0x44DCD9F08DB194DD,  +942 },  // 1.0e-266
        { 0x1.9CBAE7FE805B3p-0881,   0x6E2E2980E2B5BAFB,  +938 },  // 1.0e-265
        { 0x1.01F4D0FF1038Fp-0877,   0x5824EE00B55E2F2F,  +935 },  // 1.0e-264
        { 0x1.4272053ED4473p-0874,   0x4683F19A2AB1BF59,  +932 },  // 1.0e-263
        { 0x1.930E868E89590p-0871,   0x70D31C29DDE93228,  +928 },  // 1.0e-262
        { 0x1.F7D228322BAF5p-0868,   0x5A427CEE4B20F4ED,  +925 },  // 1.0e-261
        { 0x1.3AE3591F5B4D9p-0864,   0x483530BEA280C3F1,  +922 },  // 1.0e-260
        { 0x1.899C2F673220Fp-0861,   0x73884DFDD0CE064E,  +918 },  // 1.0e-259
        { 0x1.EC033B40FEA93p-0858,   0x5C6D0B3173D8050B,  +915 },  // 1.0e-258
        { 0x1.338205089F29Cp-0854,   0x49F0D5C129799DA2,  +912 },  // 1.0e-257
        { 0x1.8062864AC6F43p-0851,   0x764E22CEA8C295D1,  +908 },  // 1.0e-256
        { 0x1.E07B27DD78B13p-0848,   0x5EA4E8A553CEDE41,  +905 },  // 1.0e-255
        { 0x1.2C4CF8EA6B6ECp-0844,   0x4BB72084430BE500,  +902 },  // 1.0e-254
        { 0x1.77603725064A7p-0841,   0x792500D39E796E67,  +898 },  // 1.0e-253
        { 0x1.D53844EE47DD1p-0838,   0x60EA670FB1FABEB9,  +895 },  // 1.0e-252
        { 0x1.25432B14ECEA2p-0834,   0x4D885272F4C89894,  +892 },  // 1.0e-251
        { 0x1.6E93F5DA2824Bp-0831,   0x7C0D50B7EE0DC0ED,  +888 },  // 1.0e-250
        { 0x1.CA38F350B22DEp-0828,   0x633DDA2CBE716724,  +885 },  // 1.0e-249
        { 0x1.1E6398126F5CBp-0824,   0x4F64AE8A31F45283,  +882 },  // 1.0e-248
        { 0x1.65FC7E170B33Dp-0821,   0x7F077DA9E986EA6B,  +878 },  // 1.0e-247
        { 0x1.BF7B9D9CCE00Dp-0818,   0x659F97BB2138BB89,  +875 },  // 1.0e-246
        { 0x1.17AD428200C08p-0814,   0x514C796280FA2FA1,  +872 },  // 1.0e-245
        { 0x1.5D98932280F0Ap-0811,   0x4109FAB533FB594D,  +869 },  // 1.0e-244
        { 0x1.B4FEB7EB212CDp-0808,   0x680FF788532BC216,  +865 },  // 1.0e-243
        { 0x1.111F32F2F4BC0p-0804,   0x533FF939DC2301AB,  +862 },  // 1.0e-242
        { 0x1.5566FFAFB1EB0p-0801,   0x4299942E49B59AEF,  +859 },  // 1.0e-241
        { 0x1.AAC0BF9B9E65Cp-0798,   0x6A8F537D42BC2B18,  +855 },  // 1.0e-240
        { 0x1.0AB877C142FF9p-0794,   0x553F75FDCEFCEF46,  +852 },  // 1.0e-239
        { 0x1.4D6695B193BF8p-0791,   0x4432C4CB0BFD8C38,  +849 },  // 1.0e-238
        { 0x1.A0C03B1DF8AF6p-0788,   0x6D1E07AB466279F4,  +845 },  // 1.0e-237
        { 0x1.047824F2BB6D9p-0784,   0x574B3955D1E86190,  +842 },  // 1.0e-236
        { 0x1.45962E2F6A490p-0781,   0x45D5C777DB204E0D,  +839 },  // 1.0e-235
        { 0x1.96FBB9BB44DB4p-0778,   0x6FBC72595E9A167B,  +835 },  // 1.0e-234
        { 0x1.FCBAA82A16121p-0775,   0x59638EADE54811FC,  +832 },  // 1.0e-233
        { 0x1.3DF4A91A4DCB4p-0771,   0x4782D88B1DD34196,  +829 },  // 1.0e-232
        { 0x1.8D71D360E13E2p-0768,   0x726AF411C952028A,  +825 },  // 1.0e-231
        { 0x1.F0CE4839198DAp-0765,   0x5B88C3416DDB353B,  +822 },  // 1.0e-230
        { 0x1.3680ED23AFF88p-0761,   0x493A35CDF17C2A96,  +819 },  // 1.0e-229
        { 0x1.8421286C9BF6Ap-0758,   0x7529EFAFE8C6AA89,  +815 },  // 1.0e-228
        { 0x1.E5297287C2F45p-0755,   0x5DBB262653D22207,  +812 },  // 1.0e-227
        { 0x1.2F39E794D9D8Bp-0751,   0x4AFC1E850FDB4E6C,  +809 },  // 1.0e-226
        { 0x1.7B08617A104EEp-0748,   0x77F9CA6E7FC54A47,  +805 },  // 1.0e-225
        { 0x1.D9CA79D894629p-0745,   0x5FFB085866376E9F,  +802 },  // 1.0e-224
        { 0x1.281E8C275CBDAp-0741,   0x4CC8D379EB5F8BB2,  +799 },  // 1.0e-223
        { 0x1.72262F3133ED0p-0738,   0x7ADAEBF64565AC51,  +795 },  // 1.0e-222
        { 0x1.CEAFBAFD80E84p-0735,   0x6248BCC5045156A7,  +792 },  // 1.0e-221
        { 0x1.212DD4DE70913p-0731,   0x4EA0970403744552,  +789 },  // 1.0e-220
        { 0x1.69794A160CB57p-0728,   0x7DCDBE6CD253A21E,  +785 },  // 1.0e-219
        { 0x1.C3D79C9B8FE2Dp-0725,   0x64A498570EA94E7E,  +782 },  // 1.0e-218
        { 0x1.1A66C1E139EDCp-0721,   0x5083AD1272210B98,  +779 },  // 1.0e-217
        { 0x1.6100725988693p-0718,   0x40695741F4E73C79,  +776 },  // 1.0e-216
        { 0x1.B9408EEFEA838p-0715,   0x670EF2032171FA5C,  +772 },  // 1.0e-215
        { 0x1.13C85955F2923p-0711,   0x52725B35B45B2EB0,  +769 },  // 1.0e-214
        { 0x1.58BA6FAB6F36Cp-0708,   0x41F515C49048F226,  +766 },  // 1.0e-213
        { 0x1.AEE90B964B047p-0705,   0x698822D41A0E503E,  +762 },  // 1.0e-212
        { 0x1.0D51A73DEEE2Cp-0701,   0x546CE8A9AE71D9CB,  +759 },  // 1.0e-211
        { 0x1.50A6110D6A9B7p-0698,   0x438A53BAF1F4AE3C,  +756 },  // 1.0e-210
        { 0x1.A4CF9550C5425p-0695,   0x6C1085F7E9877D2D,  +752 },  // 1.0e-209
        { 0x1.0701BD527B497p-0691,   0x56739E5FEE05FDBD,  +749 },  // 1.0e-208
        { 0x1.48C22CA71A1BDp-0688,   0x45294B7FF19E6497,  +746 },  // 1.0e-207
        { 0x1.9AF2B7D0E0A2Cp-0685,   0x6EA878CCB5CA3A8C,  +742 },  // 1.0e-206
        { 0x1.00D7B2E28C65Bp-0681,   0x5886C70A2B082ED6,  +739 },  // 1.0e-205
        { 0x1.410D9F9B2F7F2p-0678,   0x46D238D4EF39BF12,  +736 },  // 1.0e-204
        { 0x1.91510781FB5EFp-0675,   0x71505AEE4B8F981D,  +732 },  // 1.0e-203
        { 0x1.F5A549627A36Bp-0672,   0x5AA6AF25093FACE4,  +729 },  // 1.0e-202
        { 0x1.39874DDD8C623p-0668,   0x488558EA6DCC8A50,  +726 },  // 1.0e-201
        { 0x1.87E92154EF7ACp-0665,   0x74088E43E2E0DD4C,  +722 },  // 1.0e-200
        { 0x1.E9E369AA2B597p-0662,   0x5CD3A5031BE71770,  +719 },  // 1.0e-199
        { 0x1.322E220A5B17Ep-0658,   0x4A42EA68E31F45F3,  +716 },  // 1.0e-198
        { 0x1.7EB9AA8CF1DDEp-0655,   0x76D1770E38320986,  +712 },  // 1.0e-197
        { 0x1.DE6815302E555p-0652,   0x5F0DF8D82CF4D46B,  +709 },  // 1.0e-196
        { 0x1.2B010D3E1CF55p-0648,   0x4C0B2D79BD90A9EF,  +706 },  // 1.0e-195
        { 0x1.75C1508DA432Ap-0645,   0x79AB7BF5FC1AA97F,  +702 },  // 1.0e-194
        { 0x1.D331A4B10D3F5p-0642,   0x6155FCC4C9AEEDFF,  +699 },  // 1.0e-193
        { 0x1.23FF06EEA8479p-0638,   0x4DDE63D0A158BE65,  +696 },  // 1.0e-192
        { 0x1.6CFEC8AA52597p-0635,   0x7C97061A9BC130A2,  +692 },  // 1.0e-191
        { 0x1.C83E7AD4E6EFDp-0632,   0x63AC04E2163426E8,  +689 },  // 1.0e-190
        { 0x1.1D270CC51055Ep-0628,   0x4FBCD0B4DE901F20,  +686 },  // 1.0e-189
        { 0x1.6470CFF6546B6p-0625,   0x7F9481216419CB67,  +682 },  // 1.0e-188
        { 0x1.BD8D03F3E9863p-0622,   0x6610674DE9AE3C52,  +679 },  // 1.0e-187
        { 0x1.1678227871F3Ep-0618,   0x51A6B90B21583042,  +676 },  // 1.0e-186
        { 0x1.5C162B168E70Ep-0615,   0x41522DA2811359CE,  +673 },  // 1.0e-185
        { 0x1.B31BB5DC320D1p-0612,   0x68837C3734EBC2E3,  +669 },  // 1.0e-184
        { 0x1.0FF151A99F482p-0608,   0x539C635F5D8968B6,  +666 },  // 1.0e-183
        { 0x1.53EDA614071A3p-0605,   0x42E382B2B13ABA2B,  +663 },  // 1.0e-182
        { 0x1.A8E90F9908E0Cp-0602,   0x6B059DEAB52AC378,  +659 },  // 1.0e-181
        { 0x1.0991A9BFA58C7p-0598,   0x559E17EEF755692D,  +656 },  // 1.0e-180
        { 0x1.4BF6142F8EEF9p-0595,   0x447E798BF91120F1,  +653 },  // 1.0e-179
        { 0x1.9EF3993B72AB8p-0592,   0x6D9728DFF4E834B5,  +649 },  // 1.0e-178
        { 0x1.03583FC527AB3p-0588,   0x57AC20B32A535D5D,  +646 },  // 1.0e-177
        { 0x1.442E4FB671960p-0585,   0x46234D5C21DC4AB1,  +643 },  // 1.0e-176
        { 0x1.9539E3A40DFB8p-0582,   0x70387BC69C93AAB5,  +639 },  // 1.0e-175
        { 0x1.FA885C8D117A6p-0579,   0x59C6C96BB076222A,  +636 },  // 1.0e-174
        { 0x1.3C9539D82AEC7p-0575,   0x47D23ABC8D2B4E88,  +633 },  // 1.0e-173
        { 0x1.8BBA884E35A79p-0572,   0x72E9F79415121740,  +629 },  // 1.0e-172
        { 0x1.EEA92A61C3118p-0569,   0x5BEE5FA9AA74DF67,  +626 },  // 1.0e-171
        { 0x1.3529BA7D19EAFp-0565,   0x498B7FBAEEC3E5EC,  +623 },  // 1.0e-170
        { 0x1.8274291C6065Ap-0562,   0x75ABFF917E063CAC,  +619 },  // 1.0e-169
        { 0x1.E3113363787F1p-0559,   0x5E2332DACB38308A,  +616 },  // 1.0e-168
        { 0x1.2DEAC01E2B4F6p-0555,   0x4B4F5BE23C2CF3A1,  +613 },  // 1.0e-167
        { 0x1.79657025B6234p-0552,   0x787EF969F9E185CF,  +609 },  // 1.0e-166
        { 0x1.D7BECC2F23AC1p-0549,   0x60659454C7E79E3F,  +606 },  // 1.0e-165
        { 0x1.26D73F9D764B9p-0545,   0x4D1E1043D31FB1CC,  +603 },  // 1.0e-164
        { 0x1.708D0F84D3DE7p-0542,   0x7B634D3951CC4FAD,  +599 },  // 1.0e-163
        { 0x1.CCB0536608D61p-0539,   0x62B5D7610E3D0C8B,  +596 },  // 1.0e-162
        { 0x1.1FEE341FC585Cp-0535,   0x4EF7DF80D830D6D5,  +593 },  // 1.0e-161
        { 0x1.67E9C127B6E74p-0532,   0x7E59659AF38157BC,  +589 },  // 1.0e-160
        { 0x1.C1E43171A4A11p-0529,   0x65145148C2CDDFC9,  +586 },  // 1.0e-159
        { 0x1.192E9EE706E4Ap-0525,   0x50DD0DD3CF0B196E,  +583 },  // 1.0e-158
        { 0x1.5F7A46A0C89DDp-0522,   0x40B0D7DCA5A27ABE,  +580 },  // 1.0e-157
        { 0x1.B758D848FAC54p-0519,   0x678159610903F797,  +576 },  // 1.0e-156
        { 0x1.1297872D9CBB4p-0515,   0x52CDE11A6D9CC612,  +573 },  // 1.0e-155
        { 0x1.573D68F903EA2p-0512,   0x423E4DAEBE1704DB,  +570 },  // 1.0e-154
        { 0x1.AD0CC33744E4Ap-0509,   0x69FD4917968B3AF9,  +566 },  // 1.0e-153
        { 0x1.0C27FA028B0EEp-0505,   0x54CAA0DFABA29594,  +563 },  // 1.0e-152
        { 0x1.4F31F8832DD2Ap-0502,   0x43D54D7FBC821143,  +560 },  // 1.0e-151
        { 0x1.A2FE76A3F9474p-0499,   0x6C887BFF94034ED2,  +556 },  // 1.0e-150
        { 0x1.05DF0A267BCC9p-0495,   0x56D396661002A574,  +553 },  // 1.0e-149
        { 0x1.4756CCB01ABFBp-0492,   0x457611EB40021DF7,  +550 },  // 1.0e-148
        { 0x1.992C7FDC216FAp-0489,   0x6F234FDECCD02FF1,  +546 },  // 1.0e-147
        { 0x1.FF779FD329CB8p-0486,   0x58E90CB23D73598E,  +543 },  // 1.0e-146
        { 0x1.3FAAC3E3FA1F3p-0482,   0x4720D6F4FDF5E13E,  +540 },  // 1.0e-145
        { 0x1.8F9574DCF8A70p-0479,   0x71CE24BB2FEFCECA,  +536 },  // 1.0e-144
        { 0x1.F37AD21436D0Cp-0476,   0x5B0B5095BFF30BD5,  +533 },  // 1.0e-143
        { 0x1.382CC34CA2427p-0472,   0x48D5DA11665C0977,  +530 },  // 1.0e-142
        { 0x1.8637F41FCAD31p-0469,   0x74895CE8A3C6758B,  +526 },  // 1.0e-141
        { 0x1.E7C5F127BD87Ep-0466,   0x5D3AB0BA1C9EC46F,  +523 },  // 1.0e-140
        { 0x1.30DBB6B8D674Ep-0462,   0x4A955A2E7D4BD059,  +520 },  // 1.0e-139
        { 0x1.7D12A4670C122p-0459,   0x77555D172EDFB3C2,  +516 },  // 1.0e-138
        { 0x1.DC574D80CF16Bp-0456,   0x5F777DAC257FC301,  +513 },  // 1.0e-137
        { 0x1.29B69070816E2p-0452,   0x4C5F97BCEACC9C01,  +510 },  // 1.0e-136
        { 0x1.7424348CA1C9Bp-0449,   0x7A328C6177ADC668,  +506 },  // 1.0e-135
        { 0x1.D12D41AFCA3C2p-0446,   0x61C209E792F16B86,  +503 },  // 1.0e-134
        { 0x1.22BC490DDE659p-0442,   0x4E34D4B9425ABC6B,  +500 },  // 1.0e-133
        { 0x1.6B6B5B5155FF0p-0439,   0x7D21545B9D5DFA46,  +496 },  // 1.0e-132
        { 0x1.C6463225AB7ECp-0436,   0x641AA9E2E44B2E9E,  +493 },  // 1.0e-131
        { 0x1.1BEBDF578B2F3p-0432,   0x501554B5836F587E,  +490 },  // 1.0e-130
        { 0x1.62E6D72D6DFB0p-0429,   0x4011109135F2AD32,  +487 },  // 1.0e-129
        { 0x1.BBA08CF8C979Cp-0426,   0x6681B41B89844850,  +483 },  // 1.0e-128
        { 0x1.1544581B7DEC1p-0422,   0x52015CE2D469D373,  +480 },  // 1.0e-127
        { 0x1.5A956E225D672p-0419,   0x419AB0B576BB0F8F,  +477 },  // 1.0e-126
        { 0x1.B13AC9AAF4C0Ep-0416,   0x68F781225791B27F,  +473 },  // 1.0e-125
        { 0x1.0EC4BE0AD8F89p-0412,   0x53F9341B79415B99,  +470 },  // 1.0e-124
        { 0x1.5275ED8D8F36Bp-0409,   0x432DC3492DCDE2E1,  +467 },  // 1.0e-123
        { 0x1.A71368F0F3046p-0406,   0x6B7C6BA849496B01,  +463 },  // 1.0e-122
        { 0x1.086C219697E2Cp-0402,   0x55FD22ED076DEF34,  +460 },  // 1.0e-121
        { 0x1.4A8729FC3DDB7p-0399,   0x44CA82573924BF5D,  +457 },  // 1.0e-120
        { 0x1.9D28F47B4D524p-0396,   0x6E10D08B8EA1322E,  +453 },  // 1.0e-119
        { 0x1.023998CD10537p-0392,   0x580D73A2D880F4F2,  +450 },  // 1.0e-118
        { 0x1.42C7FF0054684p-0389,   0x4671294F139A5D8E,  +447 },  // 1.0e-117
        { 0x1.9379FEC069826p-0386,   0x70B50EE4EC2A2F4A,  +443 },  // 1.0e-116
        { 0x1.F8587E7083E2Fp-0383,   0x5A2A7250BCEE8C3B,  +440 },  // 1.0e-115
        { 0x1.3B374F06526DDp-0379,   0x4821F50D63F209C9,  +437 },  // 1.0e-114
        { 0x1.8A0522C7E7095p-0376,   0x736988156CB6760E,  +433 },  // 1.0e-113
        { 0x1.EC866B79E0CBAp-0373,   0x5C546CDDF091F80B,  +430 },  // 1.0e-112
        { 0x1.33D4032C2C7F4p-0369,   0x49DD23E4C074C66F,  +427 },  // 1.0e-111
        { 0x1.80C903F7379F1p-0366,   0x762E9FD467213D7F,  +423 },  // 1.0e-110
        { 0x1.E0FB44F50586Ep-0363,   0x5E8BB3105280FDFF,  +420 },  // 1.0e-109
        { 0x1.2C9D0B1923744p-0359,   0x4BA2F5A6A8673199,  +417 },  // 1.0e-108
        { 0x1.77C44DDF6C515p-0356,   0x7904BC3DDA3EB5C2,  +413 },  // 1.0e-107
        { 0x1.D5B561574765Bp-0353,   0x60D09697E1CBC49B,  +410 },  // 1.0e-106
        { 0x1.25915CD68C9F9p-0349,   0x4D73ABACB4A303AF,  +407 },  // 1.0e-105
        { 0x1.6EF5B40C2FC77p-0346,   0x7BEC45E12104D2B2,  +403 },  // 1.0e-104
        { 0x1.CAB3210F3BB95p-0343,   0x63236B1A80D0A88E,  +400 },  // 1.0e-103
        { 0x1.1EAFF4A98553Dp-0339,   0x4F4F88E200A6ED3F,  +397 },  // 1.0e-102
        { 0x1.665BF1D3E6A8Cp-0336,   0x7EE5A7D0010B1531,  +393 },  // 1.0e-101
        { 0x1.BFF2EE48E052Fp-0333,   0x6584864000D5AA8E,  +390 },  // 1.0e-100
        { 0x1.17F7D4ED8C33Dp-0329,   0x5136D1CCCD77BBA4,  +387 },  // 1.0e-099
        { 0x1.5DF5CA28EF40Dp-0326,   0x40F8A7D70AC62FB7,  +384 },  // 1.0e-098
        { 0x1.B5733CB32B110p-0323,   0x67F43FBE77A37F8B,  +380 },  // 1.0e-097
        { 0x1.116805EFFAEAAp-0319,   0x5329CC985FB5FFA2,  +377 },  // 1.0e-096
        { 0x1.55C2076BF9A55p-0316,   0x4287D6E04C91994F,  +374 },  // 1.0e-095
        { 0x1.AB328946F80EAp-0313,   0x6A72F166E0E8F54B,  +370 },  // 1.0e-094
        { 0x1.0AFF95CC5B092p-0309,   0x5528C11F1A53F76F,  +367 },  // 1.0e-093
        { 0x1.4DBF7B3F71CB7p-0306,   0x44209A7F48432C59,  +364 },  // 1.0e-092
        { 0x1.A12F5A0F4E3E4p-0303,   0x6D00F7320D3846F4,  +360 },  // 1.0e-091
        { 0x1.04BD984990E6Fp-0299,   0x5733F8F4D76038C3,  +357 },  // 1.0e-090
        { 0x1.45ECFE5BF520Ap-0296,   0x45C32D90AC4CFA36,  +354 },  // 1.0e-089
        { 0x1.97683DF2F268Dp-0293,   0x6F9EAF4DE07B29F0,  +350 },  // 1.0e-088
        { 0x1.FD424D6FAF030p-0290,   0x594BBF71806287F3,  +347 },  // 1.0e-087
        { 0x1.3E497065CD61Ep-0286,   0x476FCC5ACD1B9FF6,  +344 },  // 1.0e-086
        { 0x1.8DDBCC7F40BA6p-0283,   0x724C7A2AE1C5CCBD,  +340 },  // 1.0e-085
        { 0x1.F152BF9F10E8Fp-0280,   0x5B7061BBE7D17097,  +337 },  // 1.0e-084
        { 0x1.36D3B7C36A919p-0276,   0x4926B496530DF3AC,  +334 },  // 1.0e-083
        { 0x1.8488A5B445360p-0273,   0x750ABA8A1E7CB913,  +330 },  // 1.0e-082
        { 0x1.E5AACF2156838p-0270,   0x5DA22ED4E530940F,  +327 },  // 1.0e-081
        { 0x1.2F8AC174D6123p-0266,   0x4AE825771DC07672,  +324 },  // 1.0e-080
        { 0x1.7B6D71D20B96Cp-0263,   0x77D9D58B62CD8A51,  +320 },  // 1.0e-079
        { 0x1.DA48CE468E7C7p-0260,   0x5FE177A2B5713B74,  +317 },  // 1.0e-078
        { 0x1.286D80EC190DCp-0256,   0x4CB45FB55DF42F90,  +314 },  // 1.0e-077
        { 0x1.7288E1271F513p-0253,   0x7ABA32BBC986B280,  +310 },  // 1.0e-076
        { 0x1.CF2B1970E7258p-0250,   0x622E8EFCA1388ECD,  +307 },  // 1.0e-075
        { 0x1.217AEFE690777p-0246,   0x4E8BA596E760723D,  +304 },  // 1.0e-074
        { 0x1.69D9ABE034955p-0243,   0x7DAC3C24A5671D2F,  +300 },  // 1.0e-073
        { 0x1.C45016D841BAAp-0240,   0x6489C9B6EAB8E426,  +297 },  // 1.0e-072
        { 0x1.1AB20E472914Ap-0236,   0x506E3AF8BBC71CEB,  +294 },  // 1.0e-071
        { 0x1.615E91D8F359Dp-0233,   0x40582F2D6305B0BC,  +291 },  // 1.0e-070
        { 0x1.B9B6364F30304p-0230,   0x66F37EAF04D5E793,  +287 },  // 1.0e-069
        { 0x1.1411E1F17E1E2p-0226,   0x525C6558D0AB1FA9,  +284 },  // 1.0e-068
        { 0x1.59165A6DDDA5Bp-0223,   0x41E384470D55B2ED,  +281 },  // 1.0e-067
        { 0x1.AF5BF109550F2p-0220,   0x696C06D81555EB15,  +277 },  // 1.0e-066
        { 0x1.0D9976A5D5297p-0216,   0x54566BE0111188DE,  +274 },  // 1.0e-065
        { 0x1.50FFD44F4A73Dp-0213,   0x4378564CDA746D7E,  +271 },  // 1.0e-064
        { 0x1.A53FC9631D10Cp-0210,   0x6BF3BD47C3ED7BFD,  +267 },  // 1.0e-063
        { 0x1.0747DDDDF22A7p-0206,   0x565C976C9CBDFCCB,  +264 },  // 1.0e-062
        { 0x1.4919D5556EB51p-0203,   0x4516DF8A16FE63D5,  +261 },  // 1.0e-061
        { 0x1.9B604AAACA626p-0200,   0x6E8AFF4357FD6C89,  +257 },  // 1.0e-060
        { 0x1.011C2EAABE7D7p-0196,   0x586F329C466456D4,  +254 },  // 1.0e-059
        { 0x1.41633A556E1CDp-0193,   0x46BF5BB038504576,  +251 },  // 1.0e-058
        { 0x1.91BC08EAC9A41p-0190,   0x71322C4D26E6D58A,  +247 },  // 1.0e-057
        { 0x1.F62B0B257C0D1p-0187,   0x5A8E89D75252446E,  +244 },  // 1.0e-056
        { 0x1.39DAE6F76D883p-0183,   0x487207DF750E9D25,  +241 },  // 1.0e-055
        { 0x1.8851A0B548EA3p-0180,   0x73E9A63254E42EA2,  +237 },  // 1.0e-054
        { 0x1.EA6608E29B24Cp-0177,   0x5CBAEB5B771CF21B,  +234 },  // 1.0e-053
        { 0x1.327FC58DA0F6Fp-0173,   0x4A2F22AF927D8E7C,  +231 },  // 1.0e-052
        { 0x1.7F1FB6F10934Bp-0170,   0x76B1D118EA627D93,  +227 },  // 1.0e-051
        { 0x1.DEE7A4AD4B81Ep-0167,   0x5EF4A74721E86476,  +224 },  // 1.0e-050
        { 0x1.2B50C6EC4F313p-0163,   0x4BF6EC38E7ED1D2B,  +221 },  // 1.0e-049
        { 0x1.7624F8A762FD8p-0160,   0x798B138E3FE1C845,  +217 },  // 1.0e-048
        { 0x1.D3AE36D13BBCEp-0157,   0x613C0FA4FFE7D36A,  +214 },  // 1.0e-047
        { 0x1.244CE242C5560p-0153,   0x4DC9A61D998642BB,  +211 },  // 1.0e-046
        { 0x1.6D601AD376AB9p-0150,   0x7C75D695C2706AC5,  +207 },  // 1.0e-045
        { 0x1.C8B8218854567p-0147,   0x63917877CEC0556B,  +204 },  // 1.0e-044
        { 0x1.1D7314F534B60p-0143,   0x4FA793930BCD1122,  +201 },  // 1.0e-043
        { 0x1.64CFDA3281E38p-0140,   0x7F7285B812E1B504,  +197 },  // 1.0e-042
        { 0x1.BE03D0BF225C6p-0137,   0x65F537C675815D9C,  +194 },  // 1.0e-041
        { 0x1.16C262777579Cp-0133,   0x5190F96B91344AE3,  +191 },  // 1.0e-040
        { 0x1.5C72FB1552D83p-0130,   0x4140C78940F6A24F,  +188 },  // 1.0e-039
        { 0x1.B38FB9DAA78E4p-0127,   0x6867A5A867F103B2,  +184 },  // 1.0e-038
        { 0x1.1039D428A8B8Ep-0123,   0x53861E2053273628,  +181 },  // 1.0e-037
        { 0x1.54484932D2E72p-0120,   0x42D1B1B375B8F820,  +178 },  // 1.0e-036
        { 0x1.A95A5B7F87A0Ep-0117,   0x6AE91C5255F4C034,  +174 },  // 1.0e-035
        { 0x1.09D8792FB4C49p-0113,   0x558749DB77F70029,  +171 },  // 1.0e-034
        { 0x1.4C4E977BA1F5Bp-0110,   0x446C3B15F9926687,  +168 },  // 1.0e-033
        { 0x1.9F623D5A8A732p-0107,   0x6D79F82328EA3DA6,  +164 },  // 1.0e-032
        { 0x1.039D66589687Fp-0103,   0x5794C6828721CAEB,  +161 },  // 1.0e-031
        { 0x1.4484BFEEBC29Fp-0100,   0x46109ECED2816F22,  +158 },  // 1.0e-030
        { 0x1.95A5EFEA6B347p-0097,   0x701A97B150CF1837,  +154 },  // 1.0e-029
        { 0x1.FB0F6BE506019p-0094,   0x59AEDFC10D7279C5,  +151 },  // 1.0e-028
        { 0x1.3CE9A36F23C0Fp-0090,   0x47BF19673DF52E37,  +148 },  // 1.0e-027
        { 0x1.8C240C4AECB13p-0087,   0x72CB5BD86321E38C,  +144 },  // 1.0e-026
        { 0x1.EF2D0F5DA7DD8p-0084,   0x5BD5E313828182D6,  +141 },  // 1.0e-025
        { 0x1.357C299A88EA7p-0080,   0x4977E8DC68679BDF,  +138 },  // 1.0e-024
        { 0x1.82DB34012B251p-0077,   0x758CA7C70D7292FE,  +134 },  // 1.0e-023
        { 0x1.E392010175EE5p-0074,   0x5E0A1FD271287598,  +131 },  // 1.0e-022
        { 0x1.2E3B40A0E9B4Fp-0070,   0x4B3B4CA85A86C47A,  +128 },  // 1.0e-021
        { 0x1.79CA10C924223p-0067,   0x785EE10D5DA46D90,  +124 },  // 1.0e-020
        { 0x1.D83C94FB6D2ACp-0064,   0x604BE73DE4838AD9,  +121 },  // 1.0e-019
        { 0x1.2725DD1D243ABp-0060,   0x4D0985CB1D3608AE,  +118 },  // 1.0e-018
        { 0x1.70EF54646D496p-0057,   0x7B426FAB61F00DE3,  +114 },  // 1.0e-017
        { 0x1.CD2B297D889BCp-0054,   0x629B8C891B267182,  +111 },  // 1.0e-016
        { 0x1.203AF9EE75615p-0050,   0x4EE2D6D415B85ACE,  +108 },  // 1.0e-015
        { 0x1.6849B86A12B9Bp-0047,   0x7E37BE2022C0914B,  +104 },  // 1.0e-014
        { 0x1.C25C268497681p-0044,   0x64F964E68233A76F,  +101 },  // 1.0e-013
        { 0x1.19799812DEA11p-0040,   0x50C783EB9B5C85F2,   +98 },  // 1.0e-012
        { 0x1.5FD7FE1796495p-0037,   0x409F9CBC7C4A04C2,   +95 },  // 1.0e-011
        { 0x1.B7CDFD9D7BDBAp-0034,   0x6765C793FA10079D,   +91 },  // 1.0e-010
        { 0x1.12E0BE826D694p-0030,   0x52B7D2DCC80CD2E4,   +88 },  // 1.0e-009
        { 0x1.5798EE2308C39p-0027,   0x422CA8B0A00A4250,   +85 },  // 1.0e-008
        { 0x1.AD7F29ABCAF48p-0024,   0x69E10DE76676D080,   +81 },  // 1.0e-007
        { 0x1.0C6F7A0B5ED8Dp-0020,   0x54B40B1F852BDA00,   +78 },  // 1.0e-006
        { 0x1.4F8B588E368F0p-0017,   0x43C33C1937564800,   +75 },  // 1.0e-005
        { 0x1.A36E2EB1C432Cp-0014,   0x6C6B935B8BBD4000,   +71 },  // 1.0e-004
        { 0x1.0624DD2F1A9FBp-0010,   0x56BC75E2D6310000,   +68 },  // 1.0e-003
        { 0x1.47AE147AE147Ap-0007,   0x4563918244F40000,   +65 },  // 1.0e-002
        { 0x1.9999999999999p-0004,   0x6F05B59D3B200000,   +61 },  // 1.0e-001
        { 0x1.0000000000000p+0000,   0x58D15E1762800000,   +58 },  // 1.0e+000
        { 0x1.4000000000000p+0003,   0x470DE4DF82000000,   +55 },  // 1.0e+001
        { 0x1.9000000000000p+0006,   0x71AFD498D0000000,   +51 },  // 1.0e+002
        { 0x1.F400000000000p+0009,   0x5AF3107A40000000,   +48 },  // 1.0e+003
        { 0x1.3880000000000p+0013,   0x48C2739500000000,   +45 },  // 1.0e+004
        { 0x1.86A0000000000p+0016,   0x746A528800000000,   +41 },  // 1.0e+005
        { 0x1.E848000000000p+0019,   0x5D21DBA000000000,   +38 },  // 1.0e+006
        { 0x1.312D000000000p+0023,   0x4A817C8000000000,   +35 },  // 1.0e+007
        { 0x1.7D78400000000p+0026,   0x7735940000000000,   +31 },  // 1.0e+008
        { 0x1.DCD6500000000p+0029,   0x5F5E100000000000,   +28 },  // 1.0e+009
        { 0x1.2A05F20000000p+0033,   0x4C4B400000000000,   +25 },  // 1.0e+010
        { 0x1.74876E8000000p+0036,   0x7A12000000000000,   +21 },  // 1.0e+011
        { 0x1.D1A94A2000000p+0039,   0x61A8000000000000,   +18 },  // 1.0e+012
        { 0x1.2309CE5400000p+0043,   0x4E20000000000000,   +15 },  // 1.0e+013
        { 0x1.6BCC41E900000p+0046,   0x7D00000000000000,   +11 },  // 1.0e+014
        { 0x1.C6BF526340000p+0049,   0x6400000000000000,    +8 },  // 1.0e+015
        { 0x1.1C37937E08000p+0053,   0x5000000000000000,    +5 },  // 1.0e+016
        { 0x1.6345785D8A000p+0056,   0x4000000000000000,    +2 },  // 1.0e+017
        { 0x1.BC16D674EC800p+0059,   0x6666666666666666,    -2 },  // 1.0e+018
        { 0x1.158E460913D00p+0063,   0x51EB851EB851EB85,    -5 },  // 1.0e+019
        { 0x1.5AF1D78B58C40p+0066,   0x4189374BC6A7EF9D,    -8 },  // 1.0e+020
        { 0x1.B1AE4D6E2EF50p+0069,   0x68DB8BAC710CB295,   -12 },  // 1.0e+021
        { 0x1.0F0CF064DD592p+0073,   0x53E2D6238DA3C211,   -15 },  // 1.0e+022
        { 0x1.52D02C7E14AF6p+0076,   0x431BDE82D7B634DA,   -18 },  // 1.0e+023
        { 0x1.A784379D99DB4p+0079,   0x6B5FCA6AF2BD215E,   -22 },  // 1.0e+024
        { 0x1.08B2A2C280290p+0083,   0x55E63B88C230E77E,   -25 },  // 1.0e+025
        { 0x1.4ADF4B7320334p+0086,   0x44B82FA09B5A52CB,   -28 },  // 1.0e+026
        { 0x1.9D971E4FE8401p+0089,   0x6DF37F675EF6EADF,   -32 },  // 1.0e+027
        { 0x1.027E72F1F1281p+0093,   0x57F5FF85E592557F,   -35 },  // 1.0e+028
        { 0x1.431E0FAE6D721p+0096,   0x465E6604B7A84465,   -38 },  // 1.0e+029
        { 0x1.93E5939A08CE9p+0099,   0x709709A125DA0709,   -42 },  // 1.0e+030
        { 0x1.F8DEF8808B024p+0102,   0x5A126E1A84AE6C07,   -45 },  // 1.0e+031
        { 0x1.3B8B5B5056E16p+0106,   0x480EBE7B9D58566C,   -48 },  // 1.0e+032
        { 0x1.8A6E32246C99Cp+0109,   0x734ACA5F6226F0AD,   -52 },  // 1.0e+033
        { 0x1.ED09BEAD87C03p+0112,   0x5C3BD5191B525A24,   -55 },  // 1.0e+034
        { 0x1.3426172C74D82p+0116,   0x49C97747490EAE83,   -58 },  // 1.0e+035
        { 0x1.812F9CF7920E2p+0119,   0x760F253EDB4AB0D2,   -62 },  // 1.0e+036
        { 0x1.E17B84357691Bp+0122,   0x5E72843249088D75,   -65 },  // 1.0e+037
        { 0x1.2CED32A16A1B1p+0126,   0x4B8ED0283A6D3DF7,   -68 },  // 1.0e+038
        { 0x1.78287F49C4A1Dp+0129,   0x78E480405D7B9658,   -72 },  // 1.0e+039
        { 0x1.D6329F1C35CA4p+0132,   0x60B6CD004AC94513,   -75 },  // 1.0e+040
        { 0x1.25DFA371A19E6p+0136,   0x4D5F0A66A23A9DA9,   -78 },  // 1.0e+041
        { 0x1.6F578C4E0A060p+0139,   0x7BCB43D769F762A8,   -82 },  // 1.0e+042
        { 0x1.CB2D6F618C878p+0142,   0x63090312BB2C4EED,   -85 },  // 1.0e+043
        { 0x1.1EFC659CF7D4Bp+0146,   0x4F3A68DBC8F03F24,   -88 },  // 1.0e+044
        { 0x1.66BB7F0435C9Ep+0149,   0x7EC3DAF941806506,   -92 },  // 1.0e+045
        { 0x1.C06A5EC5433C6p+0152,   0x65697BFA9ACD1D9F,   -95 },  // 1.0e+046
        { 0x1.18427B3B4A05Bp+0156,   0x51212FFBAF0A7E18,   -98 },  // 1.0e+047
        { 0x1.5E531A0A1C872p+0159,   0x40E7599625A1FE7A,  -101 },  // 1.0e+048
        { 0x1.B5E7E08CA3A8Fp+0162,   0x67D88F56A29CCA5D,  -105 },  // 1.0e+049
        { 0x1.11B0EC57E6499p+0166,   0x5313A5DEE87D6EB0,  -108 },  // 1.0e+050
        { 0x1.561D276DDFDC0p+0169,   0x42761E4BED31255A,  -111 },  // 1.0e+051
        { 0x1.ABA4714957D30p+0172,   0x6A5696DFE1E83BC3,  -115 },  // 1.0e+052
        { 0x1.0B46C6CDD6E3Ep+0176,   0x5512124CB4B9C969,  -118 },  // 1.0e+053
        { 0x1.4E1878814C9CDp+0179,   0x440E750A2A2E3ABA,  -121 },  // 1.0e+054
        { 0x1.A19E96A19FC40p+0182,   0x6CE3EE76A9E3912A,  -125 },  // 1.0e+055
        { 0x1.05031E2503DA8p+0186,   0x571CBEC554B60DBB,  -128 },  // 1.0e+056
        { 0x1.4643E5AE44D12p+0189,   0x45B0989DDD5E7163,  -131 },  // 1.0e+057
        { 0x1.97D4DF19D6057p+0192,   0x6F80F42FC8971BD1,  -135 },  // 1.0e+058
        { 0x1.FDCA16E04B86Dp+0195,   0x5933F68CA078E30E,  -138 },  // 1.0e+059
        { 0x1.3E9E4E4C2F344p+0199,   0x475CC53D4D2D8271,  -141 },  // 1.0e+060
        { 0x1.8E45E1DF3B015p+0202,   0x722E086215159D82,  -145 },  // 1.0e+061
        { 0x1.F1D75A5709C1Ap+0205,   0x5B5806B4DDAAE468,  -148 },  // 1.0e+062
        { 0x1.3726987666190p+0209,   0x49133890B1558386,  -151 },  // 1.0e+063
        { 0x1.84F03E93FF9F4p+0212,   0x74EB8DB44EEF38D7,  -155 },  // 1.0e+064
        { 0x1.E62C4E38FF872p+0215,   0x5D893E29D8BF60AC,  -158 },  // 1.0e+065
        { 0x1.2FDBB0E39FB47p+0219,   0x4AD431BB13CC4D56,  -161 },  // 1.0e+066
        { 0x1.7BD29D1C87A19p+0222,   0x77B9E92B52E07BBE,  -165 },  // 1.0e+067
        { 0x1.DAC74463A989Fp+0225,   0x5FC7EDBC424D2FCB,  -168 },  // 1.0e+068
        { 0x1.28BC8ABE49F63p+0229,   0x4C9FF163683DBFD5,  -171 },  // 1.0e+069
        { 0x1.72EBAD6DDC73Cp+0232,   0x7A998238A6C932EF,  -175 },  // 1.0e+070
        { 0x1.CFA698C95390Bp+0235,   0x6214682D523A8F26,  -178 },  // 1.0e+071
        { 0x1.21C81F7DD43A7p+0239,   0x4E76B9BDDB620C1E,  -181 },  // 1.0e+072
        { 0x1.6A3A275D49491p+0242,   0x7D8AC2C95F034697,  -185 },  // 1.0e+073
        { 0x1.C4C8B1349B9B5p+0245,   0x646F023AB2690545,  -188 },  // 1.0e+074
        { 0x1.1AFD6EC0E1411p+0249,   0x5058CE955B87376B,  -191 },  // 1.0e+075
        { 0x1.61BCCA7119915p+0252,   0x40470BAAAF9F5F88,  -194 },  // 1.0e+076
        { 0x1.BA2BFD0D5FF5Bp+0255,   0x66D812AAB29898DB,  -198 },  // 1.0e+077
        { 0x1.145B7E285BF98p+0259,   0x524675555BAD4715,  -201 },  // 1.0e+078
        { 0x1.59725DB272F7Fp+0262,   0x41D1F7777C8A9F44,  -204 },  // 1.0e+079
        { 0x1.AFCEF51F0FB5Ep+0265,   0x694FF258C7443207,  -208 },  // 1.0e+080
        { 0x1.0DE1593369D1Bp+0269,   0x543FF513D29CF4D2,  -211 },  // 1.0e+081
        { 0x1.5159AF8044462p+0272,   0x43665DA9754A5D75,  -214 },  // 1.0e+082
        { 0x1.A5B01B605557Ap+0275,   0x6BD6FC425543C8BB,  -218 },  // 1.0e+083
        { 0x1.078E111C3556Cp+0279,   0x5645969B77696D62,  -221 },  // 1.0e+084
        { 0x1.4971956342AC7p+0282,   0x4504787C5F878AB5,  -224 },  // 1.0e+085
        { 0x1.9BCDFABC13579p+0285,   0x6E6D8D93CC0C1122,  -228 },  // 1.0e+086
        { 0x1.0160BCB58C16Cp+0289,   0x5857A4763CD6741B,  -231 },  // 1.0e+087
        { 0x1.41B8EBE2EF1C7p+0292,   0x46AC8391CA4529AF,  -234 },  // 1.0e+088
        { 0x1.922726DBAAE39p+0295,   0x711405B6106EA919,  -238 },  // 1.0e+089
        { 0x1.F6B0F092959C7p+0298,   0x5A766AF80D255414,  -241 },  // 1.0e+090
        { 0x1.3A2E965B9D81Cp+0302,   0x485EBBF9A41DDCDC,  -244 },  // 1.0e+091
        { 0x1.88BA3BF284E23p+0305,   0x73CAC65C39C96161,  -248 },  // 1.0e+092
        { 0x1.EAE8CAEF261ACp+0308,   0x5CA23849C7D44DE7,  -251 },  // 1.0e+093
        { 0x1.32D17ED577D0Bp+0312,   0x4A1B603B06437185,  -254 },  // 1.0e+094
        { 0x1.7F85DE8AD5C4Ep+0315,   0x76923391A39F1C09,  -258 },  // 1.0e+095
        { 0x1.DF67562D8B362p+0318,   0x5EDB5C7482E5B007,  -261 },  // 1.0e+096
        { 0x1.2BA095DC7701Dp+0322,   0x4BE2B05D35848CD2,  -264 },  // 1.0e+097
        { 0x1.7688BB5394C25p+0325,   0x796AB3C855A0E151,  -268 },  // 1.0e+098
        { 0x1.D42AEA2879F2Ep+0328,   0x6122296D114D810D,  -271 },  // 1.0e+099
        { 0x1.249AD2594C37Cp+0332,   0x4DB4EDF0DAA4673E,  -274 },  // 1.0e+100
        { 0x1.6DC186EF9F45Cp+0335,   0x7C54AFE7C43A3ECA,  -278 },  // 1.0e+101
        { 0x1.C931E8AB87173p+0338,   0x6376F31FD02E98A1,  -281 },  // 1.0e+102
        { 0x1.1DBF316B346E7p+0342,   0x4F925C1973587A1B,  -284 },  // 1.0e+103
        { 0x1.652EFDC6018A1p+0345,   0x7F50935BEBC0C35E,  -288 },  // 1.0e+104
        { 0x1.BE7ABD3781ECAp+0348,   0x65DA0F7CBC9A35E5,  -291 },  // 1.0e+105
        { 0x1.170CB642B133Ep+0352,   0x517B3F96FD482B1D,  -294 },  // 1.0e+106
        { 0x1.5CCFE3D35D80Ep+0355,   0x412F66126439BC17,  -297 },  // 1.0e+107
        { 0x1.B403DCC834E11p+0358,   0x684BD683D38F9359,  -301 },  // 1.0e+108
        { 0x1.108269FD210CBp+0362,   0x536FDECFDC72DC47,  -304 },  // 1.0e+109
        { 0x1.54A3047C694FDp+0365,   0x42BFE57316C249D2,  -307 },  // 1.0e+110
        { 0x1.A9CBC59B83A3Dp+0368,   0x6ACCA251BE03A951,  -311 },  // 1.0e+111
        { 0x1.0A1F5B8132466p+0372,   0x557081DAFE695440,  -314 },  // 1.0e+112
        { 0x1.4CA732617ED7Fp+0375,   0x445A017BFEBAA9CD,  -317 },  // 1.0e+113
        { 0x1.9FD0FEF9DE8DFp+0378,   0x6D5CCF2CCAC442E2,  -321 },  // 1.0e+114
        { 0x1.03E29F5C2B18Bp+0382,   0x577D728A3BD03581,  -324 },  // 1.0e+115
        { 0x1.44DB473335DEEp+0385,   0x45FDF53B630CF79B,  -327 },  // 1.0e+116
        { 0x1.961219000356Ap+0388,   0x6FFCBB923814BF5E,  -331 },  // 1.0e+117
        { 0x1.FB969F40042C5p+0391,   0x5996FC74F9AA32B2,  -334 },  // 1.0e+118
        { 0x1.3D3E2388029BBp+0395,   0x47ABFD2A6154F55B,  -337 },  // 1.0e+119
        { 0x1.8C8DAC6A0342Ap+0398,   0x72ACC843CEEE555E,  -341 },  // 1.0e+120
        { 0x1.EFB1178484134p+0401,   0x5BBD6D030BF1DDE5,  -344 },  // 1.0e+121
        { 0x1.35CEAEB2D28C0p+0405,   0x49645735A327E4B7,  -347 },  // 1.0e+122
        { 0x1.83425A5F872F1p+0408,   0x756D5855D1D96DF2,  -351 },  // 1.0e+123
        { 0x1.E412F0F768FADp+0411,   0x5DF11377DB1457F5,  -354 },  // 1.0e+124
        { 0x1.2E8BD69AA19CCp+0415,   0x4B2742C648DD132A,  -357 },  // 1.0e+125
        { 0x1.7A2ECC414A03Fp+0418,   0x783ED13D4161B844,  -361 },  // 1.0e+126
        { 0x1.D8BA7F519C84Fp+0421,   0x603240FDCDE7C69C,  -364 },  // 1.0e+127
        { 0x1.27748F9301D31p+0425,   0x4CF500CB0B1FD217,  -367 },  // 1.0e+128
        { 0x1.7151B377C247Ep+0428,   0x7B219ADE7832E9BE,  -371 },  // 1.0e+129
        { 0x1.CDA62055B2D9Dp+0431,   0x628148B1F9C25498,  -374 },  // 1.0e+130
        { 0x1.2087D4358FC82p+0435,   0x4ECDD3C1949B76E0,  -377 },  // 1.0e+131
        { 0x1.68A9C942F3BA3p+0438,   0x7E161F9C20F8BE33,  -381 },  // 1.0e+132
        { 0x1.C2D43B93B0A8Bp+0441,   0x64DE7FB01A609829,  -384 },  // 1.0e+133
        { 0x1.19C4A53C4E697p+0445,   0x50B1FFC0151A1354,  -387 },  // 1.0e+134
        { 0x1.6035CE8B6203Dp+0448,   0x408E66334414DC43,  -390 },  // 1.0e+135
        { 0x1.B843422E3A84Cp+0451,   0x674A3D1ED354939F,  -394 },  // 1.0e+136
        { 0x1.132A095CE492Fp+0455,   0x52A1CA7F0F76DC7F,  -397 },  // 1.0e+137
        { 0x1.57F48BB41DB7Bp+0458,   0x421B0865A5F8B065,  -400 },  // 1.0e+138
        { 0x1.ADF1AEA12525Ap+0461,   0x69C4DA3C3CC11A3C,  -404 },  // 1.0e+139
        { 0x1.0CB70D24B7378p+0465,   0x549D7B6363CDAE96,  -407 },  // 1.0e+140
        { 0x1.4FE4D06DE5056p+0468,   0x43B12F82B63E2545,  -410 },  // 1.0e+141
        { 0x1.A3DE04895E46Cp+0471,   0x6C4EB26ABD303BA2,  -414 },  // 1.0e+142
        { 0x1.066AC2D5DAEC3p+0475,   0x56A55B889759C94E,  -417 },  // 1.0e+143
        { 0x1.4805738B51A74p+0478,   0x45511606DF7B0772,  -420 },  // 1.0e+144
        { 0x1.9A06D06E26112p+0481,   0x6EE8233E325E7250,  -424 },  // 1.0e+145
        { 0x1.00444244D7CABp+0485,   0x58B9B5CB5B7EC1D9,  -427 },  // 1.0e+146
        { 0x1.405552D60DBD6p+0488,   0x46FAF7D5E2CBCE47,  -430 },  // 1.0e+147
        { 0x1.906AA78B912CBp+0491,   0x71918C896ADFB073,  -434 },  // 1.0e+148
        { 0x1.F485516E7577Ep+0494,   0x5ADAD6D4557FC05C,  -437 },  // 1.0e+149
        { 0x1.38D352E5096AFp+0498,   0x48AF1243779966B0,  -440 },  // 1.0e+150
        { 0x1.8708279E4BC5Ap+0501,   0x744B506BF28F0AB3,  -444 },  // 1.0e+151
        { 0x1.E8CA3185DEB71p+0504,   0x5D090D2328726EF5,  -447 },  // 1.0e+152
        { 0x1.317E5EF3AB327p+0508,   0x4A6DA41C205B8BF7,  -450 },  // 1.0e+153
        { 0x1.7DDDF6B095FF0p+0511,   0x7715D36033C5ACBF,  -454 },  // 1.0e+154
        { 0x1.DD55745CBB7ECp+0514,   0x5F44A919C3048A32,  -457 },  // 1.0e+155
        { 0x1.2A5568B9F52F4p+0518,   0x4C36EDAE359D3B5B,  -460 },  // 1.0e+156
        { 0x1.74EAC2E8727B1p+0521,   0x79F17C49EF61F893,  -464 },  // 1.0e+157
        { 0x1.D22573A28F19Dp+0524,   0x618DFD07F2B4C6DC,  -467 },  // 1.0e+158
        { 0x1.2357684599702p+0528,   0x4E0B30D328909F16,  -470 },  // 1.0e+159
        { 0x1.6C2D4256FFCC2p+0531,   0x7CDEB4850DB431BD,  -474 },  // 1.0e+160
        { 0x1.C73892ECBFBF3p+0534,   0x63E55D373E29C164,  -477 },  // 1.0e+161
        { 0x1.1C835BD3F7D78p+0538,   0x4FEAB0F8FE87CDE9,  -480 },  // 1.0e+162
        { 0x1.63A432C8F5CD6p+0541,   0x7FDDE7F4CA72E30F,  -484 },  // 1.0e+163
        { 0x1.BC8D3F7B3340Bp+0544,   0x664B1FF7085BE8D9,  -487 },  // 1.0e+164
        { 0x1.15D847AD00087p+0548,   0x51D5B32C06AFED7A,  -490 },  // 1.0e+165
        { 0x1.5B4E5998400A9p+0551,   0x4177C2899EF32462,  -493 },  // 1.0e+166
        { 0x1.B221EFFE500D3p+0554,   0x68BF9DA8FE51D3D0,  -497 },  // 1.0e+167
        { 0x1.0F5535FEF2084p+0558,   0x53CC7E20CB74A973,  -500 },  // 1.0e+168
        { 0x1.532A837EAE8A5p+0561,   0x4309FE80A2C3BAC2,  -503 },  // 1.0e+169
        { 0x1.A7F5245E5A2CEp+0564,   0x6B4330CDD1392AD1,  -507 },  // 1.0e+170
        { 0x1.08F936BAF85C1p+0568,   0x55CF5A3E40FA88A7,  -510 },  // 1.0e+171
        { 0x1.4B378469B6731p+0571,   0x44A5E1CB672ED3B9,  -513 },  // 1.0e+172
        { 0x1.9E056584240FDp+0574,   0x6DD636123EB152C1,  -517 },  // 1.0e+173
        { 0x1.02C35F729689Ep+0578,   0x57DE91A832277567,  -520 },  // 1.0e+174
        { 0x1.4374374F3C2C6p+0581,   0x464BA7B9C1B92AB9,  -523 },  // 1.0e+175
        { 0x1.945145230B377p+0584,   0x70790C5C6928445C,  -527 },  // 1.0e+176
        { 0x1.F965966BCE055p+0587,   0x59FA7049EDB9D049,  -530 },  // 1.0e+177
        { 0x1.3BDF7E0360C35p+0591,   0x47FB8D07F161736E,  -533 },  // 1.0e+178
        { 0x1.8AD75D8438F43p+0594,   0x732C14D98235857D,  -537 },  // 1.0e+179
        { 0x1.ED8D34E547313p+0597,   0x5C2343E134F79DFD,  -540 },  // 1.0e+180
        { 0x1.3478410F4C7ECp+0601,   0x49B5CFE75D92E4CA,  -543 },  // 1.0e+181
        { 0x1.819651531F9E7p+0604,   0x75EFB30BC8EB07AB,  -547 },  // 1.0e+182
        { 0x1.E1FBE5A7E7861p+0607,   0x5E595C096D88D2EF,  -550 },  // 1.0e+183
        { 0x1.2D3D6F88F0B3Cp+0611,   0x4B7AB0078AD3DBF2,  -553 },  // 1.0e+184
        { 0x1.788CCB6B2CE0Cp+0614,   0x78C44CD8DE1FC650,  -557 },  // 1.0e+185
        { 0x1.D6AFFE45F818Fp+0617,   0x609D0A4718196B73,  -560 },  // 1.0e+186
        { 0x1.262DFEEBBB0F9p+0621,   0x4D4A6E9F467ABC5C,  -563 },  // 1.0e+187
        { 0x1.6FB97EA6A9D37p+0624,   0x7BAA4A9870C46094,  -567 },  // 1.0e+188
        { 0x1.CBA7DE5054485p+0627,   0x62EEA2138D69E6DD,  -570 },  // 1.0e+189
        { 0x1.1F48EAF234AD3p+0631,   0x4F254E760ABB1F17,  -573 },  // 1.0e+190
        { 0x1.671B25AEC1D88p+0634,   0x7EA21723445E9825,  -577 },  // 1.0e+191
        { 0x1.C0E1EF1A724EAp+0637,   0x654E78E9037EE01D,  -580 },  // 1.0e+192
        { 0x1.188D357087712p+0641,   0x510B93ED9C658017,  -583 },  // 1.0e+193
        { 0x1.5EB082CCA94D7p+0644,   0x40D60FF149EACCDF,  -586 },  // 1.0e+194
        { 0x1.B65CA37FD3A0Dp+0647,   0x67BCE64EDCAAE166,  -590 },  // 1.0e+195
        { 0x1.11F9E62FE4448p+0651,   0x52FD850BE3BBE784,  -593 },  // 1.0e+196
        { 0x1.56785FBBDD55Ap+0654,   0x42646A6FE9631F9D,  -596 },  // 1.0e+197
        { 0x1.AC1677AAD4AB0p+0657,   0x6A3A43E642383295,  -600 },  // 1.0e+198
        { 0x1.0B8E0ACAC4EAEp+0661,   0x54FB698501C68EDE,  -603 },  // 1.0e+199
        { 0x1.4E718D7D7625Ap+0664,   0x43FC546A67D20BE4,  -606 },  // 1.0e+200
        { 0x1.A20DF0DCD3AF0p+0667,   0x6CC6ED770C83463B,  -610 },  // 1.0e+201
        { 0x1.0548B68A044D6p+0671,   0x57058AC5A39C382F,  -613 },  // 1.0e+202
        { 0x1.469AE42C8560Cp+0674,   0x459E089E1C7CF9BF,  -616 },  // 1.0e+203
        { 0x1.98419D37A6B8Fp+0677,   0x6F6340FCFA618F98,  -620 },  // 1.0e+204
        { 0x1.FE52048590672p+0680,   0x591C33FD951AD946,  -623 },  // 1.0e+205
        { 0x1.3EF342D37A407p+0684,   0x4749C33144157A9F,  -626 },  // 1.0e+206
        { 0x1.8EB0138858D09p+0687,   0x720F9EB539BBF765,  -630 },  // 1.0e+207
        { 0x1.F25C186A6F04Cp+0690,   0x5B3FB22A94965F84,  -633 },  // 1.0e+208
        { 0x1.37798F428562Fp+0694,   0x48FFC1BBAA11E603,  -636 },  // 1.0e+209
        { 0x1.8557F31326BBBp+0697,   0x74CC692C434FD66B,  -640 },  // 1.0e+210
        { 0x1.E6ADEFD7F06AAp+0700,   0x5D705423690CAB89,  -643 },  // 1.0e+211
        { 0x1.302CB5E6F642Ap+0704,   0x4AC0434F873D5607,  -646 },  // 1.0e+212
        { 0x1.7C37E360B3D35p+0707,   0x779A054C0B955672,  -650 },  // 1.0e+213
        { 0x1.DB45DC38E0C82p+0710,   0x5FAE6AA33C77785B,  -653 },  // 1.0e+214
        { 0x1.290BA9A38C7D1p+0714,   0x4C8B888296C5F9E2,  -656 },  // 1.0e+215
        { 0x1.734E940C6F9C5p+0717,   0x7A78DA6A8AD65C9D,  -660 },  // 1.0e+216
        { 0x1.D022390F8B837p+0720,   0x61FA48553BDEB07E,  -663 },  // 1.0e+217
        { 0x1.221563A9B7322p+0724,   0x4E61D37763188D31,  -666 },  // 1.0e+218
        { 0x1.6A9ABC9424FEBp+0727,   0x7D6952589E8DAEB6,  -670 },  // 1.0e+219
        { 0x1.C5416BB92E3E6p+0730,   0x645441E07ED7BEF8,  -673 },  // 1.0e+220
        { 0x1.1B48E353BCE6Fp+0734,   0x504367E6CBDFCBF9,  -676 },  // 1.0e+221
        { 0x1.621B1C28AC20Bp+0737,   0x4035ECB8A3196FFB,  -679 },  // 1.0e+222
        { 0x1.BAA1E332D728Ep+0740,   0x66BCADF43828B32B,  -683 },  // 1.0e+223
        { 0x1.14A52DFFC6799p+0744,   0x52308B29C686F5BC,  -686 },  // 1.0e+224
        { 0x1.59CE797FB817Fp+0747,   0x41C06F549ED25E30,  -689 },  // 1.0e+225
        { 0x1.B04217DFA61DFp+0750,   0x6933E554315096B3,  -693 },  // 1.0e+226
        { 0x1.0E294EEBC7D2Bp+0754,   0x542984435AA6DEF5,  -696 },  // 1.0e+227
        { 0x1.51B3A2A6B9C76p+0757,   0x435469CF7BB8B25E,  -699 },  // 1.0e+228
        { 0x1.A6208B5068394p+0760,   0x6BBA42E592C11D63,  -703 },  // 1.0e+229
        { 0x1.07D457124123Cp+0764,   0x562E9BEADBCDB11C,  -706 },  // 1.0e+230
        { 0x1.49C96CD6D16CBp+0767,   0x44F216557CA48DB0,  -709 },  // 1.0e+231
        { 0x1.9C3BC80C85C7Ep+0770,   0x6E5023BBFAA0E2B3,  -713 },  // 1.0e+232
        { 0x1.01A55D07D39CFp+0774,   0x58401C96621A4EF6,  -716 },  // 1.0e+233
        { 0x1.420EB449C8842p+0777,   0x4699B0784E7B725E,  -719 },  // 1.0e+234
        { 0x1.9292615C3AA53p+0780,   0x70F5E726E3F8B6FD,  -723 },  // 1.0e+235
        { 0x1.F736F9B3494E8p+0783,   0x5A5E5285832D5F31,  -726 },  // 1.0e+236
        { 0x1.3A825C100DD11p+0787,   0x484B75379C244C27,  -729 },  // 1.0e+237
        { 0x1.8922F31411455p+0790,   0x73ABEEBF603A1372,  -733 },  // 1.0e+238
        { 0x1.EB6BAFD91596Bp+0793,   0x5C898BCC4CFB42C2,  -736 },  // 1.0e+239
        { 0x1.33234DE7AD7E2p+0797,   0x4A07A309D72F689B,  -739 },  // 1.0e+240
        { 0x1.7FEC216198DDBp+0800,   0x76729E762518A75E,  -743 },  // 1.0e+241
        { 0x1.DFE729B9FF152p+0803,   0x5EC2185E8413B918,  -746 },  // 1.0e+242
        { 0x1.2BF07A143F6D3p+0807,   0x4BCE79E536762DAD,  -749 },  // 1.0e+243
        { 0x1.76EC98994F488p+0810,   0x794A5CA1F0BD15E2,  -753 },  // 1.0e+244
        { 0x1.D4A7BEBFA31AAp+0813,   0x61084A1B26FDAB1B,  -756 },  // 1.0e+245
        { 0x1.24E8D737C5F0Ap+0817,   0x4DA03B48EBFE227C,  -759 },  // 1.0e+246
        { 0x1.6E230D05B76CDp+0820,   0x7C33920E46636A60,  -763 },  // 1.0e+247
        { 0x1.C9ABD04725480p+0823,   0x635C74D8384F884D,  -766 },  // 1.0e+248
        { 0x1.1E0B622C774D0p+0827,   0x4F7D2A469372D370,  -769 },  // 1.0e+249
        { 0x1.658E3AB795204p+0830,   0x7F2EAA0A85848581,  -773 },  // 1.0e+250
        { 0x1.BEF1C9657A685p+0833,   0x65BEEE6ED136D134,  -776 },  // 1.0e+251
        { 0x1.17571DDF6C813p+0837,   0x51658B8BDA9240F6,  -779 },  // 1.0e+252
        { 0x1.5D2CE55747A18p+0840,   0x411E093CAEDB672B,  -782 },  // 1.0e+253
        { 0x1.B4781EAD1989Ep+0843,   0x68300EC77E2BD845,  -786 },  // 1.0e+254
        { 0x1.10CB132C2FF63p+0847,   0x5359A56C64EFE037,  -789 },  // 1.0e+255
        { 0x1.54FDD7F73BF3Bp+0850,   0x42AE1DF050BFE693,  -792 },  // 1.0e+256
        { 0x1.AA3D4DF50AF0Ap+0853,   0x6AB02FE6E79970EB,  -796 },  // 1.0e+257
        { 0x1.0A6650B926D66p+0857,   0x5559BFEBEC7AC0BC,  -799 },  // 1.0e+258
        { 0x1.4CFFE4E7708C0p+0860,   0x4447CCBCBD2F0096,  -802 },  // 1.0e+259
        { 0x1.A03FDE214CAF0p+0863,   0x6D3FADFAC84B3424,  -806 },  // 1.0e+260
        { 0x1.0427EAD4CFED6p+0867,   0x576624C8A03C29B6,  -809 },  // 1.0e+261
        { 0x1.4531E58A03E8Bp+0870,   0x45EB50A08030215E,  -812 },  // 1.0e+262
        { 0x1.967E5EEC84E2Ep+0873,   0x6FDEE76733803564,  -816 },  // 1.0e+263
        { 0x1.FC1DF6A7A61BAp+0876,   0x597F1F85C2CCF783,  -819 },  // 1.0e+264
        { 0x1.3D92BA28C7D14p+0880,   0x4798E6049BD72C69,  -822 },  // 1.0e+265
        { 0x1.8CF768B2F9C59p+0883,   0x728E3CD42C8B7A42,  -826 },  // 1.0e+266
        { 0x1.F03542DFB8370p+0886,   0x5BA4FD768A092E9B,  -829 },  // 1.0e+267
        { 0x1.362149CBD3226p+0890,   0x4950CAC53B3A8BAF,  -832 },  // 1.0e+268
        { 0x1.83A99C3EC7EAFp+0893,   0x754E113B91F745E5,  -836 },  // 1.0e+269
        { 0x1.E494034E79E5Bp+0896,   0x5DD80DC941929E51,  -839 },  // 1.0e+270
        { 0x1.2EDC82110C2F9p+0900,   0x4B133E3A9ADBB1DA,  -842 },  // 1.0e+271
        { 0x1.7A93A2954F3B7p+0903,   0x781EC9F75E2C4FC4,  -846 },  // 1.0e+272
        { 0x1.D9388B3AA30A5p+0906,   0x6018A192B1BD0C9C,  -849 },  // 1.0e+273
        { 0x1.27C35704A5E67p+0910,   0x4CE0814227CA707D,  -852 },  // 1.0e+274
        { 0x1.71B42CC5CF601p+0913,   0x7B00CED03FAA4D95,  -856 },  // 1.0e+275
        { 0x1.CE2137F743381p+0916,   0x62670BD9CC883E11,  -859 },  // 1.0e+276
        { 0x1.20D4C2FA8A030p+0920,   0x4EB8D647D6D364DA,  -862 },  // 1.0e+277
        { 0x1.6909F3B92C83Dp+0923,   0x7DF48A0C8AEBD491,  -866 },  // 1.0e+278
        { 0x1.C34C70A777A4Cp+0926,   0x64C3A1A3A25643A7,  -869 },  // 1.0e+279
        { 0x1.1A0FC668AAC6Fp+0930,   0x509C814FB511CFB9,  -872 },  // 1.0e+280
        { 0x1.6093B802D578Bp+0933,   0x407D343FC40E3FC7,  -875 },  // 1.0e+281
        { 0x1.B8B8A6038AD6Ep+0936,   0x672EB9FFA016CC71,  -879 },  // 1.0e+282
        { 0x1.137367C236C65p+0940,   0x528BC7FFB345705B,  -882 },  // 1.0e+283
        { 0x1.585041B2C477Ep+0943,   0x42096CCC8F6AC048,  -885 },  // 1.0e+284
        { 0x1.AE64521F7595Ep+0946,   0x69A8AE1418AACD41,  -889 },  // 1.0e+285
        { 0x1.0CFEB353A97DAp+0950,   0x5486F1A9AD557101,  -892 },  // 1.0e+286
        { 0x1.503E602893DD1p+0953,   0x439F27BAF1112734,  -895 },  // 1.0e+287
        { 0x1.A44DF832B8D45p+0956,   0x6C31D92B1B4EA520,  -899 },  // 1.0e+288
        { 0x1.06B0BB1FB384Bp+0960,   0x568E4755AF721DB3,  -902 },  // 1.0e+289
        { 0x1.485CE9E7A065Ep+0963,   0x453E9F77BF8E7E29,  -905 },  // 1.0e+290
        { 0x1.9A742461887F6p+0966,   0x6ECA98BF98E3FD0E,  -909 },  // 1.0e+291
        { 0x1.008896BCF54F9p+0970,   0x58A213CC7A4FFDA5,  -912 },  // 1.0e+292
        { 0x1.40AABC6C32A38p+0973,   0x46E80FD6C83FFE1D,  -915 },  // 1.0e+293
        { 0x1.90D56B873F4C6p+0976,   0x71734C8AD9FFFCFC,  -919 },  // 1.0e+294
        { 0x1.F50AC6690F1F8p+0979,   0x5AC2A3A247FFFD96,  -922 },  // 1.0e+295
        { 0x1.3926BC01A973Bp+0983,   0x489BB61B6CCCCADF,  -925 },  // 1.0e+296
        { 0x1.87706B0213D09p+0986,   0x742C569247AE1164,  -929 },  // 1.0e+297
        { 0x1.E94C85C298C4Cp+0989,   0x5CF04541D2F1A783,  -932 },  // 1.0e+298
        { 0x1.31CFD3999F7AFp+0993,   0x4A59D101758E1F9C,  -935 },  // 1.0e+299
        { 0x1.7E43C8800759Bp+0996,   0x76F61B3588E365C7,  -939 },  // 1.0e+300
        { 0x1.DDD4BAA009302p+0999,   0x5F2B48F7A0B5EB06,  -942 },  // 1.0e+301
        { 0x1.2AA4F4A405BE1p+1003,   0x4C22A0C61A2B226B,  -945 },  // 1.0e+302
        { 0x1.754E31CD072D9p+1006,   0x79D1013CF6AB6A45,  -949 },  // 1.0e+303
        { 0x1.D2A1BE4048F90p+1009,   0x617400FD9222BB6A,  -952 },  // 1.0e+304
        { 0x1.23A516E82D9BAp+1013,   0x4DF6673141B562BB,  -955 },  // 1.0e+305
        { 0x1.6C8E5CA239028p+1016,   0x7CBD71E869223792,  -959 },  // 1.0e+306
        { 0x1.C7B1F3CAC7433p+1019,   0x63CAC186BA81C60E,  -962 },  // 1.0e+307
        { 0x1.1CCF385EBC89Fp+1023,   0x4FD5679EFB9B04D8,  -965 },  // 1.0e+308
      };
    static_assert(noadl::countof(s_decmult_F) == 633, "");

    void do_xfrexp_F_dec(uint64_t& mant, int& exp, const double& value) noexcept
      {
        // Note if `value` is not finite then the behavior is undefined.
        // Get the first digit.
        double freg = ::std::fabs(value);
        // Locate the last number in the table that is <= `freg`.
        uint32_t bpos = 1;
        uint32_t epos = noadl::countof(s_decmult_F);
        for(;;) {
          // Stop if the range is empty.
          if(bpos == epos) {
            bpos = epos - 1;
            break;
          }
          // Get the median.
          uint32_t mpos = (bpos + epos) / 2;
          const double& med = s_decmult_F[mpos].bound;
          // Stops immediately if `freg` equals `med`.
          if(::std::memcmp(&freg, &med, sizeof(double)) == 0) {
            bpos = mpos;
            break;
          }
          // Decend into either subrange.
          if(freg < med)
            epos = mpos;
          else
            bpos = mpos + 1;
        }
        // Get the multiplier.
        const auto& mult = s_decmult_F[bpos];
        // Extract the mantissa.
        freg = ::std::ldexp(freg, mult.bexp);
        uint64_t ireg = static_cast<uint64_t>(static_cast<int64_t>(freg) | 1);
        // Multiply two 64-bit values and get the high-order half.
        // TODO: Modern CPUs have intrinsics for this.
        uint64_t xhi = ireg >> 32;
        uint64_t xlo = ireg & UINT32_MAX;
        uint64_t yhi = mult.mant >> 32;
        uint64_t ylo = mult.mant & UINT32_MAX;
        ireg = xhi * yhi + (xlo * yhi >> 32) + (xhi * ylo >> 32);
        // Return the mantissa and exponent.
        mant = ireg;
        exp = static_cast<int>(bpos) - 324;
      }

    void do_xput_M_dec(char*& ep, const uint64_t& mant, const char* rdxp) noexcept
      {
        uint64_t ireg = mant;
        if(ireg != 0) {
          // Write digits in reverse order.
          char temps[24];
          char* tbp = end(temps);
          while(ireg != 0) {
            // Shift a digit out.
            uint8_t dval = static_cast<uint8_t>(ireg % 10);
            ireg /= 10;
            // Write this digit unless it is amongst trailing zeroes.
            if((tbp != end(temps)) || (dval != 0))
              *(--tbp) = do_pdigit_D(dval);
          }
          // Pop digits and append them to `ep`.
          while(tbp != end(temps)) {
            // Insert a decimal point before `rdxp`.
            if(ep == rdxp)
              *(ep++) = '.';
            // Write this digit.
            *(ep++) = *(tbp++);
          }
        }
        // If `rdxp` is set, fill zeroes until it is reached,
        // if no decimal point has been added so far.
        if(rdxp)
          while(ep < rdxp)
            *(ep++) = '0';
      }

    }  // namespace

ascii_numput& ascii_numput::put_DF(double value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += do_cast_U(sign + 1);
    }
    else {
      // Seek to the beginning of the internal buffer.
      bp = this->m_stor;
      ep = bp;
      // Prepend a minus sign if the number is negative.
      if(sign)
        *(ep++) = '-';
      // Break the number down into fractional and exponential parts. This result is approximate.
      uint64_t mant;
      int exp;
      do_xfrexp_F_dec(mant, exp, value);
      // Write the broken-down number...
      if((exp < -4) || (17 <= exp)) {
        // ... in scientific notation.
        do_xput_M_dec(ep, mant, ep + 1);
        *(ep++) = 'e';
        do_xput_I_exp(ep, exp);
      }
      else if(exp < 0) {
        // ... in plain format; the number starts with "0."; zeroes are prepended as necessary.
        *(ep++) = '0';
        *(ep++) = '.';
        noadl::ranged_for(exp, -1, [&](int) { *(ep++) = '0';  });
        do_xput_M_dec(ep, mant, nullptr);
      }
      else {
        // ... in plain format; the decimal point is inserted in the middle.
        do_xput_M_dec(ep, mant, ep + 1 + do_cast_U(exp));
      }
      // Append a null terminator.
      *ep = 0;
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

ascii_numput& ascii_numput::put_DE(double value) noexcept
  {
    this->clear();
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += do_cast_U(sign + 1);
    }
    else {
      // Seek to the beginning of the internal buffer.
      bp = this->m_stor;
      ep = bp;
      // Prepend a minus sign if the number is negative.
      if(sign)
        *(ep++) = '-';
      // Break the number down into fractional and exponential parts. This result is approximate.
      uint64_t mant;
      int exp;
      do_xfrexp_F_dec(mant, exp, value);
      // Write the broken-down number in scientific notation.
      do_xput_M_dec(ep, mant, ep + 1);
      *(ep++) = 'e';
      do_xput_I_exp(ep, exp);
      // Append a null terminator.
      *ep = 0;
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

}  // namespace rocket
