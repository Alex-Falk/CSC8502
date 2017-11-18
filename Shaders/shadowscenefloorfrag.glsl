# version 150 core

uniform sampler2D 			diffuseTex;
uniform sampler2D 			bumpTex;
uniform sampler2DShadow 	shadowTex;

uniform vec4 	lightColour;
uniform vec3 	lightPos;
uniform vec3 	cameraPos;
uniform float 	lightRadius;
uniform vec3 	lightDirection;
uniform float 	lightfov;

in Vertex {
	vec3 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 tangent ;
	vec3 binormal ;
	vec3 worldPos ;
	vec4 shadowProj ; // New !
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
	mat3 TBN 			= mat3 ( IN . tangent , IN . binormal , IN . normal );
	vec3 normal 		= normalize ( TBN *
							( texture2D ( bumpTex , IN . texCoord ). rgb * 2.0 - 1.0));

	vec4 diffuse 		= texture2D ( diffuseTex , IN . texCoord );

	vec3 incident 		= normalize (lightPos - IN . worldPos);
	float lambert 		= max (0.0 , dot ( incident , normal ));

	float dist 			= length ( lightPos - IN . worldPos );
	float atten 		= 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);

	float fragangle 	= degrees(acos(dot(-incident,lightDirection)));

	if (fragangle > lightfov -1 && fragangle < lightfov + 1) {
		atten			*= 1.0-((fragangle-lightfov+1) / 2.0);
	} else if (fragangle > lightfov - 1) {
		atten 			= 0;
	}

	vec3 viewDir 		= normalize ( cameraPos - IN . worldPos );
	vec3 halfDir 		= normalize ( incident + viewDir );

	float rFactor 		= max (0.0 , dot ( halfDir , normal));
	float sFactor 		= pow (rFactor, 2000.0f);

	float shadow 		= 1.0; // New !
	
	if( IN . shadowProj . w > 0) { // New !
		shadow 			= textureProj ( shadowTex , IN . shadowProj );
	}
	
	lambert 			*= shadow ; // New !
	
	vec3 colour 		= ( diffuse . rgb * lightColour . rgb );
	colour 				+= ( lightColour . rgb * sFactor ) * 0.33;
	gl_FragColor 		= vec4 ( colour * atten * lambert , diffuse . a );
	gl_FragColor . rgb 	+= ( diffuse . rgb * lightColour . rgb ) * 0.1;

	vec3 basePos 	= vec3(lightPos.x,IN.worldPos.y,lightPos.z);
	vec3 H 			= basePos-lightPos;
	vec3 h 			= normalize(H);
	vec3 v 			= IN.worldPos-cameraPos;
	vec3 w 			= cameraPos - lightPos;
	float m			= tan(radians(lightfov))*tan(radians(lightfov));

	float a 		= dot(v,v) - m*dot(v,h)*dot(v,h) - dot(v,h)*dot(v,h);
	float b 		= 2 * ((dot(v,w) - m*dot(v,h)*dot(w,h) - dot(v,h)*dot(w,h)));
	float c 		= dot(w,w) - m*dot(w,h)*dot(w,h) - dot(w,h)*dot(w,h);

	float det 		= b*b - 4*a*c;

	if ( det > 0 ) {
		float t		= (- b - sqrt(det)) / (2 * a);
		vec3 Lint 	= cameraPos + (t*v);

		if (Lint.y > 0 && Lint.y < lightPos.y) {
			gl_FragColor	+= vec4(0.1,0.1,0.1,1);
		}
	}
}