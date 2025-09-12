#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D tex;

void main () {
    vec3 texColor = texture(tex, TexCoords).rgb;
    FragColor = vec4(texColor, 1.0);
}
