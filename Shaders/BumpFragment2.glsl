#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;
uniform sampler2D bumpTex;
uniform sampler2D bumpTex2;
uniform sampler2DShadow shadowTex;

uniform vec3 cameraPos ;
uniform vec4 lightColour[4];
uniform vec4 specColour[4];
uniform vec3 lightPos[4];
uniform float lightRadius[4];
uniform vec4 terrainTint;
uniform vec4 shadeColour;
uniform vec4 snowColour;
uniform float offset;

in Vertex {
	vec2 texCoord ;
	vec3 normal ;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos ;
	vec3 position;
	vec4 shadowProj;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
	gl_FragColor	= vec4(0,0,0,1);
	vec3 normal 	= vec3(0,0,0);
	float sFac 		= 0;

	vec4 diffuse 	= vec4(0,0,0,1);
		if (IN.position.y <= 200 ) {//+ 2*(6*cos(IN.position.x/20)+ 4*cos(IN.position.z/20))) {
			diffuse  	= texture(diffuseTex2, IN.texCoord);
			mat3 TBN	= mat3(IN.tangent, IN.binormal, IN.normal);
			normal 		= normalize (TBN * (texture(bumpTex2, IN.texCoord).rgb * 2.0 - 1.0));
		} else if (IN.position.y <= 300) { 
			float ratio	= ((IN.position.y)-200.0) / 100.0;
			mat3 TBN	= mat3(IN.tangent, IN.binormal, IN.normal);
			diffuse 	= mix(texture(diffuseTex2,IN.texCoord),texture(diffuseTex,IN.texCoord),ratio);
			vec4 bumpMix= mix(texture(bumpTex2,IN.texCoord),texture(bumpTex,IN.texCoord),ratio);
			normal 		= normalize (TBN * (bumpMix.rgb * 2.0 - 1.0));
		} else {
			diffuse 	= texture(diffuseTex, IN.texCoord);
			mat3 TBN	= mat3(IN.tangent, IN.binormal, IN.normal);
			normal 		= normalize (TBN * (texture(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
			
		}

		if (IN.position.y > offset + (IN.position.y/1080 * 2*(6*cos(IN.position.x/20)+ 4*cos(IN.position.z/20)))) {
			diffuse 		= mix(diffuse, snowColour, 0.7);
			normal			= IN.normal;
			sFac 			= 33.0;
		} 		
		else {
			sFac 			= 10.0;
		}

		if (terrainTint.a != 0) {
			diffuse 		= mix(diffuse,terrainTint,0.8);
		}		


	for (int i = 1; i < 2; ++i) {

		vec3 incident 		= normalize(lightPos[i] - IN.worldPos);
		float lambert 		= max (0.0 , dot(incident,normal));

		float dist 			= length(lightPos[i] - IN.worldPos);
		float atten 		= 1.0 - clamp (dist / lightRadius[i], 0.0, 1.0);

		vec3 viewDir 		= normalize (cameraPos - IN.worldPos);
		vec3 halfDir 		= normalize (incident + viewDir);

		float rFactor 		= max (0.0, dot (halfDir, normal));
		float sFactor 		= pow (rFactor, sFac);

		float shadow 		= 1.0;

		if (IN.shadowProj.w > 0.0) {
			shadow 			= textureProj(shadowTex,IN.shadowProj);
		}

		lambert 			*= shadow;

		vec3 colour 		= (diffuse.rgb * lightColour[i].rgb);
		colour 				+= (specColour[i].rgb * sFactor) * 0.1;

		vec4 newColour 		= vec4 (colour * atten * lambert , diffuse.a);
		newColour.rgb		+= (diffuse.rgb * lightColour[i].rgb) * 0.025;

		if (shadow == 0) {
			gl_FragColor 	= vec4(1,0,0,1);
		}
		gl_FragColor 		+= newColour;
	}

	//gl_FragColor *= vec4(1.2,1.2,1.2,1);

}