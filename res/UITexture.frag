#version 410

layout(location=0) in vec2 inTexCoord;
layout(location=1) in vec4 inColor;

out vec4 fragColor;

uniform sampler2D colorSampler;

void main()
{
	fragColor = texture(colorSampler, inTexCoord) * inColor;
}