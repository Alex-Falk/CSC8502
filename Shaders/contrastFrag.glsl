#version 330 core
uniform sampler2D diffuseTex;

uniform float contrast;
uniform float brightness;

in Vertex {
    vec2 texCoord;
} IN ;

out vec4 FragColor;

vec4 brightnessConstrast(vec4 value, float brightness, float constrast) {
    return vec4((value.rgb - 0.5) * contrast + 0.5 + brightness, value.a);
}

void main()
{
    vec4 diffuse    = texture(diffuseTex,IN.texCoord);

    FragColor       = brightnessConstrast(diffuse,brightness,contrast);
} 