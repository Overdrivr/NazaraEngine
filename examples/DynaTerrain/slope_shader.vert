#version 110

attribute vec3 Position;
attribute vec3 Normal;

uniform mat4 WorldViewProjMatrix;

varying vec3 normal;

void main()
{
	normal = Normal;
    gl_Position = WorldViewProjMatrix * vec4(Position, 1.0);
}
