#version 150

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel

in vec3 ex_PositionEye;

uniform sampler2D DiffuseMap;

void main(void)
{
	out_Color = texture(DiffuseMap, ex_TexCoord); 
}