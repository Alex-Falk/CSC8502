#version 150
uniform sampler2D diffuseTex;
uniform vec4 terrainTint;
uniform vec4 shadeColour;
uniform vec4 snowColour;
uniform float offset;

in Vertex {
	vec2 texCoord;
	vec3 position;
} IN;

out vec4 FragColor;

void main(void){
	vec4 newColour = mix(texture(diffuseTex, IN.texCoord),terrainTint,0.8);
	newColour = mix (mix(shadeColour, newColour, 0.3),newColour,  IN.position.y/257 + 0.2f);
	if (IN.position.y > offset) {
		FragColor = mix(newColour, snowColour, IN.position.y/257 - 0.1f);
	} else {
		FragColor = newColour;
	}

}