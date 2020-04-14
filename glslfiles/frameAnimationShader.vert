#version 150

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ViewMatrix;
uniform int frameNumber;
uniform float framesHoriz;
uniform float framesVert;

in  vec3 in_Position;  // Position coming in
in  vec2 in_TexCoord;  // texture coordinate coming in
in  vec3 in_Normal;    // vertex normal used for lighting
in vec4 in_Color;


out vec2 ex_TexCoord;  // exiting texture coord
out vec3 ex_Normal;    // exiting normal transformed by the normal matrix
out vec3 ex_PositionEye; 
out vec4 ex_Color;
out vec3 water_eye_pos;

void main(void)
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
	ex_Color = in_Color;

	float row = framesVert-floor(float(frameNumber)/framesHoriz)-1;
	float column = float(frameNumber) - (framesVert-row-1.0)*framesHoriz;

	ex_TexCoord = in_TexCoord + vec2(column*(1/framesHoriz),-(framesVert-row-1)*(1/framesVert));
	//ex_TexCoord = in_TexCoord ;
	
	vec4 water_level = vec4(0.0,-5.1,0.0,1.0);
	water_eye_pos = vec3(ViewMatrix*water_level);
	
	ex_Normal = NormalMatrix*in_Normal; 

	ex_PositionEye = vec3((ModelViewMatrix * vec4(in_Position, 1.0)));

}