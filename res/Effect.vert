#version 410

// inïœêî.
layout(location=0) in vec3 vPosition;
layout(location=1) in vec2 vTexCoord;
layout(location=2) in vec4 vColor;

//	outïœêî.
layout(location=0) out vec2 outTexCoord;
layout(location=1) out vec4 outColor;

uniform mat4x4 matMVP;

void main() {
	outColor = vColor;
	outTexCoord = vTexCoord;
	gl_Position = matMVP * vec4(vPosition, 1.0);
}