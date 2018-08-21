# Modules
Modules, found in `src/module`, are subparts of the engine that are not relied on by the core engine, therefore they do not constitute dependencies (although without any module the engine is unable to load any type of resource).

## Loaders
Loaders are modules that register functions to the engine for it to be able to create resources from files or even just a single url. Here's a list of asset loaders already included:
- `blue_noise`: Uses the void and cluster algorithm to generate blue noise textures.
- `glsl`: Compile .vert and .frag files with `glslangValidator`.
- `inline_pipeline`: Allows the creation of a pipeline from a single url.
- `lon`: Create a material from an .ls script (should be merged with ls)
- `ls`: Compile .ls files into scripts
- `mid`: Load .mid and .midi files into a MidiSequence (deprecated)
- `pixel_font`: Very simple debug font
- `stb_image`: Load .jpeg, .jpg, .png and .bmp files into a Texture
- `stb_truetype`: Load .ttf files into a Font
- `stb_vorbis`: Load .ogg files into an AudioStream
- `wav`: Load .wav file into an AudioStream
- `wavefront_obj`: Load .obj files into a Mesh
