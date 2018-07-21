#include <cctype>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.h>
#include <L/src/rendering/Mesh.h>
#include <L/src/rendering/MeshBuilder.h>

using namespace L;

static inline void ignore_line(const char*& c) {
  while(*c!='\n' && *c!='\r') c++;
  while(*c=='\n' || *c=='\r') c++;
}
static inline void read_scalars(const char*& c, Vector3f& scalars) {
  for(uintptr_t i(0); i<3; i++) {
    while(*c==' ') c++;
    scalars[i] = atof(c);
    while(!Stream::isspace(*c)) c++;
  }
}
static inline bool read_indices(const char*& c, Vector3i& indices) {
  indices = 0;
  while(Stream::isspace(*c)) c++;
  for(uintptr_t i(0); i<3; i++) {
    indices[i] = atoi(c);
    while(isdigit(*c)) c++;
    while(*c=='/') c++;
    if(Stream::isspace(*c))
      break;
  }
  return indices.x()>0;
}
void obj_loader(Resource<Mesh>::Slot& slot) {
  struct Vertex {
    Vector3f position;
    Vector2f uv;
    Vector3f normal;
  };

  Buffer buffer(slot.read_source_file());
  const void* data(buffer.data());
  const size_t size(buffer.size());

  Array<Vector3f> positions, normals;
  Array<Vector2f> uvs;
  Vertex vertex {}, firstVertex {}, lastVertex {};
  Vector3f scalars;
  Vector3i indices;
  MeshBuilder mb;
  const char *cur((const char*)data), *end(cur+size);
  while(cur<end) {
    switch(*cur) { // Line start
      case 'v':
        cur += 1;
        switch(*cur) {
          case ' ': // Position
            read_scalars(cur, scalars);
            ignore_line(cur);
            positions.push(scalars);
            break;
          case 't': // Texture coordinates
            cur += 1;
            read_scalars(cur, scalars);
            ignore_line(cur);
            uvs.push(scalars.x(), 1.f-scalars.y());
            break;
          case 'n': // Normal
            cur += 1;
            read_scalars(cur, scalars);
            ignore_line(cur);
            normals.push(scalars);
            break;
          default: ignore_line(cur); break;
        }
        break;
      case 'f':
        cur += 1;
        for(uintptr_t i(0); read_indices(cur, indices); i++) {
          if(indices.z()>0) vertex.normal = normals[indices.z()-1];
          if(indices.y()>0) {
            if(!uvs.empty()) vertex.uv = uvs[indices.y()-1];
            else vertex.normal = normals[indices.y()-1];
          }
          vertex.position = positions[indices.x()-1];

          if(i==0) firstVertex = vertex;
          else if(i>2) {
            mb.addVertex(&firstVertex, sizeof(Vertex));
            mb.addVertex(&lastVertex, sizeof(Vertex));
          }
          mb.addVertex(&vertex, sizeof(Vertex));
          lastVertex = vertex;
        }
        break;
      default: ignore_line(cur); break;
    }
  }

  if(normals.empty())
    mb.computeNormals(0, sizeof(Vector2f)+sizeof(Vector3f), sizeof(Vertex));

  static const VkFormat formats[] {
    VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_SFLOAT,
  };

  slot.value = Memory::new_type<Mesh>(mb, formats, L_COUNT_OF(formats));
}

void wavefront_obj_module_init() {
  Resource<Mesh>::add_loader("obj", obj_loader);
}
