#include "VolumeRender.h"

Volume::Volume()
{
	this->Transform = new Transformation();
}

void Volume::initVBO()
{
	GLfloat vertices[48] = {
		-1.0, -1.0, -1.0, 0.0, 0.0, 0.0,
		-1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 
		-1.0, 1.0, -1.0, 0.0, 1.0, 0.0,
		-1.0, 1.0, 1.0, 0.0, 1.0, 1.0,
		1.0, -1.0, -1.0, 1.0, 0.0, 0.0,
		1.0, -1.0, 1.0, 1.0, 0.0, 1.0, 
		1.0, 1.0, -1.0, 1.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 1.0, 1.0, 1.0
	};


	// draw the six faces of the boundbox by drawwing triangles
	// draw it contra-clockwise
	// front: 1 5 7 3
	// back: 0 2 6 4
	// left£º0 1 3 2
	// right:7 5 4 6    
	// up: 2 3 7 6
	// down: 1 0 4 5
	GLuint indices[36] = {
		1,5,7,
		7,3,1,
		0,2,6,
		6,4,0,
		0,1,3,
		3,2,0,
		7,5,4,
		4,6,7,
		2,3,7,
		7,6,2,
		1,0,4,
		4,5,1
	};
	GLuint gbo[2];

	glGenBuffers(2, gbo);
	GLuint vertexdat = gbo[0];
	GLuint veridxdat = gbo[1];
	glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
	glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	// used in glDrawElement()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	// vao like a closure binding 3 buffer object: verlocdat vercoldat and veridxdat
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0); // for vertexloc
	glEnableVertexAttribArray(1); // for vertexcol

								  // the vertex location is the same as the vertex color
	glBindBuffer(GL_ARRAY_BUFFER, vertexdat);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 *(sizeof(GLfloat)), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * (sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veridxdat);
	glBindVertexArray(0);
	this->g_vao = vao;
}

void Volume::initVol3DTex(const char * filename, GLuint w, GLuint h, GLuint d)
{
	FILE *fp;
	size_t size = w * h * d;
	GLubyte *data = new GLubyte[size];			  // 8bit
	if (!(fp = fopen(filename, "rb")))
	{
		cout << "Error: opening .raw file failed" << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "OK: open .raw file successed STEP 1" << endl;
	}
	if (fread(data, sizeof(char), size, fp) != size)
	{
		cout << "Error: read .raw file failed" << endl;
		system("pause");
		exit(1);
	}
	else
	{
		cout << "OK: read .raw file successed STEP 2" << endl;
	}
	fclose(fp);
	glGenTextures(1, &g_volTexObj);
	// bind 3D texture target
	glBindTexture(GL_TEXTURE_3D, g_volTexObj);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	// pixel transfer happens here from client to OpenGL server
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, w, h, d, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);

	delete[]data;
	cout << "volume texture created" << endl;
	this->g_volTexObj =  g_volTexObj;
}

void Volume::initFrameBuffer(GLuint texWidth, GLuint texHeight)
{
	
	// create a depth buffer for our framebuffer
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texWidth, texHeight);
	glGenFramebuffers(1, &g_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->backFace2DTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
}

void Volume::initFace2DTex(GLuint texWidth, GLuint texHeight)
{
	GLuint backFace2DTex;
	glGenTextures(1, &backFace2DTex);
	glBindTexture(GL_TEXTURE_2D, backFace2DTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_FLOAT, NULL);
	this->backFace2DTex = backFace2DTex;
}

void Volume::Render(GLenum glFaces, GLuint shader_id)
{

	
	GLuint id = glGetUniformLocation(shader_id, "u_modelMat");
	glUniformMatrix4fv(id, 1, GL_FALSE, &(this->Transform->getTransformMatrix())[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, this->ttf1DTex);
	glUniform1i(glGetUniformLocation(shader_id, "texture1D"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->backFace2DTex);
	glUniform1i(glGetUniformLocation(shader_id, "texture2D"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, this->g_volTexObj);
	glUniform1i(glGetUniformLocation(shader_id, "texture3D"), 2);

	glEnable(GL_CULL_FACE);
	glCullFace(glFaces);
	glBindVertexArray(g_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)NULL);
	glBindVertexArray(0);
	glDisable(GL_CULL_FACE);

}

void Volume::initTFF1DTex(const char * filename)
{
	ifstream inFile(filename, ifstream::in);
	if (!inFile)
	{
		cerr << "Error openning file: " << filename << endl;
		exit(EXIT_FAILURE);
	}

	const int MAX_CNT = 10000;
	GLubyte *tff = (GLubyte *)calloc(MAX_CNT, sizeof(GLubyte));
	inFile.read(reinterpret_cast<char *>(tff), MAX_CNT);
	if (inFile.eof())
	{
		size_t bytecnt = inFile.gcount();
		*(tff + bytecnt) = '\0';
		cout << "bytecnt " << bytecnt << endl;
	}
	else if (inFile.fail())
	{
		cout << filename << "read failed " << endl;
	}
	else
	{
		cout << filename << "is too large" << endl;
	}
	GLuint tff1DTex;
	glGenTextures(1, &tff1DTex);
	glBindTexture(GL_TEXTURE_1D, tff1DTex);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, tff);
	free(tff);
	this->ttf1DTex = tff1DTex;

}
