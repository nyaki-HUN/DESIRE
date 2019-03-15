#pragma once

// --------------------------------------------------------------------------------------------------------------------
//	This is a modified version of sqrat
//	The changes include some cleanup, switching to C++11 and removing features
// --------------------------------------------------------------------------------------------------------------------

//
// SqratUtil: Squirrel Utilities
//

//
// Copyright (c) 2009 Brandon Jones
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the original software. If you use this software
//  in a product, an acknowledgment in the product documentation would be
//  appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source
//  distribution.
//

#include <cassert>
#include <squirrel.h>
#include <string.h>

#include <unordered_map>

namespace Sqrat {

/// @cond DEV

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Define an inline function to avoid MSVC's "conditional expression is constant" warning
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
    template <typename T>
    inline T _c_def(T value) { return value; }
    #define SQRAT_CONST_CONDITION(value) _c_def(value)
#else
    #define SQRAT_CONST_CONDITION(value) value
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define helpers to create portable import / export macros
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(SCRAT_EXPORT)
    #if defined(_WIN32)
        // Windows compilers need a specific keyword for export
        #define SQRAT_API __declspec(dllexport)
    #else
        #if __GNUC__ >= 4
            // GCC 4 has special keywords for showing/hiding symbols,
            // the same keyword is used for both importing and exporting
            #define SQRAT_API __attribute__ ((__visibility__ ("default")))
        #else
            // GCC < 4 has no mechanism to explicitly hide symbols, everything's exported
            #define SQRAT_API

        #endif
    #endif
#elif defined(SCRAT_IMPORT)
    #if defined(_WIN32)
        // Windows compilers need a specific keyword for import
        #define SQRAT_API __declspec(dllimport)
    #else
        #if __GNUC__ >= 4
            // GCC 4 has special keywords for showing/hiding symbols,
            // the same keyword is used for both importing and exporting
            #define SQRAT_API __attribute__ ((__visibility__ ("default")))
        #else
            // GCC < 4 has no mechanism to explicitly hide symbols, everything's exported
            #define SQRAT_API
        #endif
    #endif
#else
    #define SQRAT_API
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Define macros for internal error handling
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined (SCRAT_NO_ERROR_CHECKING)
    #define SQCATCH(vm)          if (SQRAT_CONST_CONDITION(false))
    #define SQCATCH_NOEXCEPT(vm) if (SQRAT_CONST_CONDITION(false))
    #define SQCLEAR(vm)
    #define SQRETHROW(vm)
    #define SQTHROW(vm, err)
    #define SQTRY()
    #define SQWHAT(vm)           _SC("")
    #define SQWHAT_NOEXCEPT(vm)  _SC("")
#elif defined (SCRAT_USE_EXCEPTIONS)
    #define SQCATCH(vm)          } catch (const Sqrat::Exception& e)
    #define SQCATCH_NOEXCEPT(vm) if (SQRAT_CONST_CONDITION(false))
    #define SQCLEAR(vm)
    #ifdef _MSC_VER // avoid MSVC's "unreachable code" warning
        #define SQRETHROW(vm)      if (SQRAT_CONST_CONDITION(true)) throw
        #define SQTHROW(vm, err)   if (SQRAT_CONST_CONDITION(true)) throw Sqrat::Exception(err)
    #else
        #define SQRETHROW(vm)      throw
        #define SQTHROW(vm, err)   throw Sqrat::Exception(err)
    #endif
    #define SQTRY()              try {
    #define SQWHAT(vm)           e.Message().c_str()
    #define SQWHAT_NOEXCEPT(vm)  _SC("")
#else
    #define SQCATCH(vm)          if (SQRAT_CONST_CONDITION(false))
    #define SQCATCH_NOEXCEPT(vm) if (Error::Occurred(vm))
    #define SQCLEAR(vm)          Error::Clear(vm)
    #define SQRETHROW(vm)
    #define SQTHROW(vm, err)     Error::Throw(vm, err)
    #define SQTRY()
    #define SQWHAT(vm)           _SC("")
    #define SQWHAT_NOEXCEPT(vm)  Error::Message(vm).c_str()
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Removes unused variable warnings in a way that Doxygen can understand
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
void SQUNUSED(const T&) {
}

/// @endcond

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Defines a string that is definitely compatible with the version of Squirrel being used (normally this is std::string)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::basic_string<SQChar> string;

/// @cond DEV
#ifdef SQUNICODE
/* from http://stackoverflow.com/questions/15333259/c-stdwstring-to-stdstring-quick-and-dirty-conversion-for-use-as-key-in,
   only works for ASCII chars */
/**
* Convert a std::string into a std::wstring
*/
static std::wstring string_to_wstring(const std::string& str)
{
    return std::wstring(str.begin(), str.end());
}

/**
* Convert a std::wstring into a std::string
*/
static std::string wstring_to_string(const std::wstring& wstr)
{
    return std::string(wstr.begin(), wstr.end());
}

#endif // SQUNICODE

/// @endcond

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Helper class that defines a VM that can be used as a fallback VM in case no other one is given to a piece of code
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultVM {
private:

    static HSQUIRRELVM& staticVm() {
        static HSQUIRRELVM vm;
        return vm;
    }

public:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Gets the default VM
    ///
    /// \return Default VM
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static HSQUIRRELVM Get() {
        return staticVm();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Sets the default VM to a given VM
    ///
    /// \param vm New default VM
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void Set(HSQUIRRELVM vm) {
        staticVm() = vm;
    }
};

#if !defined (SCRAT_NO_ERROR_CHECKING) && !defined (SCRAT_USE_EXCEPTIONS)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// The class that must be used to deal with errors that Sqrat has
///
/// \remarks
/// When documentation in Sqrat says, "This function MUST have its error handled if it occurred," that
/// means that after the function has been run, you must call Error::Occurred to see if the function
/// ran successfully. If the function did not run successfully, then you MUST either call Error::Clear
/// or Error::Message to clear the error buffer so new ones may occur and Sqrat does not get confused.
///
/// \remarks
/// Any error thrown inside of a bound C++ function will be thrown in the given Squirrel VM and
/// automatically handled.
///
/// \remarks
/// If compiling with SCRAT_USE_EXCEPTIONS defined, Sqrat will throw exceptions instead of using this
/// class to handle errors. This means that functions must be enclosed in try blocks that catch
/// Sqrat::Exception instead of checking for errors with Error::Occurred.
///
/// \remarks
/// If compiling with SCRAT_NO_ERROR_CHECKING defined, Sqrat will run significantly faster,
/// but it will no longer check for errors and the Error class itself will not be defined.
/// In this mode, a Squirrel script may crash the C++ application if errors occur in it.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Error {
public:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Clears the error associated with a given VM
    ///
    /// \param vm Target VM
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void Clear(HSQUIRRELVM vm) {
        sq_pushregistrytable(vm);
        sq_pushstring(vm, "__error", -1);
        sq_rawdeleteslot(vm, -2, false);
        sq_pop(vm, 1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Clears the error associated with a given VM and returns the associated error message
    ///
    /// \param vm Target VM
    ///
    /// \return String containing a nice error message
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static string Message(HSQUIRRELVM vm) {
        sq_pushregistrytable(vm);
        sq_pushstring(vm, "__error", -1);
        if (SQ_SUCCEEDED(sq_rawget(vm, -2))) {
            string** ud;
            sq_getuserdata(vm, -1, (SQUserPointer*)&ud, NULL);
            sq_pop(vm, 1);
            string err = **ud;
            sq_pushstring(vm, "__error", -1);
            sq_rawdeleteslot(vm, -2, false);
            sq_pop(vm, 1);
            return err;
        }
        sq_pushstring(vm, "__error", -1);
        sq_rawdeleteslot(vm, -2, false);
        sq_pop(vm, 1);
        return string(_SC("an unknown error has occurred"));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Returns whether a Sqrat error has occurred with a given VM
    ///
    /// \param vm Target VM
    ///
    /// \return True if an error has occurred, otherwise false
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool Occurred(HSQUIRRELVM vm) {
        sq_pushregistrytable(vm);
        sq_pushstring(vm, "__error", -1);
        if (SQ_SUCCEEDED(sq_rawget(vm, -2))) {
            sq_pop(vm, 2);
            return true;
        }
        sq_pop(vm, 1);
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Raises an error in a given VM with a given error message
    ///
    /// \param vm  Target VM
    /// \param err A nice error message
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void Throw(HSQUIRRELVM vm, const string& err) {
        sq_pushregistrytable(vm);
        sq_pushstring(vm, "__error", -1);
        if (SQ_FAILED(sq_rawget(vm, -2))) {
            sq_pushstring(vm, "__error", -1);
            string** ud = reinterpret_cast<string**>(sq_newuserdata(vm, sizeof(string*)));
            *ud = new string(err);
            sq_setreleasehook(vm, -1, &error_cleanup_hook);
            sq_rawset(vm, -3);
            sq_pop(vm, 1);
            return;
        }
        sq_pop(vm, 2);
    }

private:

    Error() {}

    static SQInteger error_cleanup_hook(SQUserPointer ptr, SQInteger size) {
        SQUNUSED(size);
        string** ud = reinterpret_cast<string**>(ptr);
        delete *ud;
        return 0;
    }
};
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Tells Sqrat whether Squirrel error handling should be used
///
/// \remarks
/// If true, if a runtime error occurs during the execution of a call, the VM will invoke its error handler.
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ErrorHandling {
private:

    static bool& errorHandling() {
        static bool eh = true;
        return eh;
    }

public:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Returns whether Squirrel error handling is enabled
    ///
    /// \return True if error handling is enabled, otherwise false
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsEnabled() {
        return errorHandling();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Enables or disables Squirrel error handling
    ///
    /// \param enable True to enable, false to disable
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static void Enable(bool enable) {
        errorHandling() = enable;
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sqrat exception class
///
/// \remarks
/// Used only when SCRAT_USE_EXCEPTIONS is defined (see Sqrat::Error)
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Exception {
public:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Constructs an exception
    ///
    /// \param msg A nice error message
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Exception(const string& msg) : message(msg) {}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Copy constructor
    ///
    /// \param ex Exception to copy
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Exception(const Exception& ex) : message(ex.message) {}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Returns a string identifying the exception
    ///
    /// \return A nice error message
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const string& Message() const {
        return message;
    }

private:

    string message;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Returns a string that has been formatted to give a nice type error message (for usage with Class::SquirrelFunc)
///
/// \param vm           VM the error occurred with
/// \param idx          Index on the stack of the argument that had a type error
/// \param expectedType The name of the type that the argument should have been
///
/// \return String containing a nice type error message
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline string FormatTypeError(HSQUIRRELVM vm, SQInteger idx, const string& expectedType) {
    string err = _SC("wrong type (") + expectedType + _SC(" expected");
#if (SQUIRREL_VERSION_NUMBER>= 200) && (SQUIRREL_VERSION_NUMBER < 300) // Squirrel 2.x
    err = err + _SC(")");
#else // Squirrel 3.x
    if (SQ_SUCCEEDED(sq_typeof(vm, idx))) {
        const SQChar* actualType;
        sq_tostring(vm, -1);
        sq_getstring(vm, -1, &actualType);
        sq_pop(vm, 2);
        err = err + _SC(", got ") + actualType + _SC(")");
    } else {
        err = err + _SC(", got unknown)");
    }
#endif
    return err;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Returns the last error that occurred with a Squirrel VM (not associated with Sqrat errors)
///
/// \param vm Target VM
///
/// \return String containing a nice error message
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline string LastErrorString(HSQUIRRELVM vm) {
    const SQChar* sqErr;
    sq_getlasterror(vm);
    if (sq_gettype(vm, -1) == OT_NULL) {
        sq_pop(vm, 1);
        return string();
    }
    sq_tostring(vm, -1);
    sq_getstring(vm, -1, &sqErr);
    sq_pop(vm, 2);
    return string(sqErr);
}


/// @endcond

}
