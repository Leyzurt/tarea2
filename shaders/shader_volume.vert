#version 330
layout(location = 0) in vec3 VerPos;
layout(location = 1) in vec3 VerClr;

out vec3 EntryPoint;
out vec4 ExitPointCoord;
uniform mat4 u_projMat;
uniform mat4 u_modelMat;
uniform mat4 u_viewMat;

void main()
{
	EntryPoint = VerClr;
	gl_Position = u_projMat * u_viewMat *u_modelMat * vec4(VerPos, 1.0);
	ExitPointCoord = gl_Position;
}