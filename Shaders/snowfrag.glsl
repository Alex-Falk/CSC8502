#version 150 core

uniform sampler2D diffuseTex;


in Vertex	{
	vec4 colour;
	vec2 texCoord;
	vec3 worldPos ;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	vec4 diffuse 		= IN.colour * texture(diffuseTex, IN.texCoord);

	gl_FragColor 		= diffuse;
}	
