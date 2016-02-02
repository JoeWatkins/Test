#version 150 core


uniform float scale;

in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void)	{	
	gl_FragColor = IN.colour;
	gl_FragColor.a -= 1 * (scale/100);
}