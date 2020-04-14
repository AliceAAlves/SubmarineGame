#version 150

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex
in vec4 ex_Color;

out vec4 out_Color;   //colour for the pixel

in vec3 ex_PositionEye;
in vec3 water_eye_pos;

uniform sampler2D DiffuseMap;

void main(void)
{
	//out_Color = vec4(ex_Color);
	//out_Color = texture(DiffuseMap, ex_TexCoord); 
	
	float w = 0.0;
	if(water_eye_pos.y <= -4.0){
		w = -10.0;
	}
	else if(water_eye_pos.y > -4.0 && water_eye_pos.y < 0.0){
		w = -2.0 + water_eye_pos.y*(10.0-2.0)/(4.0-0.0);
	}
	else if(water_eye_pos.y >= 4.0){
		w = 10.0;
	}
	else if(water_eye_pos.y < 4.0 && water_eye_pos.y >= 0.0){
		w = 0.0 + water_eye_pos.y*(10.0-0.0)/(4.0-0.0);
	}
	/*if(water_eye_pos.y <= 0.0){
		w = -1000.0;
	}
	else if(water_eye_pos.y > 0.0){
		w = 1000;
	}*/


	if(ex_PositionEye.y < water_eye_pos.y + w && texture(DiffuseMap, ex_TexCoord).a != 0){
		float fogStart = -40.0;
		float fogEnd = -500.0;
		float fogFactor = (fogEnd-ex_PositionEye.z)/(fogEnd-fogStart);
		fogFactor = clamp(fogFactor,0.0,1.00);
		vec4 fogColor = vec4(0.02,0.48,0.40,1.0);
		out_Color = mix(fogColor,mix(fogColor,texture(DiffuseMap, ex_TexCoord),fogFactor),0.6);
	}
	else{
		//out_Color = color;  //show just lighting

		out_Color = texture(DiffuseMap, ex_TexCoord); //show texture and lighting
	}
	//out_Color = ex_Color;  //show just lighting

}