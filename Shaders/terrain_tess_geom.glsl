#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in Vertex {
	vec2 texCoord;
} IN[];

out Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main() {
	vec3 A = gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz;
	vec3 B = gl_in[1].gl_Position.xyz-gl_in[0].gl_Position.xyz;

	mat3 normalMatrix	= transpose (inverse(mat3(modelMatrix)));
	vec3 normal 		= normalize (cross(A,B));
	vec3 tangent		= normalize (A);

	OUT.normal 			= normal;
	OUT.tangent 		= tangent;
	OUT.binormal 		= normalize(cross(normal,tangent));


	OUT.texCoord = IN[0].texCoord;
	gl_Position = gl_in[0].gl_Position;
	OUT.worldPos 		= (modelMatrix * gl_in[0].gl_Position).xyz ;
	EmitVertex();

	OUT.texCoord = IN[1].texCoord;
	gl_Position = gl_in[1].gl_Position;
	OUT.worldPos 		= (modelMatrix * gl_in[1].gl_Position).xyz ;
	EmitVertex();

	OUT.texCoord = IN[2].texCoord;
	gl_Position = gl_in[2].gl_Position;
	OUT.worldPos 		= (modelMatrix * gl_in[2].gl_Position).xyz ;
	EmitVertex();


	EndPrimitive();
}