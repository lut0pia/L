#include <L/src/dev/profiling.h>
#include <L/src/engine/Engine.h>
#include <L/src/engine/Resource.h>
#include <L/src/engine/Resource.inl>
#include <L/src/input/InputContext.h>
#include <L/src/rendering/Material.h>
#include <L/src/rendering/Mesh.h>
#include <L/src/rendering/Texture.h>

#include <imgui.h>

using namespace L;

static Resource<Texture> font_tex;
static Resource<Mesh> mesh;
static Array<Material> materials;
static const Symbol imgui_symbol = "imgui", frag_symbol = "frag", vert_symbol = "vert";
static String text_input;
static InputContext input_context;

static void imgui_new_frame() {
  ImGuiIO& io = ImGui::GetIO();
  io.DeltaTime = Engine::delta_seconds();
  io.DisplaySize.x = float(Window::width());
  io.DisplaySize.y = float(Window::height());

  if(io.WantCaptureKeyboard || io.WantCaptureMouse || io.WantTextInput) {
    input_context.set_block_mode(InputBlockMode::All);
  } else {
    input_context.set_block_mode(InputBlockMode::None);
  }

  { // Update input
    io.MousePos.x = float(Window::cursor_x());
    io.MousePos.y = float(Window::cursor_y());
    io.MouseDown[0] = input_context.get_raw_button(Device::Button::MouseLeft);
    io.MouseDown[1] = input_context.get_raw_button(Device::Button::MouseRight);
    io.MouseDown[2] = input_context.get_raw_button(Device::Button::MouseMiddle);
    io.MouseWheel = input_context.get_raw_axis(Device::Axis::MouseWheel);

    memset(io.KeysDown, 0, sizeof(io.KeysDown));
#define DB(b) io.KeysDown[(uintptr_t)Device::Button::b] |= input_context.get_raw_button(Device::Button::b);
#include <L/src/input/device_buttons.def>
#undef DB

    io.KeyCtrl = input_context.get_raw_button(Device::Button::LeftCtrl) || input_context.get_raw_button(Device::Button::RightCtrl);
    io.KeyShift = input_context.get_raw_button(Device::Button::LeftShift) || input_context.get_raw_button(Device::Button::RightShift);
    io.KeyAlt = input_context.get_raw_button(Device::Button::LeftAlt) || input_context.get_raw_button(Device::Button::RightAlt);

    io.AddInputCharactersUTF8(text_input);
    text_input.clear();
  }

  ImGui::NewFrame();
}

static void imgui_update() {
  L_SCOPE_MARKER("imgui");

  ImGui::Render();
  const ImDrawData* draw_data = ImGui::GetDrawData();
  ImDrawVert* vertex_buffer = draw_data->TotalVtxCount ? Memory::alloc_type<ImDrawVert>(draw_data->TotalVtxCount) : nullptr;
  ImDrawIdx* index_buffer = draw_data->TotalIdxCount ? Memory::alloc_type<ImDrawIdx>(draw_data->TotalIdxCount) : nullptr;
  int global_vtx_offset = 0;
  int global_idx_offset = 0;

  materials.clear();
  for(int i = 0; i < draw_data->CmdListsCount; i++) {
    const ImDrawList* draw_list = draw_data->CmdLists[i];
    for(const ImDrawCmd& cmd : draw_list->CmdBuffer) {
      Material material;
      material.shader(VK_SHADER_STAGE_VERTEX_BIT, ".imgui?stage=vert");
      material.shader(VK_SHADER_STAGE_FRAGMENT_BIT, ".imgui?stage=frag");
      material.render_pass("present");
      material.mesh(mesh);
      material.texture("tex", font_tex);
      material.scissor(Interval2i {
        Vector2i{int(cmd.ClipRect.x), int(cmd.ClipRect.y)},
        Vector2i{int(cmd.ClipRect.z), int(cmd.ClipRect.w)},
        });
      material.vertex_count(cmd.ElemCount);
      material.vertex_offset(cmd.VtxOffset + global_vtx_offset);
      material.index_offset(cmd.IdxOffset + global_idx_offset);
      material.cull_mode(VK_CULL_MODE_NONE);
      materials.push(material);
    }

    memcpy(vertex_buffer + global_vtx_offset, draw_list->VtxBuffer.Data, draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
    memcpy(index_buffer + global_idx_offset, draw_list->IdxBuffer.Data, draw_list->IdxBuffer.Size * sizeof(ImDrawIdx));

    global_vtx_offset += draw_list->VtxBuffer.Size;
    global_idx_offset += draw_list->IdxBuffer.Size;
  }

  if(draw_data->TotalVtxCount && mesh) {
    Mesh& mesh_mut = ((Mesh&)*mesh);
    mesh_mut.load(
      draw_data->TotalVtxCount,
      vertex_buffer, draw_data->TotalVtxCount * sizeof(ImDrawVert),
      mesh_mut.attributes().begin(), mesh_mut.attributes().size(),
      index_buffer, draw_data->TotalIdxCount);
  }

  if(vertex_buffer) {
    Memory::free_type<ImDrawVert>(vertex_buffer, draw_data->TotalVtxCount);
  }

  if(index_buffer) {
    Memory::free_type<ImDrawIdx>(index_buffer, draw_data->TotalIdxCount);
  }

  for(Material& material : materials) {
    material.update();
  }

  imgui_new_frame();
  ImGui::ShowDemoWindow();
}

static void imgui_gui(const Camera& cam) {
  for(Material& material : materials) {
    material.draw(cam, RenderPass::present_pass());
  }
}

static void imgui_window_event(const Window::Event& e) {
  switch(e.type) {
    case Window::Event::Type::Character:
      text_input += e.character;
      break;
  }
}

void imgui_module_init() {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGuiIO& io = ImGui::GetIO();

  ImGui::LoadIniSettingsFromDisk(io.IniFilename);

  { // Build font atlas (texture upload happens elsewhere)
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
  }

  { // Configure imgui input
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.KeyMap[ImGuiKey_Tab] = (int)Device::Button::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = (int)Device::Button::Left;
    io.KeyMap[ImGuiKey_RightArrow] = (int)Device::Button::Right;
    io.KeyMap[ImGuiKey_UpArrow] = (int)Device::Button::Up;
    io.KeyMap[ImGuiKey_DownArrow] = (int)Device::Button::Down;
    io.KeyMap[ImGuiKey_PageUp] = (int)Device::Button::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = (int)Device::Button::PageDown;
    io.KeyMap[ImGuiKey_Home] = (int)Device::Button::Home;
    io.KeyMap[ImGuiKey_End] = (int)Device::Button::End;
    io.KeyMap[ImGuiKey_Insert] = (int)Device::Button::Insert;
    io.KeyMap[ImGuiKey_Delete] = (int)Device::Button::Delete;
    io.KeyMap[ImGuiKey_Backspace] = (int)Device::Button::Backspace;
    io.KeyMap[ImGuiKey_Space] = (int)Device::Button::Space;
    io.KeyMap[ImGuiKey_Enter] = (int)Device::Button::Enter;
    io.KeyMap[ImGuiKey_Escape] = (int)Device::Button::Escape;
    io.KeyMap[ImGuiKey_KeyPadEnter] = (int)Device::Button::Enter;
    io.KeyMap[ImGuiKey_A] = (int)Device::Button::A;
    io.KeyMap[ImGuiKey_C] = (int)Device::Button::C;
    io.KeyMap[ImGuiKey_V] = (int)Device::Button::V;
    io.KeyMap[ImGuiKey_X] = (int)Device::Button::X;
    io.KeyMap[ImGuiKey_Y] = (int)Device::Button::Y;
    io.KeyMap[ImGuiKey_Z] = (int)Device::Button::Z;
  }

  ResourceLoading<Mesh>::add_loader(
    [](ResourceSlot& slot, Mesh::Intermediate& intermediate) {
      if(slot.ext != imgui_symbol) {
        return false;
      }

      intermediate.attributes.push(VertexAttribute {VK_FORMAT_R32G32_SFLOAT, VertexAttributeType::Position});
      intermediate.attributes.push(VertexAttribute {VK_FORMAT_R32G32_SFLOAT, VertexAttributeType::TexCoord});
      intermediate.attributes.push(VertexAttribute {VK_FORMAT_R8G8B8A8_UNORM, VertexAttributeType::Color});
      intermediate.vertices = Buffer(sizeof(ImDrawVert));

      return true;
    });

  ResourceLoading<Shader>::add_loader(
    [](ResourceSlot& slot, Shader::Intermediate& intermediate) {
      if(slot.ext != imgui_symbol) {
        return false;
      }

      String source;

      slot.ext = slot.parameter("stage");
      if(slot.ext == vert_symbol) {
        source =
          "layout(location = 0) in vec2 vposition;\n\
          layout(location = 1) in vec2 vtexcoords;\n\
          layout(location = 2) in vec4 vcolor;\n\
          layout(location = 0) out vec4 fcolor;\n\
          layout(location = 1) out vec2 ftexcoords;\n\
          void main() {\n\
            fcolor = vcolor;\n\
            ftexcoords = vtexcoords;\n\
            gl_Position = vec4(vposition * viewport_pixel_size.zw * 2.f - 1.f, 0, 1);\n\
          }\n";
      } else {
        source =
          "layout(location = 0) in vec4 fcolor; \n\
          layout(location = 1) in vec2 ftexcoords; \n\
          layout(location = 0) out vec4 ocolor;\n\
          layout(binding = 1) uniform sampler2D tex;\n\
          void main() {\n\
            ocolor = fcolor * texture(tex, ftexcoords).rrrr;\n\
          }\n";
      }

      slot.source_buffer = Buffer(source.begin(), source.size());
      return ResourceLoading<Shader>::load_internal(slot, intermediate);
    });

  ResourceLoading<Texture>::add_loader(
    [](ResourceSlot& slot, Texture::Intermediate& intermediate) {
      if(slot.ext != imgui_symbol) {
        return false;
      }

      ImGuiIO& io = ImGui::GetIO();
      unsigned char* pixels;
      int width, height;
      io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
      intermediate.binary = Buffer(pixels, width * height);
      intermediate.width = width;
      intermediate.height = height;
      intermediate.format = VK_FORMAT_R8_UNORM;

      return true;
    });

  // Dummy screen size for init frame
  io.DisplaySize.x = 128;
  io.DisplaySize.y = 128;
  ImGui::NewFrame();

  // Defer init because we need resources and task system up
  Engine::add_deferred_action(Engine::DeferredAction {
    [](void*) {
      font_tex = ".imgui";
      mesh = ".imgui";
      Engine::add_update(imgui_update);
      Engine::add_gui(imgui_gui);
      Engine::add_window_event(imgui_window_event);
    }});
}
