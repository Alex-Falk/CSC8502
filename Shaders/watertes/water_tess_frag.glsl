#version 150 core

uniform sampler2D waterTex;
uniform sampler2D waterBumpTex;
uniform samplerCube cubeTex;
uniform vec3 cameraPos;

uniform vec4 lightColour;
uniform vec4 specColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec3 position;
} IN;

out vec4 fragColor;

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

	vec3 incident 		= normalize(lightPos - IN.worldPos);

	vec4 reflection 	= texture (cubeTex ,
							reflect(incident ,normalize(normal)));

	float lambert 		= max (0.0 , dot(incident,normal));

	float dist 			= length(lightPos - IN.worldPos);
	float atten 		= 1.0 - clamp (dist / lightRadius, 0.0, 1.0);

	vec3 viewDir 		= normalize (cameraPos - IN.worldPos);
	vec3 halfDir 		= normalize (incident + viewDir);

	float rFactor 		= max (0.0, dot (halfDir, normal));
	float sFactor 		= pow (rFactor, sFac);

	vec3 colour 	= (diffuse.rgb * lightColour.rgb);
	colour 			+= (specColour.rgb * sFactor) * 0.1;
	
	vec4 newColour 	= vec4 (colour * atten * lambert , diffuse.a)*(diffuse+reflection);
	newColour.rgb	+= (diffuse.rgb * lightColour.rgb);


	fragColor = newColour*0.5;//texture(terrainTex, IN.texCoord);
	fragColor.a = 0.98*fragColor.a;
}