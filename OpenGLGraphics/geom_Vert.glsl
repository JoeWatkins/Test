# version 150 core
 
layout( triangles ) in;
layout ( triangle_strip , max_vertices = 12) out;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN[];
out Vertex {
	vec4 colour;
	vec2 texCoord;
} OUT;
void main() {
	for (int i = 0; i < gl_in.length(); ++i) {
		OUT.colour = IN[i].colour;
		// top right
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x += 1.0f;
		gl_Position.y += 1.0f;
		OUT.texCoord = vec2(1 ,0);
		EmitVertex();

		// top left
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x -= 1.0f;
		gl_Position.y += 1.0f;
		OUT.texCoord = vec2(0 ,0);
		EmitVertex();

		// bottom right
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x += 1.0f;
		gl_Position.y -= 1.0f;
		OUT.texCoord = vec2(1 ,1);
		EmitVertex();

		// bottom left
		gl_Position = gl_in[i].gl_Position;
		gl_Position.x -= 1.0f;
		gl_Position.y -= 1.0f;
		OUT.texCoord = vec2(0 ,1);
		EmitVertex();

		EndPrimitive();
	}
	
}