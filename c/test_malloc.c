#define CKB_C_STDLIB_PRINTF
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ckb_syscalls.h"
#include "helper.h"

#ifndef CKB_BRK_MIN
extern char _end[]; /* _end is set in the linker */
#define CKB_BRK_MIN ((uintptr_t)&_end)
#endif
#ifndef CKB_BRK_MAX
#define CKB_BRK_MAX 0x00300000
#endif

void* _sbrk(uintptr_t incr) {
    static uintptr_t p = CKB_BRK_MIN;
    uintptr_t start = p;
    p += incr;
    if (p > CKB_BRK_MAX) {
        return (void*)-1;
    }
    return (void*)start;
}

int main() {
    uint64_t base = (uint64_t)_sbrk(0);
    printf("_end = %lu K\n", base / 1024);
    uint64_t ptr1 = (uint64_t)_sbrk(100);
    uint64_t ptr2 = (uint64_t)_sbrk(200);
    ASSERT_EQ(ptr2 - ptr1, 100);
    uint64_t* ptr3 = _sbrk(8);
    *ptr3 = 100;
    *(ptr3 + 1) = 200;
    uint64_t three_million = 3 * 1024 * 1024;
    uint64_t overflow_ptr = (uint64_t)_sbrk(three_million);
    ASSERT_EQ(overflow_ptr, -1);

    return 0;
}
