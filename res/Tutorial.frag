#version 410

//	in変数.
layout(location=0) in vec4 inColor;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inWorldPosition;
layout(location=3) in vec3 inWorldNormal;

//	out変数.
out vec4 fragColor;

//	uniform変数.
uniform sampler2D colorsampler;

const int maxLightCount = 9;

// ライトデータ(点光源).
struct PointLight
{
	vec4 position;	//座標(ワールド座標系).
	vec4 color;		//明るさ.
};

// ライティングパラメータ.
layout(Std140) uniform LightData
{
	vec4 ambientColor;	// 環境光.
	PointLight light[maxLightCount];	//ライトのリスト.
}	lightData;

void main()
{
	fragColor = inColor * texture(colorsampler, inTexCoord);
	vec3 lightColor = lightData.ambientColor.rgb;
	for (int i = 0; i < maxLightCount; ++i) {
		vec3 lightVector = lightData.light[i].position.xyz - inWorldPosition;
		float lightPower = 1.0 / dot(lightVector, lightVector);
		float coSTheta = clamp(dot(inWorldNormal, normalize(lightVector)), 0, 1);
		lightColor += lightData.light[i].color.rgb * coSTheta * lightPower;
	}
	fragColor.rgb *= lightColor;
}