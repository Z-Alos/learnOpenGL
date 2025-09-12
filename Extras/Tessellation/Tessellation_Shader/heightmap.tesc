#version 460 core
layout (vertices=4) out; // patch

in vec2 TexCoord[];
out vec2 TextureCoord[];

void main () {
    // pass gl_Position & TexCoord
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = 16;
        gl_TessLevelOuter[1] = 16;
        gl_TessLevelOuter[2] = 16;
        gl_TessLevelOuter[3]  = 16;

        gl_TessLevelInner[0] = 16;
        gl_TessLevelInner[1] = 16;
    }
}
