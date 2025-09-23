/*
 * mylib.c - Example implementation for os-c-template
 *
 * Implements the functions declared in include/mylib.h.
 *
 * Design notes:
 *  - Minimal, portable C11 implementation without external dependencies.
 *  - Uses C11 atomics (stdatomic.h) for a simple lock-free init flag and a
 *    spinlock for protecting the options linked list.
 *  - No global heap-heavy structures; options are stored in a small linked list.
 *  - Intended as a starting point: replace, extend, or harden for production use.
 */

#include "mylib.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

/* ----------------------
 * Global state
 * ---------------------- */

/* Initialization flag (atomic boolean) */
static atomic_bool g_initialized = ATOMIC_VAR_INIT(false);

/*
 * Simple spinlock using atomic_flag for protecting the options list.
 * This is intentionally minimal and should be replaced by a proper mutex
 * (pthread_mutex_t on POSIX, SRWLOCK on Windows, etc.) in larger projects.
 */
static atomic_flag g_options_lock = ATOMIC_FLAG_INIT;

static void options_lock_acquire(void)
{
    while (atomic_flag_test_and_set_explicit(&g_options_lock, memory_order_acquire))
    {
        /* spin */
    }
}

static void options_lock_release(void)
{
    atomic_flag_clear_explicit(&g_options_lock, memory_order_release);
}

/* Options linked list node */
typedef struct option_node
{
    char*               key;
    char*               value;
    struct option_node* next;
} option_node_t;

/* Head of options list (owned by the library) */
static option_node_t* g_options_head = NULL;

/* ----------------------
 * Helper functions
 * ---------------------- */

static option_node_t* find_option_node(const char* key)
{
    option_node_t* it = g_options_head;
    while (it)
    {
        if (strcmp(it->key, key) == 0)
            return it;
        it = it->next;
    }
    return NULL;
}

static void free_options_list(void)
{
    option_node_t* it = g_options_head;
    while (it)
    {
        option_node_t* next = it->next;
        free(it->key);
        free(it->value);
        free(it);
        it = next;
    }
    g_options_head = NULL;
}

/* ----------------------
 * Public API
 * ---------------------- */

MYLIB_API mylib_error_t mylib_init(void)
{
    /* Fast-path: already initialized */
    bool expected = false;
    if (atomic_compare_exchange_strong(&g_initialized, &expected, true))
    {
        /* We transitioned from false -> true and are responsible for init */
        /* Initialize option list and any other global state here */
        g_options_head = NULL;
        atomic_flag_clear(&g_options_lock); /* ensure unlocked */
        return MYLIB_OK;
    }
    else
    {
        /* already initialized; this is a no-op */
        return MYLIB_OK;
    }
}

MYLIB_API void mylib_shutdown(void)
{
    bool was_init = atomic_exchange(&g_initialized, false);
    if (!was_init)
    {
        return; /* nothing to do */
    }

    /* Clean up options list under lock */
    options_lock_acquire();
    free_options_list();
    options_lock_release();
}

/* Version string */
MYLIB_API const char* mylib_version_string(void)
{
    return "mylib 0.1.0";
}

/* Simple addition */
MYLIB_API int mylib_add(int a, int b)
{
    return a + b;
}

/* Subtract with overflow detection for 32-bit signed ints */
MYLIB_API mylib_error_t mylib_subtract_checked(int32_t a, int32_t b, int32_t* result)
{
    if (!result)
        return MYLIB_ERR_INVALID_ARG;

    /* Detect overflow by doing the operation in 64-bit */
    int64_t r = (int64_t)a - (int64_t)b;
    if (r < INT32_MIN || r > INT32_MAX)
    {
        return MYLIB_ERR_INTERNAL; /* overflow */
    }
    *result = (int32_t)r;
    return MYLIB_OK;
}

/* Duplicate string using library allocator */
MYLIB_API char* mylib_dup_string(const char* s)
{
    if (!s)
        return NULL;
    size_t len  = strlen(s);
    char*  copy = (char*)malloc(len + 1);
    if (!copy)
    {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(copy, s, len + 1);
    return copy;
}

/* Free string allocated by library */
MYLIB_API void mylib_free_string(char* s)
{
    /* free accepts NULL; nothing to do if s is NULL */
    free(s);
}

MYLIB_API mylib_error_t mylib_set_option(const char* key, const char* value)
{
    if (!key || !value)
        return MYLIB_ERR_INVALID_ARG;
    if (!atomic_load(&g_initialized))
        return MYLIB_ERR_NOT_INITIALIZED;

    /* Acquire lock and set/replace option */
    options_lock_acquire();

    option_node_t* node = find_option_node(key);
    if (node)
    {
        /* replace value */
        char* new_value = strdup(value);
        if (!new_value)
        {
            options_lock_release();
            return MYLIB_ERR_OOM;
        }
        free(node->value);
        node->value = new_value;
        options_lock_release();
        return MYLIB_OK;
    }

    /* create new node */
    option_node_t* new_node = (option_node_t*)malloc(sizeof(option_node_t));
    if (!new_node)
    {
        options_lock_release();
        return MYLIB_ERR_OOM;
    }
    new_node->key   = strdup(key);
    new_node->value = strdup(value);
    if (!new_node->key || !new_node->value)
    {
        free(new_node->key);
        free(new_node->value);
        free(new_node);
        options_lock_release();
        return MYLIB_ERR_OOM;
    }
    new_node->next = g_options_head;
    g_options_head = new_node;

    options_lock_release();
    return MYLIB_OK;
}

/*
 * mylib_get_option - caller receives an allocated string in *out_value
 * which must be freed using mylib_free_string(). If the key is not found,
 * *out_value is set to NULL and MYLIB_ERR_INVALID_ARG is returned.
 */
MYLIB_API mylib_error_t mylib_get_option(const char* key, char** out_value)
{
    if (!key || !out_value)
        return MYLIB_ERR_INVALID_ARG;
    if (!atomic_load(&g_initialized))
        return MYLIB_ERR_NOT_INITIALIZED;

    options_lock_acquire();
    option_node_t* node = find_option_node(key);
    if (!node)
    {
        options_lock_release();
        *out_value = NULL;
        return MYLIB_ERR_INVALID_ARG;
    }

    /* duplicate the stored value for the caller */
    char* dup = strdup(node->value);
    if (!dup)
    {
        options_lock_release();
        return MYLIB_ERR_OOM;
    }

    options_lock_release();
    *out_value = dup;
    return MYLIB_OK;
}

MYLIB_API const char* mylib_error_string(mylib_error_t err)
{
    switch (err)
    {
        case MYLIB_OK:
            return "OK";
        case MYLIB_ERR_INVALID_ARG:
            return "Invalid argument";
        case MYLIB_ERR_OOM:
            return "Out of memory";
        case MYLIB_ERR_NOT_INITIALIZED:
            return "Not initialized";
        case MYLIB_ERR_INTERNAL:
            return "Internal error / overflow";
        default:
            return "Unknown error";
    }
}

/* End of mylib.c */
