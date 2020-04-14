#version 150

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;

in  vec3 in_Position;  // Position coming in
in  vec2 in_TexCoord;  // texture coordinate coming in
in  vec3 in_Normal;    // vertex normal used for lighting

uniform float displacement;
uniform vec4 LightPos;  // light position

out vec2 ex_TexCoord;  // exiting texture coord
out vec3 ex_Normal;    // exiting normal transformed by the normal matrix
out vec3 ex_PositionEye; 
out vec3 ex_LightDir; 

void main(void)
{
	vec3 newVertexPos = in_Position;
	newVertexPos.y = (sin(2.0 * newVertexPos.x + displacement ) * cos(1.5 * newVertexPos.y + displacement) * 0.8) + newVertexPos.y;

	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(newVertexPos, 1.0);
	
	ex_TexCoord = in_TexCoord;
		
	ex_Normal = NormalMatrix*in_Normal; 

	ex_PositionEye = vec3((ModelViewMatrix * vec4(in_Position, 1.0)));

	ex_LightDir = vec3(ViewMatrix * LightPos);
}