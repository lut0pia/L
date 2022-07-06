add_project(
  sample
  SCRIPT_INIT_FILE ini.ls
  # This project purposefully depends on a lot of modules because it tries to showcase as many of them as possible
  MOD_DEPENDENCIES
    alsa # Audio on Linux
    assimp # .glb files
    audio_win # Audio on Windows
    glslang
    imgui_console
    imgui_input
    imgui_resources
    lasm # script/proxy.lasm
    ls # Most scripts
    lz # Custom compression algorithm
    mesh_format
    mid # audio/mozart.mid
    midi_win # Playing MIDI events on Windows
    pixel_font # For the frame counter
    rawinput
    script_material
    script_mesh # mesh/*.ls
    script_optimize
    script_texture
    shader_reflect
    stb_dxt
    stb_image # texture/*.png
    stb_perlin # Terrain primitive
    stb_truetype
    stb_vorbis # audio/bach.ogg
    wav # audio/guitar.wav
    wavefront_obj # mesh/*.obj
    win32
    xinput_unix
    xinput_win
    xlib
    zlib

    # OpenGL and Vulkan have optional dependencies on win32 and xlib, for now it's better to put it at the end
    # TODO: later optional dependencies should be re-evaluated at the end
    opengl
    vulkan
)
