#define CKB_C_STDLIB_PRINTF
#define CKB_C_STDLIB_MALLOC
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ckb_syscalls.h"
#include "helper.h"

#define COUNT_OF(a) (sizeof(a) / sizeof(a[0]))

void reset(void) {
    s_program_break = CKB_BRK_MIN;
    memset(&mal, 0, sizeof(mal));
}


void loop_free(void *array[], size_t array_length, size_t step) {
  for (size_t i = 0; i < array_length; i += step) {
    free(array[i]);
    array[i] = 0;
  }
}

void test_malloc() {
  void *array[1024] = {0};
  for (size_t i = 0; i < COUNT_OF(array); i++) {
    array[i] = malloc(i * 4);
    ASSERT(array[i] != 0);
  }
  loop_free(array, COUNT_OF(array), 2);
  loop_free(array, COUNT_OF(array), 3);
  loop_free(array, COUNT_OF(array), 4);
  loop_free(array, COUNT_OF(array), 5);

  void *array2[1024] = {0};
  for (size_t i = 0; i < COUNT_OF(array); i++) {
    array[i] = malloc(i * 4);
    ASSERT(array[i] != 0);
  }
  loop_free(array2, COUNT_OF(array2), 1);

  uint64_t p = (uint64_t)_sbrk(0);
  printf("finally, the brk is at %lu K\n", p / 1024);
  uint64_t max_brk = 3 * 1024 * 1024;
  void *ptr = malloc(max_brk - p);
  ASSERT_EQ(ptr, 0);

  ptr = malloc(max_brk - p - 100);
  ASSERT(ptr != 0);
}

void test_sbrk() {
  uint64_t base = (uint64_t)_sbrk(0);
  printf("_end = %lu K\n", base / 1024);
  uint64_t ptr1 = (uint64_t)_sbrk(100);
  uint64_t ptr2 = (uint64_t)_sbrk(200);
  ASSERT_EQ(ptr2 - ptr1, 100);
  uint64_t *ptr3 = _sbrk(8);
  *ptr3 = 100;
  *(ptr3 + 1) = 200;
  uint64_t three_million = 3 * 1024 * 1024;
  uint64_t overflow_ptr = (uint64_t)_sbrk(three_million);
  ASSERT_EQ(overflow_ptr, -1);
}

int main() {
  test_malloc();
  reset();
  test_malloc();
  reset();
  test_sbrk();
  reset();
  return 0;
}
