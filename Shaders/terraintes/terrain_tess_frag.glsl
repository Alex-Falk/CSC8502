#version 150 core

uniform sampler2D rockTex;
uniform sampler2D grassTex;
uniform sampler2D terrainTex;
uniform sampler2D rockBumpTex;
uniform vec3 cameraPos;

uniform vec4 lightColour;
uniform vec4 specColour;
uniform vec3 lightPos;
uniform float lightRadius;

uniform float coolingRatio;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec3 position;
} IN;

out vec4 fragColor[2];

void main(void) {

	float sFac 				= 33.0f;
	vec4 rock 				= texture(rockTex,IN.texCoord);
	vec4 grass 				= texture(grassTex,IN.texCoord);
	vec4 bump 				= texture(rockBumpTex,IN.texCoord);
	float mixratio 			= IN.normal.y;

	if (coolingRatio > 0.0) {
		mixratio = clamp(mixratio + coolingRatio,0,1);
	}
	
	vec4 diffuse 		= mix(rock,mix(grass,vec4(0,0,0,1),0.5),mixratio);

	mat3 TBN	 		= mat3(IN.tangent, IN.binormal, IN.normal);

	vec3 normal 		= normalize(TBN * bump.rgb * 2.0 - 1.0);
	
	fragColor[0] = diffuse;//texture(terrainTex, IN.texCoord);
	fragColor[1] = vec4(normal.xyz*0.5 + 0.5, 1.0);
}