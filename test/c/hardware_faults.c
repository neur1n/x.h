#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define X_INL static inline
#include "../hardware_fault_cases.h"

int main(void)
{
  check_hardware();
  return 0;
}
