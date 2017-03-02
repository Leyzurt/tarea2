#pragma once
#include <vector>
#include <string>
#include "Texture.h"
#include "GLSLProgram.h"
class skybox {
private:
	float SIZE;
	float VERTICES[108];
	GLuint textureID;
	Texture *T;
	GLuint glVBO_indexes_dir;
	vector<GLint> *ID;
	vector<GLuint> glVBO_indexes;
	GLuint glVBO_dir, glVAO_dir;
public:
	CGLSLProgram *glslProgramSkybox;
	skybox();
	void renderSkybox();
	GLuint getTextureID();
	void setTextureID(GLuint texture);
	void setProgram(CGLSLProgram* Program);

};