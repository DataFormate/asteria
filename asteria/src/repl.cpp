// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#include "precompiled.hpp"
#include "utilities.hpp"
#include "runtime/reference.hpp"
#include "runtime/variable.hpp"
#include "runtime/global_context.hpp"
#include "runtime/simple_script.hpp"
#include "runtime/runtime_error.hpp"
#include "runtime/abstract_hooks.hpp"
#include "compiler/parser_error.hpp"
#include <locale.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace Asteria;

namespace {

enum Exit_Code : uint8_t
  {
    exit_success            = 0,
    exit_unspecified        = 1,
    exit_invalid_argument   = 2,
    exit_parser_error       = 3,
    exit_runtime_error      = 4,
  };

[[noreturn]] int do_exit(Exit_Code code) noexcept
  {
    ::fflush(nullptr);
    ::quick_exit(static_cast<int>(code));
  }

cow_string do_xindent(cow_string&& str)
  {
    size_t bp = SIZE_MAX;
    while((bp = str.find('\n', ++bp)) != cow_string::npos)
      str.insert(++bp, 1, '\t');
    return ::std::move(str);
  }

cow_string do_stringify(const Value& val) noexcept
  try {
    ::rocket::tinyfmt_str fmt;
    fmt << val;
    return do_xindent(fmt.extract_string());
  }
  catch(exception& stdex) {
    return ::rocket::sref("<bad value>");
  }

cow_string do_stringify(const Reference& ref) noexcept
  try {
    if(ref.is_void())
      return ::rocket::sref("<void>");
    if(ref.is_tail_call())
      return ::rocket::sref("<tail call>");
    ::rocket::tinyfmt_str fmt;
    // Print the value category.
    if(auto var = ref.get_variable_opt())
      if(var->is_immutable())
        fmt << "immutable variable ";
      else
        fmt << "variable ";
    else
      if(ref.is_constant())
        fmt << "constant ";
      else if(ref.is_temporary())
        fmt << "temporary ";
      else
        ROCKET_ASSERT(false);
    // Print the value.
    fmt << ref.read();
    return do_xindent(fmt.extract_string());
  }
  catch(exception& stdex) {
    return ::rocket::sref("<bad reference>");
  }

cow_string do_stringify(const exception& stdex) noexcept
  try {
    ::rocket::tinyfmt_str fmt;
    // Write the exception message verbatim.
    fmt << stdex.what();
    // Append the dynamic type of the exception object that has been caught.
    ::rocket::format(fmt, "\n[exception class `$1`]",
                          typeid(stdex).name());
    return do_xindent(fmt.extract_string());
  }
  catch(exception& other) {
    return ::rocket::sref("<bad exception>");
  }

cow_string do_stringify(const Parser_Error& except) noexcept
  try {
    ::rocket::tinyfmt_str fmt;
    // Write the description of this error.
    ::rocket::format(fmt, "ERROR $1: $2",
                          except.status(),
                          describe_parser_status(except.status()));
    // Append the source location of the error.
    if(except.line() >= 0)
      ::rocket::format(fmt, "\n[unexpected token at line $1, offset $2, length $3]",
                            except.line(),
                            except.offset(),
                            except.length());
    else
      fmt << "\n[end of input encountered]";
    // Append the dynamic type of the exception object that has been caught.
    ::rocket::format(fmt, "\n[exception class `$1`]",
                          typeid(except).name());
    return do_xindent(fmt.extract_string());
  }
  catch(exception& other) {
    return ::rocket::sref("<bad exception>");
  }

cow_string do_stringify(const Runtime_Error& except) noexcept
  try {
    ::rocket::tinyfmt_str fmt;
    // If the exception value is a string, write it verbatim.
    // Otherwise print it like `std.debug.dump()`.
    const auto& val = except.value();
    if(val.is_string())
      fmt << val.as_string();
    else
      fmt << val;
    // Append backtrace frames.
    size_t nframes = except.count_frames();
    if(nframes != 0) {
      fmt << "\n[backtrace:";
      for(size_t i = 0;  i < except.count_frames();  ++i)
        ::rocket::format(fmt, "\n  #$1 <$2> at '$3': $4",
                              i,
                              except.frame(i).what_type(),
                              except.frame(i).sloc(),
                              except.frame(i).value());
      fmt << "\n  -- end of backtrace]";
    }
    else
      fmt << "\n[no backtrace available]";
    // Append the dynamic type of the exception object that has been caught.
    ::rocket::format(fmt, "\n[exception class `$1`]",
                          typeid(except).name());
    return do_xindent(fmt.extract_string());
  }
  catch(exception& other) {
    return ::rocket::sref("<bad exception>");
  }

// Define command-line options here.
struct Command_Line_Options
  {
    // options
    uint8_t optimize = 0;
    bool verbose = false;
    bool interactive = false;
    // non-options
    cow_string path;
    cow_vector<Value> args;
  };

// We want to detect Ctrl-C.
volatile ::sig_atomic_t interrupted;

void do_trap_sigint()
  {
    // Trap Ctrl-C. Failure to set the signal handler is ignored.
    struct ::sigaction sigx = { };
    sigx.sa_handler = [](int) { interrupted = 1;  };
    ::sigaction(SIGINT, &sigx, nullptr);
  }

// These may also be automatic objects. They are declared here for convenience.
Command_Line_Options cmdline;
Compiler_Options options;
Simple_Script script;
Global_Context global;

unsigned long index;  // snippet index
cow_string code;  // snippet
cow_string heredoc;

// These hooks help debugging
struct REPL_Hooks : Abstract_Hooks
  {
    void on_variable_declare(const Source_Location& sloc, const cow_string& inside,
                             const phsh_string& name) noexcept override
      {
        if(ROCKET_EXPECT(!cmdline.verbose)) {
          return;
        }
        ::fprintf(stderr, "~ running: ['%s:%ld' inside `%s`] declaring variable: %s\n",
                          sloc.c_file(), sloc.line(), inside.c_str(),
                          name.c_str());
      }
    void on_function_call(const Source_Location& sloc, const cow_string& inside,
                          const cow_function& target) noexcept override
      {
        if(ROCKET_EXPECT(!cmdline.verbose)) {
          return;
        }
        ::fprintf(stderr, "~ running: ['%s:%ld' inside `%s`] initiating function call: %s\n",
                          sloc.c_file(), sloc.line(), inside.c_str(),
                          do_stringify(target).c_str());
      }
    void on_function_return(const Source_Location& sloc, const cow_string& inside,
                            const Reference& result) noexcept override
      {
        if(ROCKET_EXPECT(!cmdline.verbose)) {
          return;
        }
        ::fprintf(stderr, "~ running: ['%s:%ld' inside `%s`] returned from function call: %s\n",
                          sloc.c_file(), sloc.line(), inside.c_str(),
                          do_stringify(result).c_str());
      }
    void on_function_except(const Source_Location& sloc, const cow_string& inside,
                            const Runtime_Error& except) noexcept override
      {
        if(ROCKET_EXPECT(!cmdline.verbose)) {
          return;
        }
        ::fprintf(stderr, "~ running: ['%s:%ld' inside `%s`] caught exception from function call: %s\n",
                          sloc.c_file(), sloc.line(), inside.c_str(),
                          do_stringify(except).c_str());
      }
    void on_single_step_trap(const Source_Location& sloc, const cow_string& inside,
                             Executive_Context* /*ctx*/) override
      {
        if(ROCKET_EXPECT(!interrupted)) {
          return;
        }
        ASTERIA_THROW("interrupt received\n[received at '$1' inside `$2`]", sloc, inside);
      }
  };

void do_print_help(const char* self)
  {
    ::printf(
      //        1         2         3         4         5         6         7      |
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      "Usage: %s [OPTIONS] [[--] FILE [ARGUMENTS]...]\n"
      "\n"
      "  -h      show help message then exit\n"
      "  -I      suppress interactive mode [default = auto]\n"
      "  -i      force interactive mode [default = auto]\n"
      "  -O      equivalent to `-O1`\n"
      "  -O[nn]  set optimization level to `nn` [default = 2]\n"
      "  -V      show version information then exit\n"
      "  -v      print execution details to standard error\n"
      "\n"
      "Source code is read from standard input if no FILE is specified or `-` is\n"
      "given as FILE, and from FILE otherwise. ARGUMENTS following FILE are passed\n"
      "to the script as strings verbatim, which can be retrieved via `__varg`.\n"
      "\n"
      "If neither `-I` or `-i` is set, interactive mode is enabled when no FILE is\n"
      "specified and standard input is connected to a terminal, and is disabled\n"
      "otherwise. Be advised that specifying `-` explicitly disables interactive\n"
      "mode.\n"
      "\n"
      "When running in non-interactive mode, characters are read from FILE, then\n"
      "compiled and executed. If the script returns an `integer`, it is truncated\n"
      "to 8 bits as an unsigned integer and the result denotes the exit status. If\n"
      "the script returns no value, the exit status is zero. If the script returns\n"
      "a value that is neither an `integer` nor `null`, or throws an exception, the\n"
      "exit status is non-zero.\n"
      "\n"
      "Visit the homepage at <%s>.\n"
      "Report bugs to <%s>.\n"
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      //        1         2         3         4         5         6         7      |
      ,
      self,
      PACKAGE_URL,
      PACKAGE_BUGREPORT
    );
  }

void do_print_version()
  {
    ::printf(
      //        1         2         3         4         5         6         7      |
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      "%s [built on %s]\n"
      "\n"
      "Visit the homepage at <%s>.\n"
      "Report bugs to <%s>.\n"
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      //        1         2         3         4         5         6         7      |
      ,
      PACKAGE_STRING, __DATE__,
      PACKAGE_URL,
      PACKAGE_BUGREPORT
    );
  }

void do_parse_command_line(int argc, char** argv)
  {
    bool help = false;
    bool version = false;

    opt<long> optimize;
    opt<bool> verbose;
    opt<bool> interactive;
    opt<cow_string> path;
    cow_vector<Value> args;

    if(argc > 1) {
      // Rewrite some common options before calling `getopt()`.
      if(::strcmp(argv[1], "--help") == 0)
        ::strcpy(argv[1], "-h");
      else if(::strcmp(argv[1], "--version") == 0)
        ::strcpy(argv[1], "-V");
    }
    for(;;) {
      int ch = ::getopt(argc, argv, "+hIiO::Vv");
      if(ch == -1) {
        // There are no more arguments.
        break;
      }
      switch(ch) {
      case 'h': {
          help = true;
          break;
        }
      case 'I': {
          interactive = false;
          break;
        }
      case 'i': {
          interactive = true;
          break;
        }
      case 'O': {
          // If `-O` is specified without an argument, it is equivalent to `-O1`.
          if(!optarg || !*optarg) {
            optimize = 1;
            break;
          }
          // Parse the level.
          char* ep;
          long val = ::strtol(optarg, &ep, 10);
          if((*ep != 0) || (val < 0) || (val > 99)) {
            ::fprintf(stderr, "%s: invalid optimization level -- '%s'\n", argv[0], optarg);
            do_exit(exit_invalid_argument);
          }
          optimize = val;
          break;
        }
      case 'V': {
          version = true;
          break;
        }
      case 'v': {
          verbose = true;
          break;
        }
      default:
        // `getopt()` will have written an error message to standard error.
        ::fprintf(stderr, "Try `%s -h` for help.\n", argv[0]);
        do_exit(exit_invalid_argument);
      }
    }
    // Check for early exit conditions.
    if(help) {
      do_print_help(argv[0]);
      do_exit(exit_success);
    }
    if(version) {
      do_print_version();
      do_exit(exit_success);
    }
    // If more arguments follow, they denote the script to execute.
    if(optind < argc) {
      // The first non-option argument is the filename to execute. `-` is not special.
      path = V_string(argv[optind]);
      // All subsequent arguments are passed to the script verbatim.
      ::std::for_each(argv + optind + 1, argv + argc,
                      [&](const char* arg) { args.emplace_back(V_string(arg));  });
    }

    // The default optimization level is `2`.
    // Note again that `-O` without an argument is equivalent to `-O1`, which effectively decreases
    // optimization in comparison to when it wasn't specified.
    cmdline.optimize = static_cast<uint8_t>(::rocket::clamp(optimize.value_or(2), 0, UINT8_MAX));
    cmdline.verbose = verbose.value_or(false);
    // Interactive mode is enabled when no FILE is given (not even `-`) and standard input is
    // connected to a terminal.
    cmdline.interactive = interactive ? *interactive : (!path && ::isatty(STDIN_FILENO));
    cmdline.path = path.move_value_or(::rocket::sref("-"));
    cmdline.args = ::std::move(args);
  }

void do_REPL_help()
  {
    ::fprintf(stderr,
      //        1         2         3         4         5         6         7      |
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      "* commands:\n"
      "  :help    show this message\n"
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      //        1         2         3         4         5         6         7      |
    );
  }

void do_handle_REPL_command(cow_string&& cmd)
  {
    // TODO tokenize
    if(cmd == "help") {
      return do_REPL_help();
    }
    ::fprintf(stderr, "! unknown command: %s\n", cmd.c_str());
  }

int do_REP_single()
  {
    // Reset standard streams.
    if(!::freopen(nullptr, "r", stdin)) {
      ::fprintf(stderr, "! could not reopen standard input (errno was `%d`)\n", errno);
      ::abort();
    }
    if(!::freopen(nullptr, "w", stdout)) {
      ::fprintf(stderr, "! could not reopen standard output (errno was `%d`)\n", errno);
      ::abort();
    }
    // Read the next snippet.
    ::fputc('\n', stderr);
    code.clear();
    interrupted = 0;

    // Prompt for the first line.
    bool escape = false;
    long line = 0;
    int indent;
    ::fprintf(stderr, "#%lu:%lu%n> ", ++index, ++line, &indent);

    for(;;) {
      // Read a character. Break upon read errors.
      int ch = ::fgetc(stdin);
      if(ch == EOF) {
        // Force-move the caret to the next line.
        ::fputc('\n', stderr);
        break;
      }
      if(ch == '\n') {
        // Check for termination.
        if(heredoc.empty()) {
          // In line input mode, the current snippet is terminated by an unescaped line feed.
          if(!escape)
            break;
          // REPL commands can't straddle multiple lines.
          if(!code.empty() && (code.front() == ':'))
            break;
        }
        else {
          // In heredoc mode, the current snippet is terminated by a line consisting of the
          // user-defined terminator, which is not part of the snippet and must be removed.
          if(code.ends_with(heredoc)) {
            code.erase(code.size() - heredoc.size());
            heredoc.clear();
            break;
          }
        }
        // The line feed should be preserved. It'll be appended later.
        // Prompt for the next consecutive line.
        ::fprintf(stderr, "%*lu> ", indent, ++line);
      }
      else if(heredoc.empty()) {
        // In line input mode, backslashes that precede line feeds are deleted. Those that
        // do not precede line feeds are kept as is.
        if(escape) {
          code.push_back('\\');
        }
        if(ch == '\\') {
          escape = true;
          continue;
        }
      }
      // Append the character.
      code.push_back(static_cast<char>(ch));
      escape = false;
    }
    if(interrupted) {
      // Discard this snippet and read the next one.
      ::fprintf(stderr, "! interrupted\n");
      return SIGINT;
    }
    if(::ferror(stdin)) {
      // Discard the last (partial) snippet in case of read errors.
      ::fprintf(stderr, "! error reading standard input\n");
      do_exit(exit_unspecified);
    }
    if(::feof(stdin) && code.empty()) {
      // Exit normally.
      ::fprintf(stderr, "* have a nice day :)\n");
      do_exit(exit_success);
    }
    if(code.empty()) {
      // Do nothing.
      return 0;
    }
    if(code.front() == ':') {
      // Erase the initiator and process the remaining.
      code.erase(0, 1);
      do_handle_REPL_command(::std::move(code));
      return 0;
    }

    // Name the snippet.
    char name[32];
    size_t nlen = (unsigned)::std::sprintf(name, "snippet #%lu", index);
    cmdline.path.assign(name, nlen);

    // The snippet might be a statement list or an expression.
    // First, try parsing it as the former.
    script.set_options(options);
    try {
      script.reload_string(code, cmdline.path);
    }
    catch(Parser_Error& except) {
      // We only want to make another attempt in the case of absence of a semicolon at the end.
      bool retry = (except.status() == parser_status_semicolon_expected) && (except.line() < 0);
      if(retry) {
        // Rewrite the potential expression to a `return` statement.
        code.insert(0, "return& ( ");
        code.append(" );");
        // Try parsing it again.
        try {
          script.reload_string(code, cmdline.path);
        }
        catch(Parser_Error& /*other*/) {
          // If we fail again, it is the previous exception that we are interested in.
          retry = false;
        }
      }
      if(!retry) {
        // Bail out upon irrecoverable errors.
        ::fprintf(stderr, "! parser error: %s\n", do_stringify(except).c_str());
        return SIGPIPE;
      }
    }

    // Execute the script as a function, which returns a `Reference`.
    try {
      const auto ref = script.execute(global, ::std::move(cmdline.args));
      if(!ref.is_void()) {
        // Ensure it is dereferenceable.
        static_cast<void>(ref.read());
      }
      // Print the result.
      ::fprintf(stderr, "* result #%lu: %s\n", index, do_stringify(ref).c_str());
    }
    catch(Runtime_Error& except) {
      // If an exception was thrown, print something informative.
      ::fprintf(stderr, "! runtime error: %s\n", do_stringify(except).c_str());
    }
    catch(exception& stdex) {
      // If an exception was thrown, print something informative.
      ::fprintf(stderr, "! unhandled exception: %s\n", do_stringify(stdex).c_str());
    }
    return 0;
  }

[[noreturn]] int do_REPL_noreturn()
  {
    // Write the title to standard error.
    ::fprintf(stderr,
      //        1         2         3         4         5         6         7      |
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      "%s [built on %s]\n"
      "\n"
      "  Global locale is now `%s`.\n"
      "\n"
      "  All REPL commands start with a `:`. Type `:help` for instructions.\n"
      "  Multiple lines may be joined together using trailing backslashes.\n"
      // 34567890123456789012345678901234567890123456789012345678901234567890123456|
      //        1         2         3         4         5         6         7      |
      ,
      PACKAGE_STRING, __DATE__,
      ::setlocale(LC_ALL, nullptr)
    );

    // Trap Ctrl-C. Failure to set the signal handler is ignored.
    // This also makes I/O functions fail immediately, instead of attempting to try again.
    do_trap_sigint();

    // Set up runtime hooks. This is sticky.
    global.set_hooks(::rocket::make_refcnt<REPL_Hooks>());

    // In interactive mode (a.k.a. REPL mode), read user inputs in lines.
    // Outputs from the script go into standard output. Others go into standard error.
    for(;;)
      do_REP_single();
  }

[[noreturn]] int do_single_noreturn()
  {
    // Return this if an exception is thrown.
    Exit_Code status = exit_runtime_error;

    // Consume all data from standard input.
    script.set_options(options);
    try {
      if(cmdline.path == "-")
        script.reload_stdin();
      else
        script.reload_file(cmdline.path);
    }
    catch(Parser_Error& except) {
      // Report the error and exit.
      ::fprintf(stderr, "! parser error: %s\n", do_stringify(except).c_str());
      do_exit(exit_parser_error);
    }

    // Execute the script.
    try {
      const auto ref = script.execute(global, ::std::move(cmdline.args));
      if(ref.is_void()) {
        // If the script returned no value, exit with zero.
        status = exit_success;
      }
      else {
        // If the script returned an `integer`, forward its lower 8 bits.
        // Any other value indicates failure.
        const auto& val = ref.read();
        if(val.is_integer())
          status = static_cast<Exit_Code>(val.as_integer() & 0xFF);
        else
          status = exit_unspecified;
      }
    }
    catch(Runtime_Error& except) {
      // If an exception was thrown, print something informative.
      ::fprintf(stderr, "! runtime error: %s\n", do_stringify(except).c_str());
    }
    catch(exception& stdex) {
      // If an exception was thrown, print something informative.
      ::fprintf(stderr, "! unhandled exception: %s\n", do_stringify(stdex).c_str());
    }
    do_exit(status);
  }

}  // namespace

int main(int argc, char** argv)
  try {
    // Select the C locale. UTF-8 is required for wide-oriented standard streams.
    ::setlocale(LC_ALL, "C.UTF-8");

    // Note that this function shall not return in case of errors.
    do_parse_command_line(argc, argv);

    // Protect against stack overflows.
    global.set_recursion_base(&argc);

    // Call other functions which are declared `noreturn`. `main()` itself is not `noreturn` so we
    // don't get stupid warngings like 'function declared `noreturn` has a `return` statement'.
    if(cmdline.interactive)
      do_REPL_noreturn();
    else
      do_single_noreturn();
  }
  catch(exception& stdex) {
    // Print a message followed by the backtrace if it is available. There isn't much we can do.
    ::fprintf(stderr, "! unhandled exception: %s\n", do_stringify(stdex).c_str());
    do_exit(exit_unspecified);
  }
