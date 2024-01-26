#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frame;


void main()
{
    vec3 col = texture(frame, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
}