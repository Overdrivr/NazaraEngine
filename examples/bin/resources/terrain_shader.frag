#version 140

uniform sampler2D terrainTexture;
out vec4 out_Color;
in vec3 normal;
in vec3 position;

vec2 uvTileConversion(float slope, float altitude, vec2 uv);
//vec3 triplanarMappingContribution(vec3 normal);

void main()
{
	//Computing the slope
    vec3 upVector = vec3(0.0,1.0,0.0);
    float slope = dot(normal,upVector);
	
	//Altitude
	float altitude = position.y;
	
	//Computing uvw
	float tex_scale = 512.0;
	vec3 uvw;
	uvw.x = fract(position.x/tex_scale);
	uvw.y = fract(position.y/tex_scale);
	uvw.z = fract(position.z/tex_scale);
	
	//Triplanar projection
	vec3 weights = abs(normal);
	weights = max((weights - 0.2) * 5 ,0);
	weights /= vec3(weights.x + weights.y + weights.z);
	
	vec2 coord1 = uvw.zy;
	vec2 coord2 = uvw.xz;
	vec2 coord3 = uvw.yx;
	
	vec4 col1 = texture2D(terrainTexture,coord1);
	vec4 col2 = texture2D(terrainTexture,coord2);
	vec4 col3 = texture2D(terrainTexture,coord3);
	
	//Texture tiling & sampling
	/*vec4 col1 = texture2D(terrainTexture,uvTileConversion(slope,altitude,coord1));
	vec4 col2 = texture2D(terrainTexture,uvTileConversion(slope,altitude,coord2));
	vec4 col3 = texture2D(terrainTexture,uvTileConversion(slope,altitude,coord3));*/
	
	out_Color = col1 * weights.xxxx + col2 * weights.yyyy + col3 * weights.zzzz;
	
	
	//out_Color = texture2D(terrainTexture,vec2(uv.y,uv.x));
	//out_Color = texture2D(terrainTexture,uvTileConversion(slope,altitude,vec2(uv.y,uv.x)));
	
	//Pour afficher les normales par pixel
	//out_Color = vec4(normal.x, normal.y, normal.z, 1.0);
	
	//Pour afficher la variation de pente par pixel
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