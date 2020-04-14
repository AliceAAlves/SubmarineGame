#version 150

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform float health;

in  vec3 in_Position;  // Position coming in
in  vec4 in_Color;     // colour coming in
out vec4 ex_Color;     // colour leaving the vertex, this will be sent to the fragment shader
out float ex_health;

in vec2 in_TexCoord;   // texture coordinate coming in to the vertex
out vec2 ex_TexCoord;  // texture coordinate going out to the fragment shader

void main(void)
{
	vec3 newPos = in_Position;
	if(in_Position.x != 0){
		newPos.x = health*newPos.x;
	}
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(newPos, 1.0);
	ex_Color = in_Color;
	ex_health = health;
	ex_TexCoord = in_TexCoord;
}