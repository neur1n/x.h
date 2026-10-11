#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#define X_INL inline
#include "../hardware_fault_cases.h"

int main()
{
  check_hardware();
  return 0;
}
