#pragma once

// General definitions
#include "constants.h"      // Constant values
#include "macros.h"         // Useful macros
#include "objects.h"        // Copy/swap objects
#include "types.h"          // Useful typedefs

// General classes
#include "Interface.h"      // Helping to go from a format to another

// Artifical Intelligence
#include "ai/MLP.h"
#include "ai/Perceptron.h"

// Audio I/O, Wave & Midi
#include "audio/Midi.h"

// Objects that hold other objects
#include "containers/Array.h"
#include "containers/BTree.h"
#include "containers/IntervalTree.h"
#include "containers/KeyValue.h"
#include "containers/List.h"
#include "containers/Map.h"
#include "containers/MultiArray.h"
#include "containers/Pool.h"
#include "containers/Queue.h"
#include "containers/Ref.h"
#include "containers/Set.h"
#include "containers/SortedArray.h"
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
#include "engine/Resource.h"
#include "engine/RigidBody.h"
#include "engine/ScriptComponent.h"
#include "engine/Settings.h"
#include "engine/Sprite.h"
#include "engine/SpriteAnimator.h"
#include "engine/StaticMesh.h"
#include "engine/Transform.h"

// Font handling
#include "font/Font.h"
#include "font/PixelFont.h"

// Everything related to OpenGL
#include "gl/Atlas.h"
#include "gl/Buffer.h"
#include "gl/FrameBuffer.h"
#include "gl/GL.h"
#include "gl/Mesh.h"
#include "gl/MeshBuilder.h"
#include "gl/Program.h"
#include "gl/Shader.h"
#include "gl/Texture.h"

// Images and colors
#include "image/Bitmap.h"
#include "image/Color.h"

// Mathematical tools
#include "math/digits.h"
#include "math/dither.h"
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
#include "parallelism/Lock.h"
#include "parallelism/Mutex.h"
#include "parallelism/TaskSystem.h"

// Scripting language
#include "script/Context.h"
#include "script/Lexer.h"

// C-Stream wrapper and friends
#include "streams/BufferStream.h"
#include "streams/CFileStream.h"
#include "streams/FileStream.h"
#include "streams/NetStream.h"
#include "streams/Stream.h"

// Filesystem, Window
#include "system/Device.h"
#include "system/File.h"
#include "system/System.h"
#include "system/Window.h"

// Text
#include "text/encoding.h"
#include "text/String.h"
#include "text/Symbol.h"

// Timey-wimey wibbly-wobbly stuff
#include "time/ScopedTimer.h"
#include "time/Time.h"
#include "time/Timer.h"
