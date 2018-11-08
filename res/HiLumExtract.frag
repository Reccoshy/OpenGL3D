#version 410

layout(location=0) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D colorSampler;

// �|�X�g�G�t�F�N�g�f�[�^.
layout(Std140) uniform PoStEffectData {
	mat4x4 matColor; // �F�ϊ��s��.
	float luminanceScale; // �P�x�����W��.
	float bloomThreShould; // �u���[���𔭐������邵�����l.
} poStEffect;

/*
�P�x���v�Z����.

@param rgb RGB�J���[.

@return rgb��ITU=BT.2020�ɂ��������ĕϊ������P�x�̑ΐ�.
*/
float luminance(vec3 rgb)
{
	return log(dot(rgb, vec3(0.2627, 0.678, 0.0593)) + 0.0001);
}

void main()
{
	vec3 threShould = vec3(poStEffect.bloomThreShould);
	vec4 tS;
	tS.xy = vec2(1.0) / vec2(textureSize(colorSampler, 0));
	tS.zw = -tS.xy;

	vec3 rgb;
	rgb = texture(colorSampler, inTexCoord + tS.xy).rgb;
	fragColor.a = luminance(rgb);
	fragColor.rgb = max(rgb, threShould) - threShould;

	rgb = texture(colorSampler, inTexCoord + tS.zy).rgb;
	fragColor.a += luminance(rgb);
	fragColor.rgb += max(rgb, threShould) - threShould;
	
	rgb = texture(colorSampler, inTexCoord + tS.xw).rgb;
	fragColor.a += luminance(rgb);
	fragColor.rgb += max(rgb, threShould) - threShould;
	
	rgb = texture(colorSampler, inTexCoord + tS.zw).rgb;
	fragColor.a += luminance(rgb);
	fragColor.rgb += max(rgb, threShould) - threShould;
	fragColor *= 1.0 / 4.0;
}