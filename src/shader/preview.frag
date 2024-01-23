#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenFrame;

void main()
{
    FragColor = texture(screenFrame, TexCoords);
//    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}