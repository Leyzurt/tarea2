#include "main.h"

bool shadowMap;
glm::mat4 lightSpaceMatrix;
/* STAGE */
Camera *camera = new Camera();
vector<Volume*> volumenes;
GLuint shader_id, shader_id_vert;
TwBar *principalBar;
typedef enum { HEAD, BONSAI, SKULL } Specular;
Specular specular = HEAD;
TwEnumVal typeSpecular[] = { { HEAD, "Head" },{ BONSAI, "Bonsai" },{ SKULL, "Skull" } };
TwType specularType;

	float select = 1.0f;
	float TranslationSpeedX, TranslationSpeedY, TranslationSpeedZ;
	float TranslationSpeedX_Light, TranslationSpeedY_Light, TranslationSpeedZ_Light;
	float Scale, Rotation[4];
	float g_LightDirection[] = { 0.02f, -0.83f, -0.57f };
	float refract = 1.0f;
	float lightAmbientIntensity[3], lightDiffuseIntensity[3], lightSpecularIntensity[3];
	float rgb[3];
	float matAmbientReflectances[3], matDiffuseReflectances[3], matSpecularReflectances[3], matShininess;
	float roughness, fresnel, albedo, interpolacion, texture_active;
	float atConstant, atLineal, atCuadratic, exponent, spotcos;
	float Ldirect = 0.0f, Lpoint=0.0f, Lspot=1.0f;
	bool luces_on;	
	int entitieSelected = 0;
	bool animacion_on = true;



	void initFrameBuffer(GLuint texObj, GLuint texWidth, GLuint texHeight)
	{
		// create a depth buffer for our framebuffer
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texWidth, texHeight);

		// attach the texture and the depth buffer to the framebuffer
		glGenFramebuffers(1, &g_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, g_frameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texObj, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glEnable(GL_DEPTH_TEST);
	}

	void TW_CALL Select(void * clientData) {
		if (specular == HEAD) {
			entitieSelected = 0;
		}
		else if (specular == BONSAI) {
			entitieSelected = 1;
		}
		else {
			entitieSelected = 2;
		}

		TranslationSpeedX = volumenes[entitieSelected]->Transform->getTraslationMatrix()[3][0];
		TranslationSpeedY = volumenes[entitieSelected]->Transform->getTraslationMatrix()[3][1];
		TranslationSpeedZ = volumenes[entitieSelected]->Transform->getTraslationMatrix()[3][2];
		Scale = volumenes[entitieSelected]->Transform->getScaleMatrix()[0][0];
		glm::quat aux = glm::quat(volumenes[entitieSelected]->Transform->getRotationMatrix());
		Rotation[0] = aux.x; Rotation[1] = aux.y; Rotation[2] = aux.z; Rotation[3] = aux.w;

	}

	void TW_CALL Delete(void * clientData) {
		for (int i = 0; i < transfer_func->puntos.size(); i++)
		{
			if (transfer_func->puntos[i]->selected) {
				transfer_func->puntos.erase(transfer_func->puntos.begin() + i);
				transfer_func->OrderPoints();
				return;
			}
		}

	}

	void InitAnt() {
		Scale = 1.00f;
		/*for (int i = 0; i < 4; i++)
		{
			Rotation[i] = 0.0f;
		}*/
		shadowMap = true;
		Rotation[0] = 1.0f; Rotation[1] = 1.0f; Rotation[2] = 1.0f; Rotation[3] = 1.0f;
		
		rgb[0] = 1.0f;
		rgb[1] = 0.0f;
		rgb[2] = 0.0f;

		TranslationSpeedX_Light = 2.4f;
		TranslationSpeedY_Light = 15.0f;
		TranslationSpeedZ_Light = 22.8f;
		
		TranslationSpeedX = 0.0;
		TranslationSpeedY = 0.0f;
		TranslationSpeedZ = 0.0f;
		matShininess = 0.1f;
		principalBar = TwNewBar("Model");
		//TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.

		TwDefine("Model refresh = '0.0001f'");
		TwDefine("Model resizable = false");
		TwDefine("Model fontresizable = false");
		TwDefine("Model movable = false");
		TwDefine("Model visible = true");
		TwDefine("Model position = '20 20'");
		TwDefine("Model size = '300 600'");

		specularType = TwDefineEnum("Volume Selector", typeSpecular, 3);
		TwAddVarRW(principalBar, "Select Combo", specularType, &specular, "group='Select'");
		TwAddButton(principalBar, "Select Volume", Select, NULL, "group='Select'");


		TwAddSeparator(principalBar, "", NULL);


		TwAddVarRW(principalBar, "TranslationSpeedX", TW_TYPE_FLOAT, &TranslationSpeedX, " min=-10000 max=10000 step=0.1 group='Translation' label='Translation factor X' ");
		TwAddVarRW(principalBar, "TranslationSpeedY", TW_TYPE_FLOAT, &TranslationSpeedY, " min=-10000 max=10000 step=0.1 group='Translation' label='Translation factor Y' ");
		TwAddVarRW(principalBar, "TranslationSpeedZ", TW_TYPE_FLOAT, &TranslationSpeedZ, " min=-10000 max=10000 step=0.1 group='Translation' label='Translation factor Z' ");
		TwAddVarRW(principalBar, "Rotation", TW_TYPE_QUAT4F, &Rotation, "label='Rotation' opened=true help='Change the model orientation.'");
		TwAddVarRW(principalBar, "ScaleAll", TW_TYPE_FLOAT, &Scale, "min=0.1 max=10000.0 step=0.1 group='Scale'");
		TwDefine(" Model/Translation group=Transforms help='Change the model translations.'");
		TwDefine(" Model/Transforms help='Apply transforms to the selected model.' opened=false ");
		TwDefine(" Model/Rotation group=Transforms ");
		TwDefine(" Model/Scale group=Transforms ");		
		TwAddSeparator(principalBar, "", NULL);
		
		/*Material parameters*/
		TwAddVarRW(principalBar, "RGB", TW_TYPE_COLOR3F, &rgb, "group='POINTS' label='RGB_SELECTED' opened=true");
		TwAddButton(principalBar, "DELETE_POINT", Delete, NULL, "group='POINTS'");
		TwDefine("Model/POINTS opened=true");
		TwAddSeparator(principalBar, "", NULL);


	}

	void reshape(GLFWwindow *window, int width, int height) {
		gWidth = width;
		gHeight = height;		
		glViewport(0, 0, gWidth, gHeight);
		TwWindowSize(gWidth, gHeight);
		mProjMatrix = glm::perspective(45.0f, (float)gWidth / (float)gHeight, 1.0f, 1000.0f);
		//TwWindowSize(gWidth, gWidth);
	}
	bool click_left = false;
	bool firstMouse = true;
	bool click = false;
	GLfloat lastX = 0, lastY = 0;
	GLfloat pitch = 0.0f, yaw = -90.0f;
	GLfloat fov = 45.0f;
	bool MousePicking = false;
	glm::vec3 ray = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec3 punto = glm::vec3(1.0f, 1.0f, 1.0f);
	int seleccionado = -1;
	bool gpress = false;
	void cursorPos(GLFWwindow* window, double xpos, double ypos) {
		if (TwEventMousePosGLFW(int(xpos), int(ypos)))
			return;
		
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		if (!click) {	
			if (gpress) {
				if (xpos >= 1154 && ypos <= 237 && xpos <= 1340 && ypos >= 110) {
					float oldX = (xpos / gWidth) * 2 - 1;
					float oldY = 1 - (ypos / gHeight) * 2;
					float x_a = (oldX - 0.64) / (0.92 - 0.64);
					float y_a = (oldY - 0.47) / (0.76 - 0.47);

					for (int i = 0; i < transfer_func->puntos.size(); i++)
					{
						if (transfer_func->puntos[i]->selected) {
							transfer_func->puntos[i]->x = x_a;
							transfer_func->puntos[i]->y = y_a;
						}
					}
					
				}			
			}
			return;
		}
		
		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.05;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camera->setTarget(glm::normalize(front));


		//click = false;
		//cameraFront = glm::normalize(front);
	}

	void mouseButton(GLFWwindow* window, int button, int action, int mods) {

		if (TwEventMouseButtonGLFW(button, action))
			return;

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {			
			click = true;		
			firstMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			double x, y;
			glfwGetCursorPos(gWindow, &x, &y);
			float xpos = float(x);
			float ypos = float(y);
			if (xpos >= 1154 && ypos <= 237 && xpos <= 1340 && ypos >= 110) {
				float oldX = (xpos / gWidth) * 2 - 1;
				float oldY = 1 - (ypos / gHeight) * 2;
				float x_a = (oldX - 0.64) / (0.92 - 0.64);
				float y_a = (oldY - 0.47) / (0.76 - 0.47);

				for (int i = 1; i < transfer_func->puntos.size()-1; i++)
				{					
					if (transfer_func->puntos[i]->y + 0.05 >= y_a  && transfer_func->puntos[i]->y - 0.05 <= y_a && transfer_func->puntos[i]->x + 0.05 >= x_a  && transfer_func->puntos[i]->x - 0.05 <= x_a ) { //SE SELECCIONA	
						for (int j = 0; j < transfer_func->puntos.size(); j++)
						{
							transfer_func->puntos[j]->selected = false;
						}

						transfer_func->puntos[i]->selected = true;
						rgb[0] = transfer_func->puntos[i]->r;
						rgb[1] = transfer_func->puntos[i]->g;
						rgb[2] = transfer_func->puntos[i]->b;
						gpress = true;
						return;
					}					
				}
				transfer_func->puntos.push_back(new points(x_a, y_a, 1.0, 1.0, 1.0, false));
				transfer_func->OrderPoints();
			}
			return;
		
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			click = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {	
				gpress = false;
				transfer_func->OrderPoints();			
		}
	}


	void keyInput(GLFWwindow * window, int key, int scancode, int action, int mods) {
		// W adelante, S atras, A izquierda, D derecha, Z bajar, C subir (CAMARA)
		if (TwEventKeyGLFW(key, action))
			return;

		GLfloat cameraSpeed = 0.5f;
	/*	if (key == GLFW_KEY_UP) {
			light->addPosition(glm::vec3(-1.0f, 1.0f, -1.0f));			
		}*/
		if (key == GLFW_KEY_W) {
			camera->setPosition(camera->getPosition() + (cameraSpeed * camera->getTarget()));
			//camera->addPosition(glm::vec3(0.0f,0.0f,-0.2f));		
		}
		else if(key == GLFW_KEY_S)
		{
			camera->setPosition(camera->getPosition() - (cameraSpeed * camera->getTarget()));
			//camera->addPosition(glm::vec3(0.0f, 0.0f, 0.2f));
		}
		else if(key == GLFW_KEY_A)
		{

			camera->setPosition(camera->getPosition() - (cameraSpeed * glm::normalize(glm::cross(camera->getTarget(), camera->getUp()) )));
			//camera->addPosition(glm::vec3(-0.2f, 0.0f, 0.0f));
		}
		else if(key == GLFW_KEY_D)
		{
			camera->setPosition(camera->getPosition() + (cameraSpeed * glm::normalize(glm::cross(camera->getTarget(), camera->getUp()))));
			//camera->addPosition(glm::vec3(0.2f, 0.0f , 0.0f));
		}
		else if (key == GLFW_KEY_Z)
		{
			camera->addPosition(glm::vec3(0.0f, -0.2f, 0.0f));
		}
		else if (key == GLFW_KEY_C)
		{
			camera->addPosition(glm::vec3( 0.0f, 0.2f, 0.0f));
		}

		/*UserInterface *localUserInterface = UserInterface::Instance(NULL);

		
		if (key == GLFW_KEY_SPACE)
		localUserInterface->mediator->Send("Volver al principal", localUserInterface);*/
	}

	void charCall(GLFWwindow* window, unsigned int charKey) {
		if (TwEventCharGLFW(charKey, GLFW_PRESS)) return;
	}

	bool initGlfw() {		
		if (!glfwInit() )
			return false;
		
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);		
		//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		

		gWindow = glfwCreateWindow(gWidth, gHeight, "3DMView", NULL, NULL);
		
		if (!gWindow) {
			glfwTerminate();
			return false;
		}
				
		glfwMakeContextCurrent(gWindow);

		if (!TwInit(TW_OPENGL, NULL)) {
			return false;
		}

		const GLFWvidmode * vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowPos(gWindow, (vidMode->width - gWidth) >> 1, (vidMode->height - gHeight) >> 1);		
		//glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		/*Callbacks GLFW*/
		//glfwSetCharCallback(gWindow, charInput); 
		glfwSetKeyCallback(gWindow, keyInput);
		glfwSetWindowSizeCallback(gWindow, reshape);
		glfwSetMouseButtonCallback(gWindow, mouseButton);
		glfwSetCursorPosCallback(gWindow, cursorPos);
		glfwSetCharCallback(gWindow, charCall);
		/*glfwSetScrollCallback(gWindow, scroll_callback);*/
		
		return true;
}


	void initShaders() {
		
		glslProgramVolume = new CGLSLProgram();
		glslProgramVolume->loadShader("../shaders/shader_volume.vert", CGLSLProgram::VERTEX);
		glslProgramVolume->loadShader("../shaders/shader_volume.frag", CGLSLProgram::FRAGMENT);
		glslProgramVolume->create_link();

		glslProgramBackFace = new CGLSLProgram();
		glslProgramBackFace->loadShader("../shaders/backface.vert", CGLSLProgram::VERTEX);
		glslProgramBackFace->loadShader("../shaders/backface.frag", CGLSLProgram::FRAGMENT);
		glslProgramBackFace->create_link();

		glslProgramTexture = new CGLSLProgram();
		glslProgramTexture->loadShader("../shaders/texture.vert", CGLSLProgram::VERTEX);
		glslProgramTexture->loadShader("../shaders/texture.frag", CGLSLProgram::FRAGMENT);
		glslProgramTexture->create_link();

	}


	GLuint planeVAO, planeVBO;

	void createPane() {
		glslProgramTexture->enable();
		GLfloat planeVertex[] =
		{
			2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
			-2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
			-2.0f, -2.0f, 0.0f, 0.0f, 0.0f,

			2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
			2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
			-2.0f, 2.0f, 0.0f, 0.0f, 1.0f
		};

		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertex), &planeVertex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
		glslProgramTexture->disable();
	}


	void Init() {
		if (!initGlfw() || glewInit() != GLEW_OK)
			exit(1);

		/*init Shaders*/
		printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		initShaders();
		//AQUIIIIIII
		for (int i = 0; i < volumenes.size(); i++)
		{
			volumenes[i]->initVBO();
			volumenes[i]->initTFF1DTex("../images/tff.dat");
			volumenes[i]->initFace2DTex(gWidth, gHeight);
		}			
		volumenes[0]->initVol3DTex("../images/head256.raw", 256, 256, 225);
		volumenes[1]->initVol3DTex("../images/bonsai.raw", 256, 256, 256);
		volumenes[2]->initVol3DTex("../images/skull.raw", 256, 256, 256);

		for (int i = 0; i < volumenes.size(); i++)
		{
			volumenes[i]->initFrameBuffer(gWidth, gHeight);
		}
		transfer_func = new Transfer();

		Texture* textura = new Texture("../images/coords.png");
		texture_coord = textura->getID();	
		createPane();
		reshape(gWindow, gWidth, gHeight);		
	}

	bool selected = true;
	bool rebote = false;
	float velocidad_rebote = 0.2f;
	float velocidad_carro = 0.1f;
	float velocidad_elemental = -0.01f;
	float elem_rotation = 0.01f;
	float sword_rotation = 0.01f;
	float contador = 0.0f, contador_carro = 0.0f, contador_elemental = 0.0f, contador_sword = 0.0f;
	float velocidad_sword = 0.3f;
	bool controlador = false;
	float aux_car1, aux_car2 = 0.0f;
	bool primera_vez = true;
	float car_rotation = 0.1f;


	void update_Interface() {
			volumenes[entitieSelected]->Transform->setScaleMatrix(Scale);
			volumenes[entitieSelected]->Transform->setRotationMatrix(glm::toMat4(glm::normalize(glm::quat(Rotation[3], Rotation[0], Rotation[1], Rotation[2]))));
			volumenes[entitieSelected]->Transform->setTraslationMatrix(TranslationSpeedX, TranslationSpeedY, TranslationSpeedZ);
			for (int i = 0; i < transfer_func->puntos.size(); i++)
			{
				if (transfer_func->puntos[i]->selected) {
					transfer_func->puntos[i]->r = rgb[0];
					transfer_func->puntos[i]->g = rgb[1];
					transfer_func->puntos[i]->b = rgb[2];
				}

			}
			
			
	}

	void paintPlane(GLuint Texture) {

		glColor3f(1.0f, 1.0f, 1.0f);
		glPointSize(9.0f);
		glBegin(GL_POINTS);

		//glVertex3f(0.94, 0.78, 0.0);
		//glVertex3f(0.65, 0.47, 0.0);

		for (int i = 0; i < transfer_func->puntos.size(); i++)
		{
			if (transfer_func->puntos[i]->selected) {
				glColor3f(0.0f, 1.0f, 0.0f);
				float x =  ( (transfer_func->puntos[i]->x) * (0.92 - 0.64) ) + 0.64;
				float y =  ( (transfer_func->puntos[i]->y) * (0.76 - 0.47) ) + 0.47;
				glVertex3f(x, y, 0.0);
				glColor3f(1.0f, 1.0f, 1.0f);
			}else {
				float x = ((transfer_func->puntos[i]->x) * (0.92 - 0.64)) + 0.64;
				float y = ((transfer_func->puntos[i]->y) * (0.76 - 0.47)) + 0.47;
				glVertex3f(x, y, 0.0);
			}
		}
		glEnd();	

		glColor3f(0.4f, 0.5f, 1.0f);
		glPointSize(5.0f);
		glBegin(GL_LINE_STRIP);
		//glVertex3f(0.65, 0.47, 0.0);
				
		for (int i = 0; i < transfer_func->puntos.size(); i++)
		{
			float x = ((transfer_func->puntos[i]->x) * (0.92 - 0.64)) + 0.64;
			float y = ((transfer_func->puntos[i]->y) * (0.76 - 0.47)) + 0.47;
			glVertex3f(x, y, 0.0);
		}
		//glVertex3f(0.94, 0.78, 0.0);
		glEnd();

		glslProgramTexture->enable();
		mat4 model = glm::scale(vec3(0.08, 0.08, 0.08)) * glm::translate(vec3(8.5, 4.8, 0.1)) * mat4(1.0f);
		mat4 view = lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 projection = glm::perspective(45.0f, (float)gWidth / (float)gHeight, 0.1f, 1000.0f);

		ID->at(0) = glGetUniformLocation(glslProgramTexture->getProgramID(), "view");
		glUniformMatrix4fv(ID->at(0), 1, GL_FALSE, &(view)[0][0]);

		ID->at(1) = glGetUniformLocation(glslProgramTexture->getProgramID(), "projection");
		glUniformMatrix4fv(ID->at(1), 1, GL_FALSE, &(projection)[0][0]);

		ID->at(2) = glGetUniformLocation(glslProgramTexture->getProgramID(), "model");
		glUniformMatrix4fv(ID->at(2), 1, GL_FALSE, &(model)[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(glGetUniformLocation(glslProgramTexture->getProgramID(), "texture1"), 0);

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glslProgramTexture->disable();

	}

	void display() {		
		update_Interface();

		

		GLuint aux;		
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < volumenes.size(); i++)
		{
			glslProgramBackFace->enable();
			shader_id = glslProgramBackFace->getProgramID();
			glBindFramebuffer(GL_FRAMEBUFFER, volumenes[i]->g_frameBuffer);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			aux = glGetUniformLocation(shader_id, "u_viewMat");
			glUniformMatrix4fv(aux, 1, GL_FALSE, &(camera->getMatrix())[0][0]);
			aux = glGetUniformLocation(shader_id, "u_projMat");
			glUniformMatrix4fv(aux, 1, GL_FALSE, &(mProjMatrix)[0][0]);
			volumenes[i]->Render(GL_FRONT, shader_id);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glslProgramBackFace->disable();
		}
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		paintPlane(texture_coord);
		transfer_func->Interpolation();
		GLuint text1d = transfer_func->BindTexture();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glslProgramVolume->enable();
		shader_id = glslProgramVolume->getProgramID();		
		aux = glGetUniformLocation(shader_id, "eye_position");
		glUniform3f(aux, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
		aux = glGetUniformLocation(shader_id, "u_viewMat");
		glUniformMatrix4fv(aux, 1, GL_FALSE, &(camera->getMatrix())[0][0]);		
		aux = glGetUniformLocation(shader_id, "u_projMat");
		glUniformMatrix4fv(aux, 1, GL_FALSE, &(mProjMatrix)[0][0]);
		aux = glGetUniformLocation(shader_id, "ScreenSize");
		glUniform2f(aux, (float)gWidth, (float)gHeight);
		aux = glGetUniformLocation(shader_id, "StepSize");
		glUniform1f(aux, 0.001f);
		for (int i = 0; i < volumenes.size(); i++)
		{
			volumenes[i]->ttf1DTex = text1d;
			volumenes[i]->Render(GL_BACK, shader_id);
		}
		glslProgramVolume->disable();
		glDisable(GL_BLEND);
	}


	void iniciarPosiciones() {
		volumenes[0]->Transform->setTraslationMatrix(0.6f, -0.1f, 0.6f);
		volumenes[1]->Transform->setTraslationMatrix(-0.1f, 2.1f, -2.9f);
		volumenes[2]->Transform->setTraslationMatrix(3.0f, 2.4f, -4.3f);

	
	}

	int infinity() {
		double time = 0.0;
		double aux;
		int contador = 0;
	    iniciarPosiciones();
		while (!glfwWindowShouldClose(gWindow)) {			
			aux = glfwGetTime();
			contador++;
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			
			display();
			TwDraw();			
			glfwSwapBuffers(gWindow);
			glfwPollEvents();
			time = (glfwGetTime() - aux) + time;	
			if (time >= 1.0) {
				time = 0.0;
				contador = 0;
			}
		}
		return EXIT_SUCCESS;
	}

	void Init_Models() {

		Volume *volume = new Volume();
		volumenes.push_back(volume);

		Volume *volume2 = new Volume();
		volumenes.push_back(volume2);

		Volume *volume3 = new Volume();
		volumenes.push_back(volume3);

		

	}

	int main() {
		luces_on = true;
		Init_Models(); // Leo los modelos y los agrego al vector entidades
		Init();        // Inicializo todos los parámetros y las entidades
		InitAnt();
		infinity();    // Bucle infinito
		return 0;
	}