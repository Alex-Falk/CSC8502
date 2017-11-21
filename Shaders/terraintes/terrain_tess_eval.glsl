#version 400 core

layout(quads) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform float heightfrac;
uniform float npatches;
uniform float yscale;


uniform sampler2D terrainTex;

// in Vertex {
// 	vec3 tcPos;
// } IN[];

out Vertex {
	vec2 texCoord;
	vec3 worldPos;
	vec3 position;
} OUT;

void main(void) {

	float u 			= gl_TessCoord.x;
	float v 			= gl_TessCoord.y;

	vec4 a 				= mix(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_TessCoord.x);
	vec4 b				= mix(gl_in[2].gl_Position,gl_in[3].gl_Position,gl_TessCoord.x);


	vec4 pos 			= mix(a,b,gl_TessCoord.y);
	vec2 texCoord 		= (pos.xy+1)/2.0;
	OUT.texCoord 		= texCoord*2;

	float height 		= heightfrac*-yscale*texture(terrainTex, texCoord/npatches).g;

	vec3 position 		= vec3(texCoord, height);

	mat4 mvp 			= projMatrix * viewMatrix * modelMatrix;
	gl_Position 		= mvp * vec4(position, 1.0);

	OUT.position 		= position;
	OUT.worldPos 		= (modelMatrix * vec4(position, 1.0)).xyz ;
}