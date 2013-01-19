#version 110

attribute vec3 Position;
attribute vec3 Normal;

uniform mat4 WorldViewProjMatrix;

varying vec3 normal;
varying float altitude;
varying float u;
varying float v;

void main()
{
	normal = Normal;
	altitude = Position.y;
	u = mod(Position.x,512.0);
	v = mod(Position.z,512.0);
	//u = fract(Position.x);
	//v = fract(Position.z);
    gl_Position = WorldViewProjMatrix * vec4(Position, 1.0);
}
