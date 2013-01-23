#version 140

in vec3 VertexPosition;
in vec3 VertexNormal;

uniform mat4 WorldViewProjMatrix;

out vec3 normal;
out vec3 position;

void main()
{
	normal = VertexNormal;
	position = VertexPosition;

    gl_Position = WorldViewProjMatrix * vec4(VertexPosition, 1.0);
}
