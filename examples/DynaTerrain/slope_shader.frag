#version 140

out vec4 out_Color;
varying vec3 normal;

void main()
{
    vec3 upVector = vec3(0.f,1.f,0.f);
    float slope = dot(upVector,normal);
	//out_Color = vec4(slope, slope, slope, 1.0);
	out_Color = vec4(normal.x, normal.y, normal.z, 1.0);
}
