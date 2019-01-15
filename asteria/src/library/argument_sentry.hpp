// This file is part of Asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_LIBRARY_ARGUMENT_SENTRY_HPP_
#define ASTERIA_LIBRARY_ARGUMENT_SENTRY_HPP_

#include "../fwd.hpp"
#include "../runtime/reference.hpp"
#include "../runtime/value.hpp"
#include "../rocket/cow_vector.hpp"

namespace Asteria {

class Argument_sentry
  {
  public:
    struct State
      {
        const rocket::cow_vector<Reference> *args;
        unsigned offset;
        bool succeeded;
        bool finished;
      };

  private:
    rocket::cow_string m_name;
    bool m_throw_on_failure;

    // N.B. The contents of `m_state` can be copied elsewhere and back.
    // Any further operations will resume from that point.
    State m_state;

  public:
    explicit Argument_sentry(rocket::cow_string name) noexcept
      : m_name(std::move(name)), m_throw_on_failure(false),
        m_state()
      {
      }
    ROCKET_NONCOPYABLE_DESTRUCTOR(Argument_sentry);

  private:
    template<typename XvalueT>
      Argument_sentry & do_get_optional_value(XvalueT &value_out, const XvalueT &default_value);
    template<typename XvalueT>
      Argument_sentry & do_get_required_value(XvalueT &value_out);

  public:
    const rocket::cow_string & get_name() const noexcept
      {
        return this->m_name;
      }

    bool does_throw_on_failure() const noexcept
      {
        return this->m_throw_on_failure;
      }
    void set_throw_on_failure(bool throw_on_failure = true) noexcept
      {
        this->m_throw_on_failure = throw_on_failure;
      }
    const State & get_state() const noexcept
      {
        return this->m_state;
      }
    void set_state(const State &state) noexcept
      {
        this->m_state = state;
      }

    bool succeeded() const noexcept
      {
        return this->m_state.succeeded;
      }
    void reset(const rocket::cow_vector<Reference> &args) noexcept
      {
        this->m_state.args = &args;
        this->m_state.offset = 0;
        this->m_state.succeeded = true;
        this->m_state.finished = false;
      }

    // Sentry objects are allowed in `if` and `while` conditions, just like `std::istream`s.
    explicit operator bool () const noexcept
      {
        return this->succeeded();
      }

    // Get an OPTIONAL argument.
    // The argument must exist (and must be of the desired type or `null` for the overloads taking two parameters); otherwise this operation fails.
    // N.B. These functions provide STRONG exception safety guarantee.
    Argument_sentry & opt(Reference &ref_out);
    Argument_sentry & opt(Value &value_out);
    Argument_sentry & opt(D_boolean &value_out, D_boolean default_value = false);
    Argument_sentry & opt(D_integer &value_out, D_integer default_value = 0);
    Argument_sentry & opt(D_real &value_out, D_real default_value = 0);
    Argument_sentry & opt(D_string &value_out, const D_string &default_value = std::ref(""));
    Argument_sentry & opt(D_opaque &value_out, const D_opaque &default_value);  // no default value
    Argument_sentry & opt(D_function &value_out, const D_function &default_value);  // no default value
    Argument_sentry & opt(D_array &value_out, const D_array &default_value = D_array());
    Argument_sentry & opt(D_object &value_out, const D_object &default_value = D_object());

    // Get a REQUIRED argument.
    // The argument must exist and must be of the desired type; otherwise this operation fails.
    // N.B. These functions provide STRONG exception safety guarantee.
    Argument_sentry & req(D_null &value_out);
    Argument_sentry & req(D_boolean &value_out);
    Argument_sentry & req(D_integer &value_out);
    Argument_sentry & req(D_real &value_out);
    Argument_sentry & req(D_string &value_out);
    Argument_sentry & req(D_opaque &value_out);
    Argument_sentry & req(D_function &value_out);
    Argument_sentry & req(D_array &value_out);
    Argument_sentry & req(D_object &value_out);

    // Terminate the argument list.
    // There will be no more argument; otherwise this operation fails.
    // N.B. This function provides STRONG exception safety guarantee.
    Argument_sentry & cut();

    // Throw an exception saying there are no viable overloads.
    // The overload list parameter is informative.
    [[noreturn]] void throw_no_matching_function_call(std::initializer_list<const char *> overload_list) const;
  };

}

#endif