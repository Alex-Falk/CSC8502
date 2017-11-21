# version 150 core

// uniform mat4 modelMatrix;
// uniform mat4 viewMatrix;
// uniform mat4 projMatrix;
// uniform mat4 textureMatrix;

uniform sampler2D terrainTex;

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;


// out Vertex {
// 	vec3 vPos;
// } OUT;

void main(void) {
	//OUT.texCoord	= (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	// mat4 mvp 		= projMatrix * viewMatrix * modelMatrix;
	// gl_Position 	= mvp * vec4(position, 1.0);
	//gl_Position = mvp * vec4(position, 1.0);


	vec2 texCoord 	= position.xy;
	float height 	= texture2D(terrainTex,texCoord).a;
	gl_Position 	= vec4(position.x,position.y, height, 1.0);

}