#pragma once

#include <L/src/L.h>

namespace L {
  namespace DAE {
    static bool is_node(const Var* node);
    static const Symbol* node_name(const Var* node);
    static const Array<Var>* node_children(const Var* node);
    static const Var* node_child(const Var* node, const Symbol& key);
    static const Var* node_child_id(const Var* node, const String& id);
    static const Table<Symbol, Var>* node_attributes(const Var* node);
    static const String* node_inner_text(const Var* node);
    template <int d, typename T> static void fill_vector_array(const String& text, Array<Vector<d, T>>& array);
    template <typename T> static void fill_array(const String& text, Array<T>& array);

    class DAEMesh : public Interface<Mesh> {
      static DAEMesh instance;
    private:
      typedef struct {
        Vector3f position;
        Vector2f uv;
        Vector3f normal;
        Vector4f joints;
        Vector4f weights;
      } Vertex;

    public:
      inline DAEMesh() : Interface{"dae"} {}

      Ref<Mesh> from(const byte* data, size_t size) override {
        Ref<Var> root(Interface<Var>::from_bytes(data, size, "xml"));
        if(!root || !is_node(root)) return nullptr;

        Array<Vector3f> positions, normals;
        Array<Vector2f> uvs;
        Array<Vector4f> joints, weights;

        const Var* library_geometries(node_child(root, "library_geometries"));
        const Var* geometry(node_child(library_geometries, "geometry"));
        const Var* mesh(node_child(geometry, "mesh"));
        const Var* triangles(node_child(mesh, "triangles"));

        for(const auto& child : *node_children(triangles)) {
          if(*node_name(&child)==Symbol("input")) {
            const Table<Symbol, Var>* attributes(node_attributes(&child));
            const String& semantic(attributes->find("semantic")->as<String>());
            String source_id(attributes->find("source")->as<String>().substr(1));
            const Var* source(node_child_id(mesh, source_id));

            if(*node_name(source)!=Symbol("source")) { // Handle redirections
              const Var* input(node_child(source, "input"));
              source_id = (node_attributes(input)->find("source")->as<String>().substr(1));
              source = node_child_id(mesh, source_id);
            }

            const Var* float_array(node_child(source, "float_array"));
            const String* float_array_text(node_inner_text(float_array));

            if(semantic=="VERTEX") {
              fill_vector_array(*float_array_text, positions);
            } else if(semantic=="NORMAL") {
              fill_vector_array(*float_array_text, normals);
            } else if(semantic=="TEXCOORD") {
              fill_vector_array(*float_array_text, uvs);
            }
          }
        }

        bool skinned(false);
        {
          Array<Vector2i> vertex_joint_weight_index;
          Array<int> vertex_joint_count;
          Array<float> vertex_weight;

          const Var* library_controllers(node_child(root, "library_controllers"));
          const Var* controller(node_child(library_controllers, "controller"));
          const Var* skin(node_child(controller, "skin"));
          const Var* vertex_weights(node_child(skin, "vertex_weights"));
          const Var* vertex_weights_v(node_child(vertex_weights, "v"));
          const Var* vertex_weights_vcount(node_child(vertex_weights, "vcount"));
          fill_vector_array(*node_inner_text(vertex_weights_v), vertex_joint_weight_index);
          fill_array(*node_inner_text(vertex_weights_vcount), vertex_joint_count);

          for(const auto& child : *node_children(vertex_weights)) {
            if(*node_name(&child)==Symbol("input")) {
              const Table<Symbol, Var>* attributes(node_attributes(&child));
              const String& semantic(attributes->find("semantic")->as<String>());
              String source_id(attributes->find("source")->as<String>().substr(1));
              const Var* source(node_child_id(skin, source_id));

              if(semantic=="WEIGHT") {
                const Var* float_array(node_child(source, "float_array"));
                fill_array(*node_inner_text(float_array), vertex_weight);
              }
            }
          }

          if(vertex_joint_count.size()==positions.size()) {
            joints.size(positions.size(), 0.f);
            weights.size(positions.size(), 0.f);
            skinned = true;
            uintptr_t joint_index(0);
            for(uintptr_t i(0); i<positions.size(); i++) {
              const int joint_count(min(vertex_joint_count[i], 4));
              if(joint_count>0) {
                Vector4f& joint(joints[i]);
                Vector4f& weight(weights[i]);
                for(uintptr_t j(0); j<joint_count; j++) {
                  const Vector2i& joint_weight_index(vertex_joint_weight_index[joint_index]);
                  joint[j] = joint_weight_index.x();
                  weight[j] = vertex_weight[joint_weight_index.y()];
                  joint_index++;
                }
                weight /= weight.sum(); // Make sure weight sum is one
              }
            }
          }
        }

        {
          const Var* triangles_p(node_child(triangles, "p"));
          const String* tri_str(node_inner_text(triangles_p));
          Array<Vector3i> triangle_indices;
          fill_vector_array(*tri_str, triangle_indices);
          const size_t vertex_size = skinned ? sizeof(Vertex) : (sizeof(Vertex)-sizeof(Vector4f)*2);

          MeshBuilder mb;
          for(const Vector3i& tri : triangle_indices) {
            Vertex vertex;
            vertex.position = positions[tri[0]];
            if(skinned) {
              vertex.joints = joints[tri[0]];
              vertex.weights = weights[tri[0]];
            }
            vertex.normal = normals[tri[1]];
            vertex.uv = uvs[tri[2]];
            vertex.uv.y() = 1.f-vertex.uv.y();
            mb.addVertex(&vertex, vertex_size);
          }

          static const std::initializer_list<Mesh::Attribute> attributes = {
            {3,GL_FLOAT,GL_FALSE}, // Positions
            {2,GL_FLOAT,GL_FALSE}, // Texcoords
            {3,GL_FLOAT,GL_FALSE}, // Normals
            {4,GL_FLOAT,GL_FALSE}, // Joints
            {4,GL_FLOAT,GL_FALSE}, // Weights
          };
          return ref<Mesh>(mb, GL_TRIANGLES, attributes);
        }
        return nullptr;
      }
    };
    DAEMesh DAEMesh::instance;

    static bool is_node(const Var* node) {
      return node && node->is<Table<Symbol, Var>>();
    }
    static const Symbol* node_name(const Var* node) {
      if(is_node(node)) {
        const Table<Symbol, Var>& node_table(node->as<Table<Symbol, Var>>());
        if(const Var* node_name_var = node_table.find("name")) {
          return &node_name_var->as<Symbol>();
        }
      }
      return nullptr;
    }
    static const Array<Var>* node_children(const Var* node) {
      if(is_node(node)) {
        const Table<Symbol, Var>& node_table(node->as<Table<Symbol, Var>>());
        if(const Var* node_children_var = node_table.find("children")) {
          return &node_children_var->as<Array<Var>>();
        }
      }
      return nullptr;
    }
    static const Var* node_child(const Var* node, const Symbol& key) {
      if(const Array<Var>* children = node_children(node)) {
        for(const auto& child : *children) {
          if(*node_name(&child)==key)
            return &child;
        }
      }
      return nullptr;
    }
    static const Var* node_child_id(const Var* node, const String& id) {
      if(const Array<Var>* children = node_children(node)) {
        for(const auto& child : *children) {
          const Table<Symbol, Var>* attributes(node_attributes(&child));
          if(const Var* node_id_var = attributes->find("id")) {
            if(*node_id_var==id)
              return &child;
          }
        }
      }
      return nullptr;
    }
    static const Table<Symbol, Var>* node_attributes(const Var* node) {
      if(is_node(node)) {
        const Table<Symbol, Var>& node_table(node->as<Table<Symbol, Var>>());
        if(const Var* node_attributes_var = node_table.find("attributes")) {
          return &node_attributes_var->as<Table<Symbol, Var>>();
        }
      }
      return nullptr;
    }
    static const String* node_inner_text(const Var* node) {
      if(const Array<Var>* children = node_children(node)) {
        if(children->size()==1 && (*children)[0].is<String>()) {
          return &(*children)[0].as<String>();
        }
      }
      return nullptr;
    }

    template <int d, typename T>
    static void fill_vector_array(const String& text, Array<Vector<d, T>>& array) {
      uintptr_t index(0);
      const char* c(text.begin());
      while(*c) {
        if(index%d==0) array.push(read<T>(c));
        else array[index/d][index%d] = read<T>(c);
        index++;
        while(*c && !Stream::isspace(*c)) c++;
        while(*c && Stream::isspace(*c)) c++;
      }
    }
    template <typename T>
    static void fill_array(const String& text, Array<T>& array) {
      const char* c(text.begin());
      while(*c) {
        array.push(read<T>(c));
        while(*c && !Stream::isspace(*c)) c++;
        while(*c && Stream::isspace(*c)) c++;
      }
    }
    template <typename T> static T read(const char* str);
    template <> inline float read(const char* str) { return atof(str); }
    template <> inline int read(const char* str) { return atoi(str); }
  }
}