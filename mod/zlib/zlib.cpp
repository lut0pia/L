#include <L/src/container/Buffer.h>
#include <L/src/macros.h>
#include <L/src/stream/CFileStream.h>
#include <L/src/text/compression.h>

L_PUSH_NO_WARNINGS

#include <zlib.h>

L_POP_NO_WARNINGS

using namespace L;

static void zlib_compress(const void* in_data_void, size_t in_size, Stream& out_stream) {
  char buffer[1024];
  z_stream stream{};
  int init_result = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
  L_ASSERT(init_result == Z_OK);
  stream.next_in = (Bytef*)in_data_void;
  stream.avail_in = (uInt)in_size;

  int deflate_result;
  do {
    stream.next_out = (Bytef*)buffer;
    stream.avail_out = sizeof(buffer);
    deflate_result = deflate(&stream, Z_FINISH);
    L_ASSERT(deflate_result == Z_OK || deflate_result == Z_STREAM_END || deflate_result == Z_BUF_ERROR);
    out_stream.write(buffer, sizeof(buffer) - stream.avail_out);
  } while(stream.avail_out == 0);
  L_ASSERT(stream.total_in == in_size);

  deflateEnd(&stream);
}

static Buffer zlib_decompress(const void* in_data_void, size_t in_size) {
  Buffer buffer(16);
  z_stream stream{};
  int init_result = inflateInit(&stream);
  L_ASSERT(init_result == Z_OK);
  stream.next_in = (Bytef*)in_data_void;
  stream.avail_in = (uInt)in_size;

  int inflate_result;
  do {
    buffer.size(buffer.size() * 2);
    stream.next_out = ((Bytef*)buffer.data()) + stream.total_out;
    stream.avail_out = (uInt)(buffer.size() - stream.total_out);
    inflate_result = inflate(&stream, Z_FINISH);
    L_ASSERT(inflate_result == Z_OK || inflate_result == Z_STREAM_END || inflate_result == Z_BUF_ERROR);
  } while(stream.avail_out == 0);
  L_ASSERT(stream.total_in == in_size);

  buffer.size(stream.total_out);

  inflateEnd(&stream);

  return buffer;
}

void zlib_module_init() {
  register_compression({"zlib", zlib_compress, zlib_decompress});
}
