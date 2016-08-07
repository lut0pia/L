#pragma once 

#define L_SHAREDUNIFORM_SIZE (sizeof(Matrix44f)+sizeof(Vector3f))
#define L_SHAREDUNIFORM_VIEWPROJ 0
#define L_SHAREDUNIFORM_EYE sizeof(Matrix44f)
#define L_SHAREDUNIFORM \
"layout (std140) uniform Shared {" \
"mat4 viewProj;" \
"vec3 eye;" \
"};"
