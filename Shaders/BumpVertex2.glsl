# version 150 core
uniform mat4 modelMatrix ;
uniform mat4 viewMatrix ;
uniform mat4 projMatrix ;
uniform mat4 textureMatrix ;

in vec3 position ;
in vec3 normal ;
in vec3 tangent;
in vec2 texCoord ;

out Vertex {
	vec2 texCoord ;
	vec3 normal ;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos ;
	vec3 position;
	vec4 shadowProj;
} OUT;

void main ( void ) {
	mat3 normalMatrix	= transpose (inverse(mat3(modelMatrix)));
	OUT.texCoord		= (textureMatrix * vec4 (texCoord , 0.0 , 1.0)).xy ;

	OUT.normal 			= normalize (normalMatrix * normalize (normal));
	OUT.tangent 		= normalize ( normalMatrix * normalize ( tangent ));
	OUT.binormal 		= normalize ( normalMatrix * normalize ( cross ( normal , tangent )));

	OUT.worldPos 		= (modelMatrix * vec4 (position ,1)).xyz ;
	gl_Position 		= (projMatrix * viewMatrix * modelMatrix) *
							vec4(position , 1.0);
	OUT.position 		= position;
	OUT.shadowProj		= ( textureMatrix * vec4 ( position + ( normal *1.5) ,1));
}