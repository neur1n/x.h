#define X_ERROR_DOMAIN_IMPLEMENTATION
#if TEST_RESOLVE_FAULTS
#include "../filesystem_fault_injection.h"
#endif
#if TEST_GENERATED
#include "generated_under_test.h"
#else
#include "../../c/x.h"
#endif
#include "../filesystem_cases.h"
