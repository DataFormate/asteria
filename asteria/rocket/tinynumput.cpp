// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "tinynumput.hpp"
#include <cmath>

namespace rocket {

tinynumput& tinynumput::put_TB(bool value) noexcept
  {
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

tinynumput& tinynumput::put_XP(const void* value) noexcept
  {
    char* bp = this->m_stor;
    char* ep = bp;
    // Write the hexadecimal prefix.
    *(ep++) = '0';
    *(ep++) = 'x';
    // Write all digits in normal order.
    // This is done by extract digits from the more significant side.
    uintptr_t reg = reinterpret_cast<uintptr_t>(value);
    constexpr int nbits = numeric_limits<uintptr_t>::digits;
    for(int i = 0; i != nbits / 4; ++i) {
      // Shift a digit out.
      size_t dval = reg >> (nbits - 4);
      reg <<= 4;
      // Write this digit.
      *(ep++) = "0123456789ABCDEF"[dval];
    }
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

    namespace {

    template<uint8_t radixT, typename valueT,
             ROCKET_ENABLE_IF(is_unsigned<valueT>::value)>
        char* do_xput_U_bkwd(char*& bp, const valueT& value, size_t width = 1)
      {
        char* stop = bp - width;
        // Write digits backwards.
        valueT reg = value;
        while(reg != 0) {
          // Shift a digit out.
          size_t dval = reg % radixT;
          reg /= radixT;
          // Write this digit.
          *--bp = "0123456789ABCDEF"[dval];
        }
        // Pad the string to at least the width requested.
        while(bp > stop)
          *--bp = '0';
        return bp;
      }

    }  // namespace

tinynumput& tinynumput::put_BU(uint32_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Write digits backwards.
    do_xput_U_bkwd<2>(bp, value);
    // Prepend the binary prefix.
    *--bp = 'b';
    *--bp = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_BQ(uint64_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Write digits backwards.
    do_xput_U_bkwd<2>(bp, value);
    // Prepend the binary prefix.
    *--bp = 'b';
    *--bp = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_DU(uint32_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Write digits backwards.
    do_xput_U_bkwd<10>(bp, value);
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_DQ(uint64_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Write digits backwards.
    do_xput_U_bkwd<10>(bp, value);
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_XU(uint32_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Write digits backwards.
    do_xput_U_bkwd<16>(bp, value);
    // Prepend the hexadecimal prefix.
    *--bp = 'x';
    *--bp = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_XQ(uint64_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Write digits backwards.
    do_xput_U_bkwd<16>(bp, value);
    // Prepend the hexadecimal prefix.
    *--bp = 'x';
    *--bp = '0';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_BI(int32_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint32_t sign = static_cast<uint32_t>(value >> 31);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd<2>(bp, (static_cast<uint32_t>(value) ^ sign) - sign);
    // Prepend the binary prefix.
    *--bp = 'b';
    *--bp = '0';
    // If the number is negative, prepend a minus sign.
    if(sign)
      *--bp = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_BL(int64_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint64_t sign = static_cast<uint64_t>(value >> 63);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd<2>(bp, (static_cast<uint64_t>(value) ^ sign) - sign);
    // Prepend the binary prefix.
    *--bp = 'b';
    *--bp = '0';
    // If the number is negative, prepend a minus sign.
    if(sign)
      *--bp = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_DI(int32_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint32_t sign = static_cast<uint32_t>(value >> 31);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd<10>(bp, (static_cast<uint32_t>(value) ^ sign) - sign);
    // If the number is negative, prepend a minus sign.
    if(sign)
      *--bp = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_DL(int64_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint64_t sign = static_cast<uint64_t>(value >> 63);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd<10>(bp, (static_cast<uint64_t>(value) ^ sign) - sign);
    // If the number is negative, prepend a minus sign.
    if(sign)
      *--bp = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_XI(int32_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint32_t sign = static_cast<uint32_t>(value >> 31);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd<16>(bp, (static_cast<uint32_t>(value) ^ sign) - sign);
    // Prepend the hexadecimal prefix.
    *--bp = 'x';
    *--bp = '0';
    // If the number is negative, prepend a minus sign.
    if(sign)
      *--bp = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_XL(int64_t value) noexcept
  {
    char* bp = this->m_stor + M;
    char* ep = bp;
    // Extend the sign bit to a word, assuming arithmetic shift.
    uint64_t sign = static_cast<uint64_t>(value >> 63);
    // Write digits backwards using its absolute value without causing overflows.
    do_xput_U_bkwd<16>(bp, (static_cast<uint64_t>(value) ^ sign) - sign);
    // Prepend the hexadecimal prefix.
    *--bp = 'x';
    *--bp = '0';
    // If the number is negative, prepend a minus sign.
    if(sign)
      *--bp = '-';
    // Set the string, which resides in the internal storage.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

    namespace {

    template<typename valueT>
        char* do_check_special_opt(char*& bp, char*& ep, const valueT& value)
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

    char* do_xput_I_exp(char*& ep, const int& exp)
      {
        // Append the sign symbol, always.
        if(exp < 0)
          *(ep++) = '-';
        else
          *(ep++) = '+';
        // Write the exponent in this temporary storage backwards.
        // The exponent shall contain at least two digits.
        char exps[8];
        char* expb = ::std::end(exps);
        do_xput_U_bkwd<10>(expb, static_cast<unsigned>(::std::abs(exp)), 2);
        // Append the exponent.
        noadl::ranged_for(expb, ::std::end(exps), [&](const char* p) { *(ep++) = *p;  });
        return ep;
      }

    char* do_xput_F_bin(char*& ep, const double& frac, const char* dp_opt)
      {
        // Write digits in normal order.
        uint64_t reg = static_cast<uint64_t>(static_cast<int64_t>(frac)) << 8;
        while(reg != 0) {
          // Shift a digit out.
          size_t dval = static_cast<size_t>(reg >> 63);
          reg <<= 1;
          // Insert a decimal point before `dp_opt`.
          if(ep == dp_opt)
            *(ep++) = '.';
          // Write this digit.
          *(ep++) = static_cast<char>('0' + dval);
        }
        return ep;
      }

    }  // namespace


tinynumput& tinynumput::put_BF(double value) noexcept
  {
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special_opt(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += static_cast<unsigned>(sign + 1);
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
      int exp;
      double frac = ::std::frexp(::std::fabs(value), &exp);
      --exp;
      // Normalize the integral part so it is the maximum value in the range [0,2).
      // The significant value is adjusted into the range [0,0x1p56).
      frac = ::std::ldexp(frac, 56);
      // Write the broken-down number...
      if((exp < -4) || (53 <= exp)) {
        // ... in scientific notation.
        do_xput_F_bin(ep, frac, ep + 1);
        *(ep++) = 'p';
        do_xput_I_exp(ep, exp);
      }
      else if(exp < 0) {
        // ... in plain format; the number starts with "0."; zeroes are prepended as necessary.
        *(ep++) = '0';
        *(ep++) = '.';
        noadl::ranged_for(exp, -1, [&](int) { *(ep++) = '0';  });
        do_xput_F_bin(ep, frac, nullptr);
      }
      else
        // ... in plain format; the decimal is inserted in the middle.
        do_xput_F_bin(ep, frac, ep + 1 + static_cast<unsigned>(exp));
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_BE(double value) noexcept
  {
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special_opt(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += static_cast<unsigned>(sign + 1);
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
      int exp;
      double frac = ::std::frexp(::std::fabs(value), &exp);
      --exp;
      // Normalize the integral part so it is the maximum value in the range [0,2).
      // The significant value is adjusted into the range [0,0x1p56).
      frac = ::std::ldexp(frac, 56);
      // Write the broken-down number in scientific notation.
      do_xput_F_bin(ep, frac, ep + 1);
      *(ep++) = 'p';
      do_xput_I_exp(ep, exp);
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }
/*
tinynumput& tinynumput::put_DF(double value) noexcept
  {
  }

tinynumput& tinynumput::put_DE(double value) noexcept
  {
  }
*/
    namespace {

    char* do_xput_F_hex(char*& ep, const double& frac, const char* dp_opt)
      {
        // Write digits in normal order.
        uint64_t reg = static_cast<uint64_t>(static_cast<int64_t>(frac)) << 8;
        while(reg != 0) {
          // Shift a digit out.
          size_t dval = static_cast<size_t>(reg >> 60);
          reg <<= 4;
          // Insert a decimal point before `dp_opt`.
          if(ep == dp_opt)
            *(ep++) = '.';
          // Write this digit.
          *(ep++) = "0123456789ABCDEF"[dval];
        }
        return ep;
      }

    }  // namespace

tinynumput& tinynumput::put_XF(double value) noexcept
  {
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special_opt(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += static_cast<unsigned>(sign + 1);
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
      int exp;
      double frac = ::std::frexp(::std::fabs(value), &exp);
      --exp;
      // Normalize the integral part so it is the maximum value in the range [0,16).
      // The significant value is adjusted into the range [0,0x1p56).
      frac = ::std::ldexp(frac, 53 + (exp & 3));
      exp &= -4;
      // Write the broken-down number...
      if((exp < -16) || (53 <= exp)) {
        // ... in scientific notation.
        do_xput_F_hex(ep, frac, ep + 1);
        *(ep++) = 'p';
        do_xput_I_exp(ep, exp);
      }
      else if(exp < 0) {
        // ... in plain format; the number starts with "0."; zeroes are prepended as necessary.
        *(ep++) = '0';
        *(ep++) = '.';
        noadl::ranged_for(exp >> 2, -1, [&](int) { *(ep++) = '0';  });
        do_xput_F_hex(ep, frac, nullptr);
      }
      else
        // ... in plain format; the decimal is inserted in the middle.
        do_xput_F_hex(ep, frac, ep + 1 + static_cast<unsigned>(exp >> 2));
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

tinynumput& tinynumput::put_XE(double value) noexcept
  {
    char* bp;
    char* ep;
    // Extract the sign bit and extend it to a word.
    int sign = ::std::signbit(value) ? -1 : 0;
    // Treat non-finite values and zeroes specially.
    if(do_check_special_opt(bp, ep, value)) {
      // Use the template string literal, which is immutable.
      // Skip the minus sign if the sign bit is clear.
      bp += static_cast<unsigned>(sign + 1);
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
      int exp;
      double frac = ::std::frexp(::std::fabs(value), &exp);
      --exp;
      // Normalize the integral part so it is the maximum value in the range [0,16).
      // The significant value is adjusted into the range [0,0x1p56).
      frac = ::std::ldexp(frac, 53 + (exp & 3));
      exp &= -4;
      // Write the broken-down number in scientific notation.
      do_xput_F_hex(ep, frac, ep + 1);
      *(ep++) = 'p';
      do_xput_I_exp(ep, exp);
    }
    // Set the string. The internal storage is used for finite values only.
    this->m_bptr = bp;
    this->m_eptr = ep;
    return *this;
  }

}  // namespace rocket