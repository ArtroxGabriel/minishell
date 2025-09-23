/*
 * mylib.h - Example public header for os-c-template
 *
 * This file provides a small, documented, and portable C API that can be
 * used as a starting point for assignments in the Systems Operating course.
 *
 * Design goals:
 *  - Simple, self-contained API (C11 compatible)
 *  - Clear ownership rules (who frees what)
 *  - Thread-safety notes where relevant
 *  - Error enumeration and human-readable error strings
 *
 * Replace and extend this header to match your project's real public API.
 */

#ifndef MYLIB_H
#define MYLIB_H

/* Public API versioning */
#define MYLIB_VERSION_MAJOR 0
#define MYLIB_VERSION_MINOR 1
#define MYLIB_VERSION_PATCH 0

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h> /* for bool */
#include <stddef.h>  /* for size_t */
#include <stdint.h>  /* for fixed-width integer types */

/*
 * Visibility macro (helps when building shared libraries).
 * Consumers may define MYLIB_API_EXPORTS when building the library.
 */
#if defined(_WIN32) || defined(_WIN64)
#if defined(MYLIB_API_EXPORTS)
#define MYLIB_API __declspec(dllexport)
#else
#define MYLIB_API __declspec(dllimport)
#endif
#else
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define MYLIB_API __attribute__((visibility("default")))
#else
#define MYLIB_API
#endif
#endif

    /**
     * mylib_error_t - Error codes returned by library functions
     *
     * All public functions that can fail will return an integer error code where
     * `MYLIB_OK` indicates success. Additional codes indicate different failure
     * modes. Callers may use `mylib_error_string()` to get a human-readable
     * message.
     */
    typedef enum
    {
        MYLIB_OK                  = 0,
        MYLIB_ERR_INVALID_ARG     = 1,
        MYLIB_ERR_OOM             = 2,
        MYLIB_ERR_NOT_INITIALIZED = 3,
        MYLIB_ERR_INTERNAL        = 4,
    } mylib_error_t;

    /**
     * mylib_init() - Initialize global library state.
     *
     * Many libraries require an explicit initialization step. This function is a
     * placeholder for that. It's safe to call multiple times; subsequent calls
     * are no-ops and return MYLIB_OK.
     *
     * Thread-safety: calling concurrently from multiple threads is safe; the
     * implementation should perform its own synchronization or use an atomic flag.
     *
     * Returns:
     *  - MYLIB_OK on success
     *  - MYLIB_ERR_OOM if allocation fails
     *  - MYLIB_ERR_INTERNAL on unexpected errors
     */
    MYLIB_API mylib_error_t mylib_init(void);

    /**
     * mylib_shutdown() - Release global library state.
     *
     * After calling this function, functions that require initialization may
     * return MYLIB_ERR_NOT_INITIALIZED. It's undefined to call other library
     * functions after shutdown unless you re-initialize.
     *
     * Thread-safety: when possible, call in a single-threaded shutdown path.
     */
    MYLIB_API void mylib_shutdown(void);

    /**
     * mylib_version_string() - Get a static string describing library version.
     *
     * The returned pointer is to a statically allocated, null-terminated string
     * owned by the library and must not be freed.
     */
    MYLIB_API const char* mylib_version_string(void);

    /**
     * mylib_add() - Example pure function that adds two integers.
     *
     * This demonstrates a simple API that cannot fail.
     */
    MYLIB_API int mylib_add(int a, int b);

    /**
     * mylib_subtract_checked() - Subtract `b` from `a` with overflow detection.
     *
     * On success sets *result and returns MYLIB_OK. If the result would overflow
     * the signed 32-bit range, the function returns MYLIB_ERR_INTERNAL and
     * does not modify *result.
     *
     * Parameters:
     *  - a, b: operands
     *  - result: out parameter, must be non-NULL
     */
    MYLIB_API mylib_error_t mylib_subtract_checked(int32_t a, int32_t b, int32_t* result);

    /**
     * mylib_dup_string() - Duplicate a NUL-terminated string using library
     * allocator.
     *
     * The returned string must be freed by the caller using `mylib_free_string()`.
     *
     * Returns:
     *  - pointer to allocated string on success
     *  - NULL if `s` is NULL or on allocation failure (check error via errno or
     *    by design using mylib_error_t in your real API)
     */
    MYLIB_API char* mylib_dup_string(const char* s);

    /**
     * mylib_free_string() - Free a string allocated by the library.
     *
     * This function should be used to free strings returned by `mylib_dup_string`.
     * It is a thin wrapper over the library allocator, which allows the library
     * to manage its allocation method across platforms.
     *
     * Passing NULL is allowed and is a no-op.
     */
    MYLIB_API void mylib_free_string(char* s);

    /**
     * mylib_set_option() / mylib_get_option() - Simple key/value option API.
     *
     * These functions are a tiny example for controlling runtime behavior of the
     * library. Keys are case-sensitive, NUL-terminated strings. Values follow the
     * same ownership rules as strings returned by `mylib_dup_string` (i.e. caller
     * provides owned strings).
     *
     * Both functions are safe to call after initialization. Implementations may
     * choose to copy keys/values or store pointers depending on design.
     *
     * Returns MYLIB_OK on success or an appropriate mylib_error_t on failure.
     */
    MYLIB_API mylib_error_t mylib_set_option(const char* key, const char* value);
    MYLIB_API mylib_error_t mylib_get_option(const char* key, char** out_value);

    /**
     * mylib_error_string() - Convert an error code to a human-readable string.
     *
     * The returned pointer is statically owned and must not be freed. It is valid
     * for the lifetime of the program.
     */
    MYLIB_API const char* mylib_error_string(mylib_error_t err);

/* --- Convenience inline helpers --- */

/**
 * MYLIB_MAKE_VERSION - Macro to combine version components into a single int.
 *
 * Example: MYLIB_MAKE_VERSION(1,2,3)
 */
#define MYLIB_MAKE_VERSION(major, minor, patch) (((major) << 16) | ((minor) << 8) | (patch))

    /**
     * mylib_version() - Get packed integer version.
     *
     * Example check:
     *   if (mylib_version() >= MYLIB_MAKE_VERSION(0,1,0)) { ... }
     */
    static inline uint32_t mylib_version(void)
    {
        return (uint32_t)MYLIB_MAKE_VERSION(MYLIB_VERSION_MAJOR, MYLIB_VERSION_MINOR,
                                            MYLIB_VERSION_PATCH);
    }

/* --- Usage example (for documentation purposes) ---
 *
 * #include "mylib.h"
 *
 * int main(void) {
 *     if (mylib_init() != MYLIB_OK) return 1;
 *     const char *v = mylib_version_string();
 *     printf(\"mylib %s\\n\", v);
 *     char *s = mylib_dup_string(\"hello\");
 *     if (s) {
 *         puts(s);
 *         mylib_free_string(s);
 *     }
 *     mylib_shutdown();
 *     return 0;
 * }
 */

/* End of public header */
#ifdef __cplusplus
}
#endif

#endif /* MYLIB_H */
