#version 150 core

uniform sampler2D diffuseTex;

uniform vec3 	lightDirection;
uniform float 	lightfov;
uniform vec3 	lightPos;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
	vec3 worldPos ;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	vec3 incident 		= normalize (lightPos - IN . worldPos);
	vec4 diffuse 		= IN.colour* texture(diffuseTex, IN.texCoord);

	if (degrees(acos(dot(-incident,lightDirection))) > lightfov) {
		gl_FragColor 	= vec4(0.1,0.1,0.1,1)*diffuse;
	} 
	else {
		gl_FragColor 	= diffuse;
	}
}	
