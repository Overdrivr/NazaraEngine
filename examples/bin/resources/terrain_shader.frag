#version 140

uniform sampler2D terrainTexture;
out vec4 out_Color;
in vec3 normal;
in float altitude;
in vec3 uvw;

vec2 uvTileConversion(float slope, float altitude, vec2 uv);

void main()
{
    vec3 upVector = vec3(0.0,1.0,0.0);
    float slope = dot(normal,upVector);
	
	vec3 uv;///FIX ME !
	
	//Triplanar projection
	vec3 weights = normal;
	
	//out_Color = texture2D(terrainTexture,vec2(uv.y,uv.x));
	out_Color = texture2D(terrainTexture,uvTileConversion(slope,altitude,vec2(uv.y,uv.x)));
	
	//Pour afficher les normales par pixel
	//out_Color = vec4(normal.x, normal.y, normal.z, 1.0);
	
	//Pour afficher 
	//out_Color = vec4(slope, slope, slope, 1.0);
}

vec2 uvTileConversion(float slope, float altitude, vec2 uv)
{
    vec2 tile = vec2(0.0,3.0);
	//On détermine à quelle tile le point appartient
	if(altitude > 600.0)
		tile = vec2(3.0,3.0);//Snow
	else if(altitude < 75.0)
		tile = vec2(3.0,2.0);//Sand
	else if(slope > 0.5)
		tile = vec2(2.0,3.0);//Rock
	
	vec2 output;
	output.x = uv.x*0.25 + tile.x*0.25;
	output.y = uv.y*0.25 + tile.y*0.25;
	return output;
}