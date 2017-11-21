#version 150 core

uniform sampler2D diffuseTex;


in Vertex	{
	vec4 colour;
	vec2 texCoord;
	vec3 worldPos ;
} IN;

out vec4 fragColor;

void main(void)	{
	vec4 diffuse 		= texture2D(diffuseTex, IN.texCoord);

	fragColor 		= diffuse;
}	
