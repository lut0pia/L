#include <cctype>
#include <cstdlib>
#include <L/src/container/Buffer.h>
#include <L/src/engine/Resource.inl>
#include <L/src/rendering/Mesh.h>

using namespace L;

static const Symbol obj_symbol("obj");

static inline void ignore_line(const char*& c, const char* end) {
  while(c < end && *c != '\n' && *c != '\r') c++;
  while(c < end && (*c == '\n' || *c == '\r')) c++;
}
static inline void read_scalars(const char*& c, Vector3f& scalars) {
  for(uintptr_t i(0); i < 3; i++) {
    while(*c == ' ') c++;
    scalars[i] = float(atof(c));
    while(!Stream::isspace(*c)) c++;
  }
}
static inline bool read_indices(const char*& c, Vector3ui& indices) {
  indices = 0u;
  while(Stream::isspace(*c)) c++;
  for(uintptr_t i(0); i < 3; i++) {
    indices[i] = atoi(c);
    while(isdigit(*c)) c++;
    while(*c == '/') c++;
    if(Stream::isspace(*c))
      break;
  }
  return indices.x() > 0;
}
template <size_t d>
static inline void add_attribute(Array<float>& vertex, const Vector<d, float>& attr) {
  for(uintptr_t i = 0; i < d; i++) {
    vertex.push(attr[i]);
  }
}
bool obj_loader(ResourceSlot& slot, Mesh::Intermediate& intermediate) {
  if(slot.ext != obj_symbol) {
    return false;
  }

  Buffer buffer(slot.read_source_file());
  Array<float> vertices;

  Array<Vector3f> positions, normals;
  Array<Vector2f> texcoords;
  Vector3f scalars;
  Vector3ui indices;
  const char *cur((const char*)buffer.data()), *end(cur + buffer.size());
  while(cur < end) {
    switch(*cur) { // Line start
      case 'v':
        cur += 1;
        switch(*cur) {
          case ' ': // Position
            read_scalars(cur, scalars);
            ignore_line(cur, end);
            positions.push(scalars);
            break;
          case 't': // Texture coordinates
            cur += 1;
            read_scalars(cur, scalars);
            ignore_line(cur, end);
            texcoords.push(Vector2f(scalars.x(), 1.f - scalars.y()));
            break;
          case 'n': // Normal
            cur += 1;
            read_scalars(cur, scalars);
            ignore_line(cur, end);
            normals.push(scalars);
            break;
          default: ignore_line(cur, end); break;
        }
        break;
      case 'f':
      {
        cur += 1;

        Array<float> first_vertex, last_vertex;
        for(uintptr_t i(0); read_indices(cur, indices); i++) {
          Array<float> vertex;
          add_attribute(vertex, positions[indices.x() - 1]);
          if(indices.y() > 0 && texcoords.size() >= indices.y()) {
            add_attribute(vertex, texcoords[indices.y() - 1]);
          }
          if(indices.z() > 0 && normals.size() >= indices.z()) {
            add_attribute(vertex, normals[indices.z() - 1]);
          }

          if(i == 0) first_vertex = vertex;
          else if(i > 2) {
            vertices += first_vertex;
            vertices += last_vertex;
          }
          vertices += vertex;
          last_vertex = vertex;
        }
        break;
      }
      default: ignore_line(cur, end); break;
    }
  }

  intermediate.vertices = Buffer(vertices.begin(), vertices.size() * sizeof(float));
  intermediate.indices = Buffer();
  intermediate.attributes.clear();
  intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32_SFloat, VertexAttributeType::Position});
  if(!texcoords.empty()) {
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32_SFloat, VertexAttributeType::TexCoord});
  }
  if(!normals.empty()) {
    intermediate.attributes.push(VertexAttribute {RenderFormat::R32G32B32_SFloat, VertexAttributeType::Normal});
  }
  return true;
}

void wavefront_obj_module_init() {
  ResourceLoading<Mesh>::add_loader(obj_loader);
}
