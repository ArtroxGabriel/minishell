/* Auto-generated minimal test for os-c-template */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mylib.h"

int main(void)
{
    /* initialize library if needed */
    if (mylib_init() != MYLIB_OK)
    {
        fprintf(stderr, "mylib_init failed\n");
        return EXIT_FAILURE;
    }

    /* simple functional checks */
    assert(mylib_add(2, 3) == 5);

    int32_t out = 0;
    if (mylib_subtract_checked(10, 4, &out) != MYLIB_OK)
    {
        fprintf(stderr, "mylib_subtract_checked failed\n");
        return EXIT_FAILURE;
    }
    assert(out == 6);

    char* s = mylib_dup_string("hello");
    if (!s)
    {
        fprintf(stderr, "mylib_dup_string failed\n");
        return EXIT_FAILURE;
    }
    puts(s);
    mylib_free_string(s);

    mylib_shutdown();
    puts("[OK] basic tests passed");
    return EXIT_SUCCESS;
}
