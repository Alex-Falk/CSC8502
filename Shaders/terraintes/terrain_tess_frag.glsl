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

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec3 position;
} IN;

out vec4 gl_FragColor;

void main(void) {

	float sFac 				= 33.0f;
	vec4 rock 				= texture2D(rockTex,IN.texCoord);
	vec4 grass 				= texture2D(grassTex,IN.texCoord);
	vec4 bump 				= texture2D(rockBumpTex,IN.texCoord);
	float mixratio 			= IN.normal.y;

	
	vec4 diffuse 		= mix(rock,mix(grass,vec4(0,0,0,1),0.5),mixratio);

	mat3 TBN	 		= mat3(IN.tangent, IN.binormal, IN.normal);

	vec3 normal 		= normalize(TBN * bump.rgb * 2.0 - 1.0);
	
	vec3 incident 		= normalize(lightPos - IN.worldPos);

	float lambert 		= max (0.0 , dot(incident,normal));

	float dist 			= length(lightPos - IN.worldPos);
	float atten 		= 1.0 - clamp (dist / lightRadius, 0.0, 1.0);

	vec3 viewDir 		= normalize (cameraPos - IN.worldPos);
	vec3 halfDir 		= normalize (incident + viewDir);

	float rFactor 		= max (0.0, dot (halfDir, normal));
	float sFactor 		= pow (rFactor, sFac);

	vec3 colour 	= (diffuse.rgb * lightColour.rgb);
	colour 			+= (specColour.rgb * sFactor) * 0.1;
	
	vec4 newColour 	= vec4 (colour * atten * lambert , diffuse.a);
	newColour.rgb	+= (diffuse.rgb * lightColour.rgb);


	gl_FragColor = newColour;//texture(terrainTex, IN.texCoord);
}