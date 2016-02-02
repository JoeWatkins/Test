#version 330 core

uniform sampler2D smileyT;
uniform sampler2D noiseT;
uniform float change;


in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void)	{	
vec4 smileyColour = texture(smileyT, IN.texCoord);
vec4 noiseColour = texture(noiseT,IN.texCoord);



gl_FragColor = (smileyColour * change) +  (noiseColour * (1.0 - change));




}