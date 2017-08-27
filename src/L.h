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
#include "audio/Audio.h"
#include "audio/AudioBuffer.h"
#include "audio/AudioSource.h"
#include "audio/Midi.h"

// Components
#include "component/AudioListenerComponent.h"
#include "component/AudioSourceComponent.h"
#include "component/Camera.h"
#include "component/Collider.h"
#include "component/Component.h"
#include "component/Entity.h"
#include "component/HierarchyComponent.h"
#include "component/LightComponent.h"
#include "component/NameComponent.h"
#include "component/Primitive.h"
#include "component/RigidBody.h"
#include "component/ScriptComponent.h"
#include "component/Sprite.h"
#include "component/SpriteAnimator.h"
#include "component/StaticMesh.h"
#include "component/Transform.h"

// Objects that hold other objects
#include "containers/Array.h"
#include "containers/BTree.h"
#include "containers/Buffer.h"
#include "containers/IntervalTree.h"
#include "containers/KeyValue.h"
#include "containers/List.h"
#include "containers/Map.h"
#include "containers/MultiArray.h"
#include "containers/Pool.h"
#include "containers/Queue.h"
#include "containers/Raw.h"
#include "containers/Ref.h"
#include "containers/Set.h"
#include "containers/SortedArray.h"
#include "containers/StaticStack.h"
#include "containers/Table.h"
#include "containers/Tree.h"

// Dev tools
#ifdef L_DEBUG
#include "dev/ScriptServer.h"
#endif

// Dynamic typing/casting/compiling
#include "dynamic/Assembly.h"
#include "dynamic/Type.h"
#include "dynamic/Variable.h"

// Core engine systems
#include "engine/Engine.h"
#include "engine/Resource.h"
#include "engine/Settings.h"
#include "engine/SharedUniform.h"

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
#include "network/Server.h"

// Threads, coroutines, fibers and synchronization
#include "parallelism/Lock.h"
#include "parallelism/Mutex.h"
#include "parallelism/TaskSystem.h"

// Scripting language
#include "script/Context.h"
#include "script/Lexer.h"

// Streams, IO
#include "stream/AsyncFileStream.h"
#include "stream/BufferStream.h"
#include "stream/CFileStream.h"
#include "stream/DirectStream.h"
#include "stream/NetStream.h"
#include "stream/serial.h"
#include "stream/Stream.h"
#include "stream/StringStream.h"

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
#include "time/Date.h"
#include "time/ScopedTimer.h"
#include "time/Time.h"
#include "time/Timer.h"
