#version 150

in  vec2 ex_TexCoord; //texture coord arriving from the vertex
in  vec3 ex_Normal;  //normal arriving from the vertex

out vec4 out_Color;   //colour for the pixel
in vec3 ex_LightDir;  //light direction arriving from the vertex
in vec3 ex_SpotLightPos; //spotlight direction arriving to the vertex
in vec3 ex_SpotLightDir;		//spotlight direction
in vec3 water_eye_pos;

in vec3 ex_PositionEye;

uniform int hurtFlag;
uniform int activeCam;

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;

uniform vec4 spotlight_ambient;
uniform vec4 spotlight_specular;
uniform float spotlight_cutoff;
uniform float spotlight_outercutoff;

uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;

uniform sampler2D DiffuseMap;

void main(void)
{
	//out_Color = texture(DiffuseMap, ex_TexCoord); //show texture values

	//out_Color = vec4(ex_Normal,1.0); //Show normals

	//out_Color = vec4(ex_TexCoord,0.0,1.0); //show texture coords

	//Calculate lighting
	vec3 n, L, Ls;
	vec4 color;
	float NdotL, LdotLs;
	
	n = normalize(ex_Normal);
	L = normalize(ex_LightDir);

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	
	float RdotV = max(0.0, dot(r, v));

	NdotL = max(dot(n, L),0.0);

	//color = light_ambient * material_ambient;
	color = vec4(1,1,1,1) * material_ambient;
	
	if(NdotL > 0.0) 
	{
		color += (light_ambient * material_diffuse * NdotL);
	}

	color += material_specular * light_specular * pow(RdotV, material_shininess);

	// ***** SPOTLIGHT ************
	// https://learnopengl.com/Lighting/Light-casters

	L = normalize(ex_SpotLightPos - ex_PositionEye);
	Ls = normalize(ex_SpotLightDir);

	float theta = dot(L, -Ls);
	float epsilon   = spotlight_cutoff - spotlight_outercutoff;
	float I = clamp((theta - spotlight_outercutoff) / epsilon, 0.0, 1.0);    
    
	float Kc = 0.3;     //constant term 0.5
	float Kl = 0.0017;   //linear term 0.0014
	float Kq = 0.000009;  //quadratic term 0.000008
	float distance    = length(ex_SpotLightPos - ex_PositionEye);
	float attenuation = 1.0 / (Kc + Kl * distance + Kq * (distance * distance));

	if(theta > spotlight_outercutoff) 
	{       
		 r = normalize(-reflect(L, n));
	
		RdotV = max(0.0, dot(r, v));

		NdotL = max(dot(n, L),0.0);

		if(NdotL > 0.0) 
		{
			color += (spotlight_ambient * material_diffuse * NdotL)*pow(theta,50)*I*attenuation;
			color += material_specular * spotlight_specular * pow(RdotV, 5)*pow(theta,50)*I*attenuation;
		}
	}
	
	// END OF SPOTLIGHT ******************************
	
	vec4 red = vec4(0.49, 0.0, 0.0, 1.0);
	if(hurtFlag == 1) color = mix(red,color * texture(DiffuseMap, ex_TexCoord),0.7);
	else color = color * texture(DiffuseMap, ex_TexCoord);

	// ******* FOG ******
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


	if(ex_PositionEye.y < water_eye_pos.y + w || activeCam == 4){
		float fogStart = -40.0;
		float fogEnd = -500.0;
		float fogFactor = (fogEnd-ex_PositionEye.z)/(fogEnd-fogStart);
		fogFactor = clamp(fogFactor,0.0,1.00);
		vec4 fogColor = vec4(0.02,0.48,0.40,1.0);
		out_Color = mix(fogColor,mix(fogColor,color,fogFactor),0.6);
	}
	else{
		//out_Color = color;  //show just lighting

		out_Color = color; //show texture and lighting
	}
	
}