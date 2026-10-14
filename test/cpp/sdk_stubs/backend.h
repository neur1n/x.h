#ifndef X_TEST_BACKEND_H
#define X_TEST_BACKEND_H
#include <cstddef>
#include <cstdint>
#include <cstdlib>
// Synthetic SDK-shaped APIs; these do not model actual GPU execution.
struct test_backend
{
  int creates{0}, destroys{0}, records{0}, synchronizes{0}, elapsed_calls{0};
  int live{0}, fail_create_at{0}, fail_destroy_at{0};
  int record_error{0}, sync_error{0}, elapsed_error{0}, query_error{0};
  int devices{3}, memory_type{1}, message_error{0};
  int memory_queries{0};
  int pointer_queries{0}, device_queries{0}, message_calls{0};
  int last_error{0};
  int pending_error{0}, capture_calls{0};
  const char* message{nullptr};
  float milliseconds{2.0f};
  bool null_message{false};
  const void* last_pointer{nullptr};
  const void* last_record_event{nullptr};
  const void* last_sync_event{nullptr};
  const void* last_elapsed_start{nullptr};
  const void* last_elapsed_stop{nullptr};
  std::uintptr_t last_driver_pointer{0};
  unsigned int last_flags{0};
  void* last_stream{nullptr};
};
inline test_backend driver, runtime;
#endif
