// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "argument_reader.hpp"
#include "../utilities.hpp"

namespace Asteria {

void Argument_Reader::do_record_parameter_required(Vtype vtype)
  {
    if(this->m_state.finished) {
      ASTERIA_THROW("argument reader finished and disposed");
    }
    // Record a parameter and increment the number of parameters in total.
    this->m_state.history << ", " << describe_vtype(vtype);
    this->m_state.nparams++;
  }

void Argument_Reader::do_record_parameter_optional(Vtype vtype)
  {
    if(this->m_state.finished) {
      ASTERIA_THROW("argument reader finished and disposed");
    }
    // Record a parameter and increment the number of parameters in total.
    this->m_state.history << ", [" << describe_vtype(vtype) << ']';
    this->m_state.nparams++;
  }

void Argument_Reader::do_record_parameter_generic()
  {
    if(this->m_state.finished) {
      ASTERIA_THROW("argument reader finished and disposed");
    }
    // Record a parameter and increment the number of parameters in total.
    this->m_state.history << ", <generic>";
    this->m_state.nparams++;
  }

void Argument_Reader::do_record_parameter_variadic()
  {
    if(this->m_state.finished) {
      ASTERIA_THROW("argument reader finished and disposed");
    }
    // Terminate the parameter list.
    this->m_state.history << ", ...";
  }

void Argument_Reader::do_record_parameter_finish()
  {
    if(this->m_state.finished) {
      ASTERIA_THROW("argument reader finished and disposed");
    }
    // Terminate this overload.
    this->m_state.history.push_back('\0');
    // Append it to the overload list as a single operation.
    this->m_ovlds.append(this->m_state.history);
  }

const Reference* Argument_Reader::do_peek_argument_opt() const
  {
    if(!this->m_state.succeeded) {
      return nullptr;
    }
    // Before calling this function, the parameter information must have been recorded.
    auto index = this->m_state.nparams - 1;
    // Return a pointer to the argument at `index`.
    return this->m_args->get_ptr(index);
  }

opt<size_t> Argument_Reader::do_check_finish_opt() const
  {
    if(!this->m_state.succeeded) {
      return nullopt;
    }
    // Before calling this function, the current overload must have been finished.
    auto index = this->m_state.nparams;
    // Return the beginning of variadic arguments.
    return ::rocket::min(index, this->m_args->size());
  }

Argument_Reader& Argument_Reader::I() noexcept
  {
    // Clear internal states.
    this->m_state.history.clear();
    this->m_state.nparams = 0;
    this->m_state.finished = false;
    this->m_state.succeeded = true;
    return *this;
  }

bool Argument_Reader::F(cow_vector<Reference>& vargs)
  {
    this->do_record_parameter_variadic();
    this->do_record_parameter_finish();
    // Get the number of named parameters.
    auto qvoff = this->do_check_finish_opt();
    if(!qvoff) {
      return false;
    }
    // Initialize the argument vector.
    vargs.clear();
    // Copy variadic arguments, if any.
    auto nargs = this->m_args->size();
    if(nargs > *qvoff) {
      ::rocket::ranged_for(*qvoff, nargs, [&](size_t i) { vargs.emplace_back(this->m_args->data()[i]);  });
    }
    return true;
  }

bool Argument_Reader::F(cow_vector<Value>& vargs)
  {
    this->do_record_parameter_variadic();
    this->do_record_parameter_finish();
    // Get the number of named parameters.
    auto qvoff = this->do_check_finish_opt();
    if(!qvoff) {
      return false;
    }
    // Initialize the argument vector.
    vargs.clear();
    // Copy variadic arguments, if any.
    auto nargs = this->m_args->size();
    if(nargs > *qvoff) {
      ::rocket::ranged_for(*qvoff, nargs, [&](size_t i) { vargs.emplace_back(this->m_args->data()[i].read());  });
    }
    return true;
  }

bool Argument_Reader::F()
  {
    this->do_record_parameter_finish();
    // Get the number of named parameters.
    auto qvoff = this->do_check_finish_opt();
    if(!qvoff) {
      return false;
    }
    // There shall be no more arguments than parameters.
    auto nargs = this->m_args->size();
    if(nargs > *qvoff) {
      this->m_state.succeeded = false;
      return false;
    }
    return true;
  }

Argument_Reader& Argument_Reader::v(Bval& xval)
  {
    this->do_record_parameter_required(vtype_boolean);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_boolean()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_boolean();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Ival& xval)
  {
    this->do_record_parameter_required(vtype_integer);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_integer()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_integer();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Rval& xval)
  {
    this->do_record_parameter_required(vtype_real);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_convertible_to_real()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.convert_to_real();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Sval& xval)
  {
    this->do_record_parameter_required(vtype_string);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_string()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_string();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Pval& xval)
  {
    this->do_record_parameter_required(vtype_opaque);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_opaque()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_opaque();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Fval& xval)
  {
    this->do_record_parameter_required(vtype_function);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_function()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_function();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Aval& xval)
  {
    this->do_record_parameter_required(vtype_array);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_array()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_array();
    return *this;
  }

Argument_Reader& Argument_Reader::v(Oval& xval)
  {
    this->do_record_parameter_required(vtype_object);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      this->m_state.succeeded = false;
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(!val.is_object()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xval = val.as_object();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Reference& ref)
  {
    this->do_record_parameter_generic();
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      ref = Reference_root::S_constant();
      return *this;
    }
    // Copy the reference as is.
    ref = *karg;
    return *this;
  }

Argument_Reader& Argument_Reader::o(Value& val)
  {
    this->do_record_parameter_generic();
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      val = nullptr;
      return *this;
    }
    // Copy the val as is.
    val = karg->read();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Bopt& xopt)
  {
    this->do_record_parameter_optional(vtype_boolean);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_boolean()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_boolean();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Iopt& xopt)
  {
    this->do_record_parameter_optional(vtype_integer);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_integer()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_integer();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Ropt& xopt)
  {
    this->do_record_parameter_optional(vtype_real);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_convertible_to_real()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.convert_to_real();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Sopt& xopt)
  {
    this->do_record_parameter_optional(vtype_string);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_string()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_string();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Popt& xopt)
  {
    this->do_record_parameter_optional(vtype_opaque);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_opaque()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_opaque();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Fopt& xopt)
  {
    this->do_record_parameter_optional(vtype_function);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_function()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_function();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Aopt& xopt)
  {
    this->do_record_parameter_optional(vtype_array);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_array()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_array();
    return *this;
  }

Argument_Reader& Argument_Reader::o(Oopt& xopt)
  {
    this->do_record_parameter_optional(vtype_object);
    // Get the next argument.
    auto karg = this->do_peek_argument_opt();
    if(!karg) {
      xopt.reset();
      return *this;
    }
    // Read a value from the argument.
    const auto& val = karg->read();
    if(val.is_null()) {
      // Accept a `null` argument.
      xopt.reset();
      return *this;
    }
    if(!val.is_object()) {
      // If the val doesn't have the desired type, fail.
      this->m_state.succeeded = false;
      return *this;
    }
    // Copy the value.
    xopt = val.as_object();
    return *this;
  }

void Argument_Reader::throw_no_matching_function_call() const
  {
    // Create a message containing all arguments.
    cow_string args;
    if(!this->m_args->empty()) {
      size_t k = 0;
      for(;;) {
        args << this->m_args.get()[k].read().what_vtype();
        // Seek to the next argument.
        if(++k == this->m_args->size())
          break;
        args << ", ";
      }
    }
    // Append the list of overloads.
    cow_string ovlds;
    if(!this->m_ovlds.empty()) {
      ovlds << "\n[list of overloads:\n  ";
      size_t k = 0;
      for(;;) {
        ovlds << '`' << this->m_name << '(';
        // Get the current parameter list.
        const char* s = this->m_ovlds.data() + k;
        size_t n = ::std::strlen(s);
        // If the parameter list is not empty, it always start with a ", ".
        if(n != 0) {
          ovlds.append(s + 2, n - 2);
          k += n;
        }
        ovlds << ')' << '`';
        // Seek to the next overload.
        if(++k == this->m_ovlds.size())
          break;
        ovlds << ",\n  ";
      }
      ovlds << "\n  -- end of list of overloads]";
    }
    // Throw the exception now.
    ASTERIA_THROW("no matching function call for `$1($2)`$3", this->m_name, args, ovlds);
  }

}  // namespace Asteria
