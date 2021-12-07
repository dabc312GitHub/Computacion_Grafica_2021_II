#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform vec4 ourColorGlobal;

void main()
{
    FragColor = texture(texture1, TexCoord) * ourColorGlobal;
}