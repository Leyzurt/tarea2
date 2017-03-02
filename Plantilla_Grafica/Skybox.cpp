#include "Skybox.h"

skybox::skybox() {
	SIZE = 500.0f;
	float VERTICES[] = {
		-SIZE,  SIZE, -SIZE,
		-SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE, -SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,

		-SIZE, -SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE, -SIZE,  SIZE,
		-SIZE, -SIZE,  SIZE,

		-SIZE,  SIZE, -SIZE,
		SIZE,  SIZE, -SIZE,
		SIZE,  SIZE,  SIZE,
		SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE,  SIZE,
		-SIZE,  SIZE, -SIZE,

		-SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE, -SIZE,
		SIZE, -SIZE, -SIZE,
		-SIZE, -SIZE,  SIZE,
		SIZE, -SIZE,  SIZE
	};

	glGenVertexArrays(1, &(this->glVAO_dir));
	glBindVertexArray(this->glVAO_dir);
	glGenBuffers(1, &(this->glVBO_dir));
	glBindBuffer(GL_ARRAY_BUFFER, this->glVBO_dir);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindVertexArray(0);

	vector<string> path;
	path.push_back("../models/new_skybox/right.png"); 
	path.push_back("../models/new_skybox/left.png"); 
	path.push_back("../models/new_skybox/top.png");
	path.push_back("../models/new_skybox/bottom.png"); 
	path.push_back("../models/new_skybox/front.png"); 
	path.push_back("../models/new_skybox/back.png");
	T = new Texture();
	this->textureID = T->loadCubeMap(path);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

GLuint skybox::getTextureID() {
	return this->textureID;
}

void skybox::setTextureID(GLuint texture) {
	this->textureID = texture;
}

void skybox::renderSkybox()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->textureID);
	glUniform1i(glGetUniformLocation(this->glslProgramSkybox->getProgramID(), "cubeMap"), 0);	
	glBindVertexArray(this->glVAO_dir);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void skybox::setProgram(CGLSLProgram * Program)
{
	glslProgramSkybox = Program;
}

