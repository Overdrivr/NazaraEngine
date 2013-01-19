#version 140

uniform sampler2D textureSampler;
out vec4 out_Color;
varying vec3 normal;
varying float altitude;
varying float u;
varying float v;

void main()
{
    vec3 upVector = vec3(0.f,1.f,0.f);
    float slope = dot(normal,upVector);
	
	vec4 color = vec4(1.0,1.0,1.0,1.0);
	
	vec2 uv = vec2(u,v);
	
	uv.x /= 512.0;
	uv.y /= 512.0;
	
	//if((uv.x >= 0 && uv.x < 0.05))// || (uv.y >= 0 && uv.y < 0.005))
		color = vec4(uv.x,0.0,0.0,1.0);
	
	out_Color = color;
	//out_Color = texture2D(textureSampler,vec2(u,v));
	
	/*
	if(altitude > 600.f)
		color = vec4(0.9,0.9,0.9,1.0);
	else if(altitude < 75.f)
		color = vec4(0.4,0.4,0.9,1.0);
	else if(slope > 0.5)
		color = vec4(0.2,0.7,0.2,1.0);
	else
		color = vec4(0.5,0.5,0.1,1.0);
	
	out_Color = color;*/
	
	//Pour afficher les normales par pixel
	//out_Color = vec4(normal.x, normal.y, normal.z, 1.0);
	
	//Pour afficher 
	//out_Color = vec4(slope, slope, slope, 1.0);
}
