#include "FrameBuffer.h"
#include <iostream>
using namespace std;

FrameBuffer::FrameBuffer(){
	//-- Sizes
	this->IMAGE_WIDTH = 900;
	this->IMAGE_HEIGHT = 900;
	this->CUBEMAP_WIDTH = 128;
	this->CUBEMAP_HEIGHT = 128;

	//-- Buffers
	initialiseImageFrameBuffer();
	initialiseCubeMapFrameBuffer();
}

FrameBuffer::FrameBuffer(const int width, const int height) {
	//-- Sizes
	this->IMAGE_WIDTH = width;
	this->IMAGE_HEIGHT = height;
	this->CUBEMAP_WIDTH = width;
	this->CUBEMAP_HEIGHT = height;

	//-- Buffers
	initialiseImageFrameBuffer();
	initialiseCubeMapFrameBuffer();
}

FrameBuffer::FrameBuffer(const glm::vec2 imageSize, const glm::vec2 cubeMapSize) {
	//-- Sizes
	this->IMAGE_WIDTH = (int)imageSize.x;
	this->IMAGE_HEIGHT = (int)imageSize.y;
	this->CUBEMAP_WIDTH = (int)cubeMapSize.x;
	this->CUBEMAP_HEIGHT = (int)cubeMapSize.y;

	//-- Buffers
	initialiseImageFrameBuffer();
	initialiseCubeMapFrameBuffer();
}

FrameBuffer::FrameBuffer(const int imageWidth, const int imageHeight, const int cubeMapWidth, const int cubeMapHeight){
	//-- Sizes
	this->IMAGE_WIDTH = imageWidth;
	this->IMAGE_HEIGHT = imageHeight;
	this->CUBEMAP_WIDTH = cubeMapWidth;
	this->CUBEMAP_HEIGHT = cubeMapHeight;

	//-- Buffers
	initialiseImageFrameBuffer();
	initialiseCubeMapFrameBuffer();
}

FrameBuffer::~FrameBuffer(){
	glDeleteFramebuffers(1, &this->imageFrameBuffer);
	glDeleteFramebuffers(1, &this->cubeMapFrameBuffer);

	glGenRenderbuffers(1, &this->imageDepthBuffer);
	glGenRenderbuffers(1, &this->cubeMapDepthBuffer);

	glDeleteTextures(1, &this->imageColorTexture);
	glDeleteTextures(1, &this->cubeMapColorTexture);
	glDeleteTextures(1, &this->imageDepthTexture);
	glDeleteTextures(1, &this->cubeMapDepthTexture);
}

int FrameBuffer::getImageWidth(){
	return this->IMAGE_WIDTH;
}

int FrameBuffer::getImageHeight(){
	return this->IMAGE_HEIGHT;
}

int FrameBuffer::getCubeMapWidth(){
	return this->CUBEMAP_WIDTH;
}

int FrameBuffer::getCubeMapHeight(){
	return this->CUBEMAP_HEIGHT;
}

glm::vec2 FrameBuffer::getImageSize(){
	return glm::vec2(this->IMAGE_WIDTH, this->IMAGE_HEIGHT);
}

glm::vec2 FrameBuffer::getCubeMapSize(){
	return glm::vec2(this->IMAGE_WIDTH, this->IMAGE_HEIGHT);
}

GLuint FrameBuffer::getImageColorTexture(){
	return this->imageColorTexture;
}

GLuint FrameBuffer::getImageDepthTexture(){
	return this->imageDepthTexture;
}

GLuint FrameBuffer::getCubeMapColorTexture(){
	return this->cubeMapColorTexture;
}

GLuint FrameBuffer::getCubeMapDepthTexture(){
	return this->cubeMapDepthTexture;
}

void FrameBuffer::setImageWidth(const int imageWidth){
	this->IMAGE_WIDTH = imageWidth;
}

void FrameBuffer::setImageHeight(const int imageHeight){
	this->IMAGE_HEIGHT = imageHeight;
}

void FrameBuffer::setCubeMapWidth(const int cubeMapWidth){
	this->CUBEMAP_WIDTH = cubeMapWidth;
}

void FrameBuffer::setCubeMapHeight(const int cubeMapHeight){
	this->CUBEMAP_HEIGHT = cubeMapHeight;
}

void FrameBuffer::setImageSize(const glm::vec2 imageSize){
	this->IMAGE_WIDTH = (int)imageSize.x;
	this->IMAGE_HEIGHT = (int)imageSize.y;
}

void FrameBuffer::setCubeMapSize(const glm::vec2 cubeMapSize){
	this->CUBEMAP_WIDTH = (int)cubeMapSize.x;
	this->CUBEMAP_HEIGHT = (int)cubeMapSize.y;
}

void FrameBuffer::cleanUp(){
	this->~FrameBuffer();
}

void FrameBuffer::bindImageFrameBuffer(){
	bindImageFrameBuffer(this->imageFrameBuffer, this->IMAGE_WIDTH, this->IMAGE_HEIGHT);
}

void FrameBuffer::bindCubeMapFrameBuffer() {
	bindCubeMapFrameBuffer(this->cubeMapFrameBuffer, this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
}

void FrameBuffer::activeImageColorTexture(){
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_RGB,
		this->imageColorTexture,
		0);
}

void FrameBuffer::activeImageDepthTexture(){
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_RGB,
		this->imageDepthTexture,
		0);
}

void FrameBuffer::activeCubeMapColorTexture(const int face){
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
		this->cubeMapColorTexture,
		0);
}

void FrameBuffer::activeCubeMapDepthTexture(const int face){
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
		this->cubeMapDepthTexture,
		0);
}

void FrameBuffer::unbindCurrentFrameBuffer(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1440, 900);
}


GLuint FrameBuffer::getImageFrameBuffer() {
	return this->imageFrameBuffer;
}

void FrameBuffer::initialiseImageFrameBuffer(){
	this->imageFrameBuffer = createFrameBuffer();
	//this->imageDepthBuffer = createDepthBufferAttachment(this->IMAGE_WIDTH, this->IMAGE_HEIGHT);
	//this->imageColorTexture = createImageColorTextureAttachment(this->IMAGE_WIDTH, this->IMAGE_HEIGHT);
	this->imageDepthTexture = createImageDepthTextureAttachment(this->IMAGE_WIDTH, this->IMAGE_HEIGHT);
	this->unbindCurrentFrameBuffer();
}

void FrameBuffer::initialiseCubeMapFrameBuffer(){
	this->cubeMapFrameBuffer = createFrameBuffer();
	this->cubeMapDepthBuffer = createDepthBufferAttachment(this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
	this->cubeMapColorTexture = createCubeMapColorTextureAttachment(this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
	this->cubeMapDepthTexture = createCubeMapDepthTextureAttachment(this->CUBEMAP_WIDTH, this->CUBEMAP_HEIGHT);
	this->unbindCurrentFrameBuffer();
}

void FrameBuffer::bindCubeMapFrameBuffer(const GLuint frameBuffer, const int width, const int height){
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}

void FrameBuffer::bindImageFrameBuffer(const GLuint frameBuffer, const int width, const int height){
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_NONE);
	glViewport(0, 0, width, height);
}

GLuint FrameBuffer::createFrameBuffer(){
	cout << "create Frame Buffer\n";
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_NONE);
	return frameBuffer;
}

GLuint FrameBuffer::createImageColorTextureAttachment(const int width, const int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint FrameBuffer::createCubeMapColorTextureAttachment(const int width, const int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	for (int i = 0; i < 6; i++) 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return texture;
}

GLuint FrameBuffer::createImageDepthTextureAttachment(const int width, const int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); //CAMBIE DE GL_DEPTH_COMPONENT16
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	glDrawBuffer(GL_NONE);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint FrameBuffer::createCubeMapDepthTextureAttachment(const int width, const int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return texture;
}

GLuint FrameBuffer::createDepthBufferAttachment(const int width, const int height){
	GLuint depthBuffer;
	//glGenRenderbuffers(1, &depthBuffer);
	glGenFramebuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}