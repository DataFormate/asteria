// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "bindings_io.hpp"
#include "argument_reader.hpp"
#include "simple_binding_wrapper.hpp"
#include "../utilities.hpp"

namespace Asteria {

void create_bindings_io(G_object& result, API_Version /*version*/)
  {
    //===================================================================
    // `std.io.print()`
    //===================================================================
//    result.insert_or_assign(rocket::sref("print"),
//      G_function(make_simple_binding(
//        // Description
//        rocket::sref
//          (
//            "std.io.print(...)`\n"
//            "  * Prints all arguments to the standard error stream. A line break\n"
//            "    is appended to terminate the line.\n"
//            "  * Returns `true` if the operation succeeds, or `null` otherwise.\n"
//          ),
//        // Opaque parameter
//        G_null
//          (
//            nullptr
//          ),
//        // Definition
//        [](const Value& /*opaque*/, const Global_Context& /*global*/, Cow_Vector<Reference>&& args) -> Reference
//          {
//            Argument_Reader reader(rocket::sref("std.io.print"), args);
//            // Parse variadic arguments.
//            Cow_Vector<Value> values;
//            if(reader.start().finish(values)) {
//              // Call the binding function.
//              if(!std_io_print(values)) {
//                return Reference_Root::S_null();
//              }
//              Reference_Root::S_temporary xref = { true };
//              return rocket::move(xref);
//            }
//            // Fail.
//            reader.throw_no_matching_function_call();
//          }
//      )));
    //===================================================================
    // End of `std.io`
    //===================================================================
  }

}  // namespace Asteria
