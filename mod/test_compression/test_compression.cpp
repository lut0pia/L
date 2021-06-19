#include <L/src/dev/test.h>
#include <L/src/stream/StringStream.h>
#include <L/src/text/compression.h>

using namespace L;

void test_compression_module_init() {
  Test test_compression{};
  test_compression.name = "compression";
  test_compression.func = []() {
    bool success = true;

    Array<String> test_strings{
      "",
      "L",
      "Word",
      "Test string that's pretty simple and short",
      "blablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablablabla",
      "Another test string that's rather long. We're going for a lot more information here to make sure the compression can take it. I'll now paste some lorem ipsum. Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
    };

    for(uintptr_t i = 0; i < test_strings.size(); i++) {
      const String& test_string = test_strings[i];
      StringStream compressed_stream;
      lz_compress(test_string.begin(), test_string.size(), compressed_stream);

      Buffer decompressed_buffer;
      decompressed_buffer = lz_decompress(compressed_stream.string().begin(), compressed_stream.string().size());

      if(test_string.size() != decompressed_buffer.size()) {
        log("test_compression: [%d] failed with different size (%d to %d)", i, test_string.size(), decompressed_buffer.size());
        success = false;
      } else if(memcmp(test_string.begin(), decompressed_buffer.data(), test_string.size())) {
        log("test_compression: [%d] failed with different values for", i);
        success = false;
      } else {
        log("test_compression: [%d] succeeded with ratio %f", i, (float(compressed_stream.size()) / float(test_string.size())));
      }
    }

    return success;
  };

  add_test(test_compression);
}
