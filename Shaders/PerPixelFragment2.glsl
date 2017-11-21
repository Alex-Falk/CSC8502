#version 150 core

uniform sampler2D diffuseTex ;
uniform samplerCube cubeTex ;

uniform vec3 cameraPos ;

in Vertex {
	vec3 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 worldPos ;
} IN ;

out vec4 fragColor ;

void main ( void ) {
	fragColor = vec4(0,0,0,1);
	vec4 lightColour 	 	= vec4(1,1,1,1);
	vec4 specColour 	 	= vec4(1,1,1,1);
	vec3 lightPos			= vec3(0,5000,0);
	float lightRadius 		= 10000;

	vec4 diffuse = texture(diffuseTex,IN.texCoord);

	vec3 incident 		= normalize ( IN.worldPos - cameraPos );
	vec4 reflection 	= texture (cubeTex ,
							reflect(incident ,normalize(IN.normal)));

	float dist = length(lightPos - IN.worldPos );
	float atten = 1.0 - clamp (dist / lightRadius, 0.2, 1.0);

	fragColor += ( lightColour * diffuse * atten )*( diffuse + reflection );
	fragColor.a = fragColor.a * 0.8;

}