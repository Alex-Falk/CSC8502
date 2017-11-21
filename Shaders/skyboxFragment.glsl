# version 150 core

uniform samplerCube cubeTex ;
uniform vec3 cameraPos ;

in Vertex {
	vec3 normal ;
} IN ;

out vec4 fragColor ;

void main ( void ) {
	fragColor = texture ( cubeTex , normalize ( IN . normal ));
}
