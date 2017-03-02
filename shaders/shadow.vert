#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;

// Values that stay constant for the whole mesh.
uniform mat4 u_projMat;
uniform mat4 u_modelMat;
uniform mat4 u_viewMat;

void main(){
 gl_Position =  u_projMat * u_viewMat *u_modelMat * vec4(position,1);
}