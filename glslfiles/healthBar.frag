// Fragment Shader 
#version 150

in  vec4 ex_Color;  //colour arriving from the vertex
out vec4 out_Color; //colour for the pixel

in vec2 ex_TexCoord;  //texture coordinate coming from the vertex shader
in float ex_health;

uniform sampler2D DiffuseMap;  //sampler 2d for the texture map
//uniform float health;

void main(void)
{
	vec4 colorRed = vec4(0.58,0.0,0.0,1.0);
	vec4 colorOrange = vec4(0.83,0.51,0.02,1.0);
	vec4 colorGreen = vec4(0.05,0.46,0.0,1.0);
	if(ex_health <= 0.3) out_Color = colorRed;
	else if(ex_health <= 0.6) out_Color = colorOrange;
	else out_Color = colorGreen;
}