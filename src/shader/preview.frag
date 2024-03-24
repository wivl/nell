#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frame;

vec3 gamma_correction(vec3 c){
    return pow(c, vec3(1.0 / 2.2));
}

void main()
{
    vec3 col = texture(frame, TexCoords).rgb;
//    col = gamma_correction(col);
    FragColor = vec4(col, 1.0);
}