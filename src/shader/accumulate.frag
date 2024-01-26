#version 410 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D currentFrame;
uniform sampler2D accFrame;
uniform int loop_count;



void main() {
    vec3 currentColor = texture(currentFrame, TexCoords).rgb;
    vec3 accColor = texture(accFrame, TexCoords).rgb;
//    FragColor = mix(currentColor, accColor, mix_part);
    vec3 color = (accColor * float(loop_count) + currentColor) / (float(loop_count) + 1.0);
    FragColor = vec4(color, 1.0);
}