#define X_ERROR_DOMAIN_IMPLEMENTATION
#include "error_test_support.h"

static uint32_t reference_crc32(const uint8_t* data, const size_t size)
{
  // Use the normal polynomial and a left-shifting register, unlike the
  // reflected, right-shifting production implementation. Input bits arrive
  // least significant first; reflect the register before the final complement.
  uint32_t remainder = 0xFFFFFFFFu;
  for (size_t i = 0; i < size; ++i) {
    for (unsigned int bit = 0; bit < 8; ++bit) {
      const uint32_t feedback = (remainder >> 31) ^ ((data[i] >> bit) & 1u);
      remainder <<= 1;
      if (feedback != 0) {
        remainder ^= 0x04C11DB7u;
      }
    }
  }

  uint32_t reflected = 0;
  for (unsigned int bit = 0; bit < 32; ++bit) {
    reflected = (reflected << 1) | (remainder & 1u);
    remainder >>= 1;
  }
  return ~reflected;
}

static uint16_t reference_internet(const uint8_t* data, const size_t size)
{
  // Sum individual byte contributions with a wide accumulator. Test inputs
  // are bounded, so this reference cannot overflow before the final fold.
  uint64_t sum = 0;
  for (size_t i = 0; i < size; ++i) {
    sum += (uint64_t)data[i] << (i % 2 == 0 ? 8 : 0);
  }
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }
  return (uint16_t)~sum;
}

static uint8_t reference_xor(const uint8_t* data, const size_t size)
{
  uint8_t value = 0;
  for (size_t i = 0; i < size; ++i) {
    value ^= data[i];
  }
  return value;
}

int main(void)
{
  CHECK(reference_crc32((const uint8_t*)"123456789", 9) == 0xCBF43926u);
  CHECK(x_checksum_crc32("123456789", 9, NULL) == 0xCBF43926u);
  const uint8_t rfc[] = {0x00, 0x01, 0xF2, 0x03, 0xF4, 0xF5, 0xF6, 0xF7};
  CHECK(x_checksum_rfc1071(rfc, sizeof(rfc), NULL) == 0x220D);
  const uint8_t pair[] = {1, 2};
  CHECK(x_checksum_rfc1071(pair, sizeof(pair), NULL) == 0xFEFD);
  const uint8_t carry[] = {0, 1, 0xFF, 0xFF};
  uint16_t previous = x_checksum_rfc1071(carry, 2, NULL);
  CHECK(x_checksum_rfc1071(carry + 2, 2, &previous) == 0xFFFE);

  uint8_t verified[sizeof(rfc) + 2];
  memcpy(verified, rfc, sizeof(rfc));
  verified[sizeof(rfc)] = 0x22;
  verified[sizeof(rfc) + 1] = 0x0D;
  CHECK(x_checksum_rfc1071(verified, sizeof(verified), NULL) == 0);

  for (size_t size = 0; size <= 257; ++size) {
    // Exactly bounded, deliberately unaligned inputs exercise odd tails and
    // both sides of the 8/64-byte XOR boundaries under AddressSanitizer too.
    uint8_t* allocation = (uint8_t*)malloc(size + 1);
    CHECK(allocation != NULL);
    uint8_t* data = allocation + 1;
    for (size_t i = 0; i < size; ++i) {
      data[i] = (uint8_t)(i * 37 + size * 11);
    }
    const uint8_t expected_xor = reference_xor(data, size);
    const uint16_t expected_internet = reference_internet(data, size);
    const uint32_t expected_crc = reference_crc32(data, size);
    CHECK(x_checksum_crc32(data, size, NULL) == expected_crc);
    CHECK(x_checksum_xor(data, size, NULL) == expected_xor);
    CHECK(x_checksum_rfc1071(data, size, NULL) == expected_internet);
    for (size_t split = 0; split <= size; ++split) {
      uint8_t xor_part = x_checksum_xor(data, split, NULL);
      uint32_t crc_part = x_checksum_crc32(data, split, NULL);
      CHECK(x_checksum_xor(data + split, size - split, &xor_part) == expected_xor);
      CHECK(x_checksum_crc32(data + split, size - split, &crc_part) == expected_crc);
      if (split % 2 == 0) {
        uint16_t part = x_checksum_rfc1071(data, split, NULL);
        CHECK(x_checksum_rfc1071(data + split, size - split, &part)
            == expected_internet);
      }
    }
    free(allocation);
  }

  const size_t large_size = 131076;
  uint8_t* large = (uint8_t*)malloc(large_size);
  CHECK(large != NULL);
  memset(large, 0xFF, large_size);
  CHECK(x_checksum_rfc1071(large, large_size, NULL) == 0);
  CHECK(x_checksum_rfc1071(large, large_size, NULL)
      == reference_internet(large, large_size));
  free(large);

  uint32_t crc = 0x12345678;
  uint16_t internet = 0x1234;
  uint8_t xor_value = 0x12;
  CHECK(x_checksum_crc32(NULL, 0, NULL) == 0);
  CHECK(x_checksum_rfc1071(NULL, 0, NULL) == 0xFFFF);
  CHECK(x_checksum_xor(NULL, 0, NULL) == 0);
  CHECK(x_checksum_crc32(NULL, 17, &crc) == crc);
  CHECK(x_checksum_rfc1071(NULL, 17, &internet) == internet);
  CHECK(x_checksum_xor(NULL, 17, &xor_value) == xor_value);
  CHECK(x_checksum_crc32(pair, 0, NULL) == 0);
  CHECK(x_checksum_rfc1071(pair, 0, NULL) == 0xFFFF);
  CHECK(x_checksum_xor(pair, 0, NULL) == 0);
  CHECK(x_checksum_crc32(pair, 0, &crc) == crc);
  CHECK(x_checksum_rfc1071(pair, 0, &internet) == internet);
  CHECK(x_checksum_xor(pair, 0, &xor_value) == xor_value);

#if X_WINDOWS
  // The last input byte borders an inaccessible page. This catches overreads
  // on the available MinGW build without relying on sanitizer support.
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  const size_t page = info.dwPageSize;
  uint8_t* guarded = (uint8_t*)VirtualAlloc(
      NULL, page * 2, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  CHECK(guarded != NULL);
  DWORD old_protection = 0;
  CHECK(VirtualProtect(guarded + page, page, PAGE_NOACCESS, &old_protection));
  for (size_t size = 1; size <= 129; ++size) {
    uint8_t* data = guarded + page - size;
    for (size_t i = 0; i < size; ++i) {
      data[i] = (uint8_t)(i * 13 + size);
    }
    CHECK(x_checksum_crc32(data, size, NULL) == reference_crc32(data, size));
    CHECK(x_checksum_xor(data, size, NULL) == reference_xor(data, size));
    CHECK(x_checksum_rfc1071(data, size, NULL) == reference_internet(data, size));
  }
  CHECK(VirtualFree(guarded, 0, MEM_RELEASE));
#endif
  return EXIT_SUCCESS;
}
