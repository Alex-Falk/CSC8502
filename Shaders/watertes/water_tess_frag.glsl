#version 150 core

uniform sampler2D waterTex;
uniform sampler2D waterBumpTex;
uniform samplerCube cubeTex;
uniform vec3 cameraPos;

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

	//vec4 lightColour 	 	= vec4(1,1,1,1);
	//vec4 specColour 	 	= vec4(1,1,1,1);
	//vec3 lightPos			= vec3(0,400,0);
	//float lightRadius 		= 10000;
	float sFac 				= 5000.0f;
	vec4 diffuse			= texture(waterTex,IN.texCoord);
	vec4 bump 				= texture(waterBumpTex,IN.texCoord);

	mat3 TBN	 		= mat3(IN.tangent, IN.binormal, IN.normal);

	vec3 normal 			= normalize(TBN * bump.rgb * 2.0 - 1.0);

	fragColor[0] = diffuse;//texture(terrainTex, IN.texCoord);
	fragColor[1] = vec4(normal.xyz*0.5 + 0.5, 1.0);
	//fragColor.a = 0.98*fragColor.a;
}