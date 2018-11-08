#version 400 core

layout(location=0) in vec2 inTexCoord;

out vec4 out_Color;

uniform sampler2D backGroundTexture;
uniform sampler2D rTexture;
uniform sampler2D gTexture;
uniform sampler2D bTexture;
uniform sampler2D blendMap;

void main (){

	vec4 blendMapColour = texture(blendMap, inTexCoord);

	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = inTexCoord * 100.0;
	vec4 backgroundTextureColour = texture(backGroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTextureColour = texture(rTexture, tiledCoords) * blendMapColour.r;
	vec4 gTextureColour = texture(gTexture, tiledCoords) * blendMapColour.g;
	vec4 bTextureColour = texture(bTexture, tiledCoords) * blendMapColour.b;

	vec4 totalColour = backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour;

	out_Color = totalColour;
	
//	out_Color = texture(bTexture, inTexCoord);
}