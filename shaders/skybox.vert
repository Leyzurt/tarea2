#version 330

in layout(location=0) vec3 position;
out vec3 textureCoords;

uniform mat4 u_projMat;
uniform mat4 u_viewMat;

void main(void){
	
	gl_Position = u_projMat * u_viewMat * vec4(position, 1.0); 
	textureCoords = position;
	
}