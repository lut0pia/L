#pragma once 

#define L_SHAREDUNIFORM_SIZE (sizeof(Matrix44f)*2+sizeof(Vector3f))
#define L_SHAREDUNIFORM_VIEWPROJ 0
#define L_SHAREDUNIFORM_INVVIEWPROJ (L_SHAREDUNIFORM_VIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM_EYE (L_SHAREDUNIFORM_INVVIEWPROJ+sizeof(Matrix44f))
#define L_SHAREDUNIFORM \
"layout (std140) uniform Shared {" \
"mat4 viewProj, invViewProj;" \
"vec3 eye;" \
"};"
