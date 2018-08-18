# Modules
Modules, found in `src/module`, are subparts of the engine that are not relied on by the core engine, therefore they do not constitute dependencies (although without any module the engine is unable to load any type of resource).

## Loaders
Loaders are modules that register functions to the engine for it to be able to create resources from files or even just a single url. Here's a list of asset loaders already included:
- stb_image
- stb_truetype
- stb_vorbis
- JSON
- Wavefront OBJ
- Midi
- WAVE
