// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#ifndef ROCKET_UNIQUE_POSIX_FILE_HPP_
#define ROCKET_UNIQUE_POSIX_FILE_HPP_

#include "unique_handle.hpp"
#include <stdio.h>  // ::FILE, ::fclose()

namespace rocket {

class posix_file_closer
  {
  public:
    using handle_type  = ::FILE*;
    using closer_type  = decltype(::fclose)*;

  private:
    closer_type m_cl;

  public:
    constexpr posix_file_closer(closer_type cl) noexcept
      :
        m_cl(cl)
      {
      }

  public:
    constexpr operator closer_type () const noexcept
      {
        return this->m_cl;
      }
    int operator()(handle_type fp) const noexcept
      {
        return this->close(fp);
      }

    constexpr handle_type null() const noexcept
      {
        return nullptr;
      }
    constexpr bool is_null(handle_type fp) const noexcept
      {
        return fp == nullptr;
      }
    int close(handle_type fp) const noexcept
      {
        if(this->m_cl)
          return this->m_cl(fp);
        else
          return 0;
      }
  };

using unique_posix_file  = unique_handle<::FILE*, posix_file_closer>;

}  // namespace rocket

#endif
