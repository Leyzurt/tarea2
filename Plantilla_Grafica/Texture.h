#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>
#include "../include/glew/glew.h"
#include "../include/SOIL/SOIL.h"

#include <iostream>
#include <stdio.h>
#include <vector>

/*#define SDL_MAIN_HANDLED
#include <sdl2\SDL.h>
#include <sdl2\SDL_image.h>*/

using namespace std;

using std::string;
class Texture {
private:
	string route;
	GLuint m_texture;
	int num_texture;
public:
	Texture(string route);
	Texture();
	~Texture();
	GLuint getID();
	GLuint loadTexture(GLchar const *path);
	GLuint loadCubeMap(vector<string> path);
	string getRoute();
	void setTextureID(GLuint m_texture);
};

#endif

