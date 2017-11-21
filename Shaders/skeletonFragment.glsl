#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec4 	colour;
	vec2 	texCoord;
	vec3 	normal;
	vec3 	tangent;
	vec3 	worldPos;
} IN;

out vec4 fragColor;

void main(void){
	fragColor = texture(diffuseTex, IN.texCoord);
	
	if(fragColor.a <= 0)
	{
		discard;	
	}
}