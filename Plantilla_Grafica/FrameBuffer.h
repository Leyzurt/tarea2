#pragma once
#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

//-- Gloabal includes
#include "Texture.h"

#include <vector>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtc\type_ptr.hpp>

//-- Using spaces
using std::vector;
using std::string;
using glm::vec2;
using glm::mat4;

class FrameBuffer {
private:
	//-- Variables
	int IMAGE_WIDTH = 320;
	int IMAGE_HEIGHT = 180;

	int CUBEMAP_WIDTH = 320;
	int CUBEMAP_HEIGHT = 320;

	GLuint imageFrameBuffer;
	GLuint imageDepthBuffer;
	GLuint imageColorTexture;
	GLuint imageDepthTexture;

	GLuint cubeMapFrameBuffer;
	GLuint cubeMapDepthBuffer;
	GLuint cubeMapColorTexture;
	GLuint cubeMapDepthTexture;

	//-- Initialize Functions
	void initialiseImageFrameBuffer();
	void initialiseCubeMapFrameBuffer();

	void bindImageFrameBuffer(const GLuint frameBuffer, const int width, const int height);
	void bindCubeMapFrameBuffer(const GLuint frameBuffer, const int width, const int height);
	
	//-- Create Functions
	GLuint createFrameBuffer();
	GLuint createDepthBufferAttachment(const int width, const int height);
	GLuint createImageColorTextureAttachment(const int width, const int height);
	GLuint createCubeMapColorTextureAttachment(const int width, const int height);
	GLuint createImageDepthTextureAttachment(const int width, const int height);
	GLuint createCubeMapDepthTextureAttachment(const int width, const int height);
	
public:
	FrameBuffer();
	FrameBuffer(const int width, const int height);
	FrameBuffer(const glm::vec2 imageSize, const glm::vec2 cubeMapSize);
	FrameBuffer(const int imageWidth, const int imageHeight, const int cubeMapWidth, const int cubeMapHeight);
	~FrameBuffer();

	//-- Getters
	int getImageWidth();
	int getImageHeight();
	int getCubeMapWidth();
	int getCubeMapHeight();
	glm::vec2 getImageSize();
	glm::vec2 getCubeMapSize();
	GLuint getImageColorTexture();
	GLuint getImageDepthTexture();
	GLuint getCubeMapColorTexture();
	GLuint getCubeMapDepthTexture();
	GLuint getImageFrameBuffer();

	//-- Setters
	void setImageWidth(const int imageWidth);
	void setImageHeight(const int imageHeight);
	void setCubeMapWidth(const int cubeMapWidth);
	void setCubeMapHeight(const int cubeMapHeight);
	void setImageSize(const glm::vec2 imageSize);
	void setCubeMapSize(const glm::vec2 cubeMapSize);

	//-- Functions
	void bindImageFrameBuffer();
	void bindCubeMapFrameBuffer();
	void activeImageColorTexture();
	void activeImageDepthTexture();
	void activeCubeMapColorTexture(const int face);
	void activeCubeMapDepthTexture(const int face);
	void unbindCurrentFrameBuffer();
	void cleanUp();
};


#endif