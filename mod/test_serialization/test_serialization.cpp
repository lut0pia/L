#include <L/src/container/Table.h>
#include <L/src/dev/test.h>
#include <L/src/dynamic/Variable.h>
#include <L/src/math/Quaternion.h>
#include <L/src/stream/StringStream.h>
#include <L/src/text/String.h>

using namespace L;

template <class T>
void serial_test(bool& success, const T& v) {
  StringStream print_stream;
  print_stream << v;
  StringStream test_stream;
  test_stream <= v;
  T v2;
  test_stream.rewind();
  test_stream >= v2;
  if(v == v2) {
    log("test_serialization: succeeded for %s (%s)", print_stream.string().begin(), type_name<T>());
  } else {
    warning("test_serialization: failed for %s (%s)", print_stream.string().begin(), type_name<T>());
    success = false;
  }
}

void test_serialization_module_init() {
  Test test_serialization{};
  test_serialization.name = "serialization";
  test_serialization.func = []() {
    bool success = true;

    serial_test(success, Var(Vector3f(2.f, 1.6f, -1.f)));
    serial_test(success, Symbol("symbol"));
    serial_test(success, Quatf(Vector3f(1.f, 0.f, 0.f), 0.75f));
    serial_test(success, Array<bool>{true, false});
    serial_test(success, String("A short string."));
    serial_test(success, Array<Vector3i>{Vector3i(0, 0, 1), Vector3i(1, 0, 0), Vector3i(0, 1, 0)});
    serial_test(success, Vector3i(0, -4, 6));
    serial_test(success, Array<int>{0, -4, 6, 12, 42});
    serial_test(success, 1.f / 3.f);
    serial_test(success, 18);
    serial_test(success, -8);
    serial_test(success, 0);
    serial_test(success, sqrt(2.f));
#if 0 // No support for table equality
    {
      Table<int, String> table;
      table[2] = "prout";
      table[4] = "caca";
      serial_test(success, table);
    }
#endif

    return success;
  };

  add_test(test_serialization);
}
