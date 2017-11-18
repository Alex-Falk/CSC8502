# version 150 core

uniform sampler2D diffuseTex ;
uniform samplerCube cubeTex ;

uniform vec3 cameraPos ;
uniform vec4 lightColour[4];
uniform vec4 specColour[4];
uniform vec3 lightPos[4];
uniform float lightRadius[4];
uniform float offset;

in Vertex {
	//vec3 position;
	vec4 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 worldPos ;
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
	vec4 diffuse = vec4(0,0,1,1);
	if (IN.worldPos.y < offset + (IN.worldPos.y/257 * 2*(2*cos(IN.worldPos.x/10)+ 1*cos(IN.worldPos.z/10)))) {
		diffuse 		= texture (diffuseTex, IN.texCoord) * IN.colour ;
	} else {
		diffuse 		= mix(texture(diffuseTex, IN.texCoord) * IN.colour,vec4(1,1,1,1),0.5);
	}
	//vec4 diffuse = texture (diffuseTex, IN.texCoord) * IN.colour ;

	
	vec3 incident 		= normalize ( IN.worldPos - cameraPos );
	vec4 reflection 	= texture (cubeTex ,
							reflect(incident ,normalize(IN.normal)));

	vec4 newColour 		= vec4(0,0,0,1);
	for (int i = 0; i < 4; ++i)
	{	

		float dist 		= length ( lightPos[i] - IN.worldPos );
		float atten 	= 1.0 - clamp ( dist / lightRadius[i] , 0.2 , 1.0);

		newColour 		+= ( lightColour[i] * diffuse * atten )*( diffuse + reflection );
	}
	gl_FragColor = newColour * .5;
	gl_FragColor.a = gl_FragColor.a / 4.;
}
