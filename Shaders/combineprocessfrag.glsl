#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D sobelTex;
uniform sampler2D contrastTex;
uniform bool isSobel;
uniform bool isContrast;

uniform float contrast;

in Vertex {
	vec2 texCoord;
} IN ;

out vec4 FragColor;




void main()
{
	vec4 diffuse 		= texture(diffuseTex,IN.texCoord);
	vec4 sobel 			= vec4(0,0,0,0);
	vec4 contrast 		= vec4(0,0,0,0);

	if (isSobel) {
		sobel 			= texture(sobelTex,IN.texCoord)-diffuse;
	}
	if (isContrast) {
		contrast 		= texture(contrastTex,IN.texCoord)-diffuse;
	}

	FragColor 			= contrast+sobel+diffuse;
} 