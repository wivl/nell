#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D currentFrame;
uniform sampler2D accFrame;
uniform float mix_part;



void main() {
    vec4 currentColor = texture(currentFrame, TexCoords);
    vec4 accColor = texture(accFrame, TexCoords);
    FragColor = mix(currentColor, accColor, mix_part);
//    FragColor = currentColor;
}