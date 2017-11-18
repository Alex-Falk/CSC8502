#version 150 core

uniform sampler2D diffuseTex;

uniform vec3 	cameraPos;
uniform vec3	lightPos;
uniform vec3 	lightDirection;
uniform float 	lightfov;
uniform vec4 	lightColour;


in Vertex {
	vec2 texCoord;
	vec3 worldPos;
} IN ;

out vec4 FragColor;

void main(void) {
	FragColor 	= texture(diffuseTex, IN.texCoord);
	
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
			FragColor	+= vec4(0.1,0.1,0.1,1);
		}
	}



}