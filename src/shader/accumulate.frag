#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D currentFrame;
uniform sampler2D accFrame;
uniform int loop_count;



void main() {
    vec4 currentColor = texture(currentFrame, TexCoords);
    vec4 accColor = texture(accFrame, TexCoords);
//    FragColor = mix(currentColor, accColor, mix_part);
    FragColor = (accColor * loop_count + currentColor) / (loop_count + 1);
}