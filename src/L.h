#pragma once

// General definitions
#include "constants.h"      // Constant values
#include "macros.h"         // Useful macros
#include "objects.h"        // Construct/destruct/copy/move/swap objects
#include "types.h"          // Useful typedefs

// General classes
#include "Interface.h"      // Helping to go from a format to another
#include "String.h"         // Mandatory string class

// Artifical Intelligence
#include "ai/MLP.h"
#include "ai/Perceptron.h"

// Audio I/O, Wave & Midi
#include "audio/Midi.h"

// Byte to byte functions, encoding
#include "bytes/BWT.h"
#include "bytes/encoding.h"
#include "bytes/Huffman.h"
#include "bytes/MTF.h"

// Objects that hold other objects
#include "containers/Array.h"
#include "containers/BTree.h"
#include "containers/IntervalTree.h"
#include "containers/KeyValue.h"
#include "containers/List.h"
#include "containers/Map.h"
#include "containers/MultiArray.h"
#include "containers/Pool.h"
#include "containers/Ref.h"
#include "containers/Set.h"
#include "containers/SortedArray.h"
#include "containers/StaticRing.h"
#include "containers/StaticStack.h"
#include "containers/Table.h"
#include "containers/Tree.h"
#include "containers/XML.h"

// Dynamic typing/casting/compiling
#include "dynamic/Assembly.h"
#include "dynamic/Type.h"
#include "dynamic/Variable.h"

// Entity and components
#include "engine/Camera.h"
#include "engine/Collider.h"
#include "engine/Component.h"
#include "engine/Engine.h"
#include "engine/Entity.h"
#include "engine/Primitive.h"
#include "engine/RigidBody.h"
#include "engine/ScriptComponent.h"
#include "engine/Sprite.h"
#include "engine/SpriteAnimator.h"
#include "engine/StaticMesh.h"
#include "engine/Transform.h"

// Font handling
#include "font/Font.h"
#include "font/FontBase.h"
#include "font/FontBitmap.h"
#include "font/Pixel.h"

// Everything related to OpenGL
#include "gl/Buffer.h"
#include "gl/FrameBuffer.h"
#include "gl/GL.h"
#include "gl/Mesh.h"
#include "gl/MeshBuilder.h"
#include "gl/PostProcess.h"
#include "gl/Program.h"
#include "gl/RenderBuffer.h"
#include "gl/Shader.h"
#include "gl/Texture.h"

// 2D graphical user interface
#include "gui/ActionListener.h"
#include "gui/Background.h"
#include "gui/Base.h"
#include "gui/Border.h"
#include "gui/GridContainer.h"
#include "gui/GUI.h"
#include "gui/Image.h"
#include "gui/Layer.h"
#include "gui/Line.h"
#include "gui/ListContainer.h"
#include "gui/Rectangle.h"
#include "gui/RelativeContainer.h"
#include "gui/Scrollable.h"
#include "gui/Sizable.h"
#include "gui/Slider.h"
#include "gui/Solid.h"
#include "gui/Text.h"
#include "gui/TextInput.h"

// Images and colors
#include "image/Bitmap.h"
#include "image/Color.h"

// Mathematical tools
#include "math/Fixed.h"
#include "math/geometry.h"
#include "math/Integer.h"
#include "math/Interpolation.h"
#include "math/Interval.h"
#include "math/math.h"
#include "math/Matrix.h"
#include "math/Perlin.h"
#include "math/Quaternion.h"
#include "math/Rand.h"
#include "math/Rational.h"
#include "math/Real.h"
#include "math/Vector.h"

// Network tools
#include "network/Network.h"

// Threads, coroutines, fibers and synchronization
#include "parallelism/Coroutine.h"
#include "parallelism/Fiber.h"
#include "parallelism/Mutex.h"
#include "parallelism/Semaphore.h"
#include "parallelism/Thread.h"

// Scripting language
#include "script/Context.h"
#include "script/Lexer.h"

// C-Stream wrapper and friends
#include "streams/FileStream.h"
#include "streams/NetStream.h"
#include "streams/Stream.h"

// Filesystem, Window
#include "system/File.h"
#include "system/System.h"
#include "system/Window.h"

// Timey-wimey wibbly-wobbly stuff
#include "time/Time.h"
#include "time/Timer.h"
