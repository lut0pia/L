# Modules
Modules, found in `src/module`, are subparts of the engine that are not relied on by the core engine, therefore they do not constitute hard dependencies (although without any module the engine is unable to load any type of resource).

## Loaders
Modules can register loading functions to the resource mechanism to create resources from files or even just a single url. Here's a list of such modules already included:
- `blue_noise`: Generate blue noise textures using the void and cluster algorithm
- `glsl`: Load shaders from `.vert` and `.frag` files using `glslang`
- `gltf`: Load materials, meshes, textures and shaders from `.glb` files
- `lasm`: Load scripts from `.lasm` files
- `ls`: Load scripts from `.ls` files
- `mid`: Load midi sequences from `.mid` and `.midi` files
- `pixel_font`: Generate a very simple debug font
- `script_material`: Load materials from any script file
- `stb_image`: Load textures from `.jpeg`, `.jpg`, `.png` and `.bmp` files
- `stb_perlin`: Generate perlin noise textures
- `stb_truetype`: Load fonts from `.ttf` files
- `stb_vorbis`: Load audio streams from `.ogg` files
- `wav`: Load audio streams from `.wav` files
- `wavefront_obj`: Load meshes from `.obj` files

## Transformers
Modules can register transform functions to the resource mechanism to transform resources after being loaded. Here's a list of such modules already included:
- `stb_dxt`: Compress textures in BC1 or BC3
- `mesh_format`: Modify mesh vertex format and compute normals and tangents if necessary

## Interfaces
Modules can register objects or mechanisms to interact with an external system. Here's a list of such modules already included:
- `alsa`: Render audio to ALSA
- `audio_win`: Render audio to WASAPI
- `joystick_unix`: Register `/dev/input` joysticks as devices
- `midi_win`: Play Midi events to Windows MidiOut
- `rawinput`: Register Windows HIDs as devices
- `window_win`: Open Windows windows
- `xinput_unix`: Register X XInput2 devices as devices
- `xinput_win`: Register Windows XInput devices as devices
- `xlib`: Open X windows
