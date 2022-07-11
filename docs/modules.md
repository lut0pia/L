# Modules
Modules, found in the `mod` folder, are subparts of the engine that are not relied on by the core engine, therefore they do not constitute hard dependencies (although without any module the engine is unable to load any type of resource).

## Loaders
Modules can register loading functions to the resource mechanism to create resources from files or even just a single url. Here's a list of such modules already included:
- `assimp`: Load meshes, materials, scenes, textures and shaders using `assimp`
- `blue_noise`: Generate blue noise textures using the void and cluster algorithm
- `glslang`: Load shaders from `.vert` and `.frag` files using `glslang`
- `lasm`: Load scripts from `.lasm` files
- [`ls`](../mod/ls): Load scripts from `.ls` files
- `mid`: Load midi sequences from `.mid` and `.midi` files
- `pixel_font`: Generate a very simple debug font
- `script_material`: Load materials from any script file
- `script_mesh`: Generate meshes from any script file
- `script_texture`: Generate textures from any script file
- `stb_image`: Load textures from `.jpeg`, `.jpg`, `.png` and `.bmp` files
- `stb_perlin`: Generate perlin noise textures
- `stb_truetype`: Load fonts from `.ttf` files
- `stb_vorbis`: Load audio streams from `.ogg` files
- `wav`: Load audio streams from `.wav` files
- `wavefront_obj`: Load meshes from `.obj` files
- `yaml`: Load objects from `.yaml` files

## Transformers
Modules can register transform functions to the resource mechanism to transform resources after being loaded. Here's a list of such modules already included:
- `stb_dxt`: Compress textures in BC1 or BC3
- `mesh_format`: Modify mesh vertex format and compute normals and tangents if necessary
- `script_optimize`: Optimize script by using more complex opcodes
- `shader_reflect`: Read SPIR-V to deduce inputs and outputs of shaders

## Interfaces
Modules can register objects or mechanisms to interact with an external system. Here's a list of such modules already included:

### Audio Output
- `alsa`: Render audio to ALSA
- `audio_win`: Render audio to WASAPI

### Compression
- `lz`: Custom LZ compression
- `zlib`: zlib compression

### Game Platform
- `steamworks`: Steam

### Input
- `joystick_unix`: Register `/dev/input` joysticks as devices
- `rawinput`: Register Windows HIDs as devices
- `xinput_unix`: Register X XInput2 devices as devices
- `xinput_win`: Register Windows XInput devices as devices

### Midi
- `midi_win`: Play Midi events to Windows MidiOut

### Renderer
- `opengl`: Implement OpenGL renderer
- `vulkan`: Implement Vulkan renderer

### Window
- `win32`: Windows windows
- `xlib`: X windows

## Debug
- `imgui`: Dear ImGui integration
- `imgui_console`: Add a console for executing script (opens with Ctrl+F12)
- `imgui_game_platform`: Display game platform achievements
- `imgui_input`: Display input context stack and input values
- `imgui_resources`: Display resources along with their loading status
- `script_server`: Implement telnet server to execute script remotely
