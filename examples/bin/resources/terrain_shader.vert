#version 140

in vec3 VertexPosition;
in vec3 VertexNormal;

uniform mat4 WorldViewProjMatrix;

out vec3 normal;
out float altitude;
out vec3 uvw;

void main()
{
	normal = VertexNormal;
	altitude = VertexPosition.y;
	uvw.x = mod(VertexPosition.x,512.0)/512.0;
	uvw.y = mod(VertexPosition.z,512.0)/512.0;
	uvw.z = mod(VertexPosition.y,512.0)/512.0;
    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
}
