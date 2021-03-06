#version 410

layout(location=0) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D colorSampler;

void main()
{
	const float numShadeS = 3; // �e�̒i�K.
	fragColor = texture(colorSampler, inTexCoord);
	fragColor.rgb = ceil(fragColor.rgb * numShadeS) * (1 / numShadeS);
}