#version 410

layout(location=0) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D colorSampler;

void main()
{
	vec4 tS;
	tS.xy = vec2(1.0) / vec2(textureSize(colorSampler, 0));
	tS.zw = -tS.xy;
	fragColor = texture(colorSampler, inTexCoord + tS.xy);
	fragColor += texture(colorSampler, inTexCoord + tS.zy);
	fragColor += texture(colorSampler, inTexCoord + tS.xw);
	fragColor += texture(colorSampler, inTexCoord + tS.zw);
	fragColor *= 1.0 / 4.0;
}