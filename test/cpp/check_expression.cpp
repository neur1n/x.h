#define X_LOG_LEVEL (-1)
#include "../../cpp/x.h"
#include "support.h"

static int calls, domain_calls, output_calls;

static int32_t status()
{
  ++calls;
  return 0;
}

static int32_t status(int value)
{
  ++calls;
  return value;
}

static int32_t defaulted(int value = 0)
{
  ++calls;
  return value;
}

template<typename A, typename B>
static int32_t typed_status()
{
  static_assert(std::is_integral_v<A> && std::is_integral_v<B>);
  ++calls;
  return 0;
}

static const char* domain_name()
{
  ++domain_calls;
  return "generic";
}

static x_error& output(x_error& err)
{
  ++output_calls;
  return err;
}

static x_error returned()
{
  ++calls;
  return x_error("generic", 7);
}

class operation
{
public:
  int32_t status(int value = 0)
  {
    ++this->m_calls;
    return value;
  }

  int calls() const
  {
    return this->m_calls;
  }

private:
  int m_calls{0};
};

int main()
{
  x_error err;
  x_check(domain_name(), output(err), status());
  CHECK(calls == 1 && domain_calls == 1 && output_calls == 1 && !err);
  int value = 0;
  x_check("generic", err, status(value++));
  CHECK(value == 1 && calls == 2 && !err);
  x_check("generic", err, defaulted());
  CHECK(calls == 3 && !err);

  int32_t (*function)() = static_cast<int32_t (*)()>(status);
  x_check("generic", err, function());
  CHECK(calls == 4 && !err);
  operation op;
  x_check("generic", err, op.status());
  x_check("generic", err, op.status(0));
  CHECK(op.calls() == 2 && !err);

  // Extra parentheses protect commas visible to the macro preprocessor.
  x_check("generic", err, (typed_status<int, unsigned int>()));
  x_check("generic", err, (status(), 0));
  int32_t expression = 0;
  x_check("generic", err, expression);
  CHECK(calls == 6 && !err);

  // An x_error result keeps its own domain, even if the supplied name is invalid.
  x_check((++domain_calls, "missing"), output(err), returned());
  CHECK(calls == 7 && domain_calls == 2 && output_calls == 2);
  CHECK(err.value() == 7 && strcmp(err.domain(), "generic") == 0 && err);
  const x_error value_error("generic", 9);
  x_check("missing", err, value_error);
  CHECK(err.value() == 9 && err);
  return 0;
}
