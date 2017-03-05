#include "main.h"

bool shadowMap;
glm::mat4 lightSpaceMatrix;
/* STAGE */
Camera *camera = new Camera();
vector<Model*> entidades;
GLuint shader_id, shader_id_vert;
TwBar *principalBar;
FrameBuffer* frameBuffer;
typedef enum { COOK, PHONG, BLINN } Specular;
Specular specular = COOK;
TwEnumVal typeSpecular[] = { { COOK, "Cook" },{ PHONG, "Phong" },{ BLINN, "Blinn" } };
TwType specularType;

typedef enum { LAMBERT, OREN } Difuse;
Difuse difuse = LAMBERT;
TwEnumVal typeDifuse[] = { { LAMBERT, "Lambert" },{ OREN, "Oren-nayar" } };
TwType difuseType;
	float select = 1.0f;
	float TranslationSpeedX, TranslationSpeedY, TranslationSpeedZ;
	float TranslationSpeedX_Light, TranslationSpeedY_Light, TranslationSpeedZ_Light;
	float Scale, Rotation[4];
	float g_LightDirection[] = { 0.02f, -0.83f, -0.57f };
	float refract = 1.0f;
	float lightAmbientIntensity[3], lightDiffuseIntensity[3], lightSpecularIntensity[3];
	float matAmbientReflectances[3], matDiffuseReflectances[3], matSpecularReflectances[3], matShininess;
	float roughness, fresnel, albedo, interpolacion, texture_active;
	float atConstant, atLineal, atCuadratic, exponent, spotcos;
	float Ldirect = 0.0f, Lpoint=0.0f, Lspot=1.0f;
	bool luces_on;	
	int entitieSelected = 1;
	bool animacion_on = true;

	void TW_CALL LuzON(void * clientData) {
		if (luces_on) {
			luces_on = false;
		}
		else {
			luces_on = true;
		}

	}

	void TW_CALL Select(void * clientData) {
		entitieSelected = (int) select;
		TranslationSpeedX = entidades[entitieSelected]->transformation->getTraslationMatrix()[3][0];
		TranslationSpeedY = entidades[entitieSelected]->transformation->getTraslationMatrix()[3][1];
		TranslationSpeedZ = entidades[entitieSelected]->transformation->getTraslationMatrix()[3][2];		
		Scale = entidades[entitieSelected]->transformation->getScaleMatrix()[0][0];
		glm::quat aux = glm::quat(entidades[entitieSelected]->transformation->getRotationMatrix());
		Rotation[0] = aux.x; Rotation[1] = aux.y; Rotation[2] = aux.z; Rotation[3] = aux.w;
		vec3 ambient_aux = entidades[entitieSelected]->material->getAmbient();
		matAmbientReflectances[0] = ambient_aux.x; matAmbientReflectances[1] = ambient_aux.y; matAmbientReflectances[2] = ambient_aux.z;
		ambient_aux = entidades[entitieSelected]->material->getDiffuse();
		matDiffuseReflectances[0] = ambient_aux.x; matDiffuseReflectances[1] = ambient_aux.y; matDiffuseReflectances[2] = ambient_aux.z;
		ambient_aux = entidades[entitieSelected]->material->getSpecular();
		matSpecularReflectances[0] = ambient_aux.x; matSpecularReflectances[1] = ambient_aux.y; matSpecularReflectances[2] = ambient_aux.z;
		matShininess = entidades[entitieSelected]->getShininess();
		refract = entidades[entitieSelected]->getRatioRefract();
		roughness = entidades[entitieSelected]->getRoughness();
		fresnel = entidades[entitieSelected]->getFresnel();
		albedo = entidades[entitieSelected]->getAlbedo();
	}

	void TW_CALL LuzDireccional(void * clientData) {
		Ldirect = 1.0f;
		Lpoint = 0.0f;
		Lspot = 0.0f;
		shadowMap = true;
	}

	void TW_CALL LuzPuntual(void * clientData) {
		Ldirect = 0.0f;
		Lpoint = 1.0f;
		Lspot = 0.0f;
		shadowMap = false;
	}

	void TW_CALL LuzSpot(void * clientData) {
		Ldirect = 0.0f;
		Lpoint = 0.0f;
		Lspot = 1.0f;
		shadowMap = true;
	}
	

	void TW_CALL Animate(void * clientData) {
		if (animacion_on) {
			animacion_on = false;
		}
		else {
			animacion_on = true;
		}
	}

	void TW_CALL ReflectActive(void * clientData) {
		if (entidades[entitieSelected]->getActiveReflect() == 0.0f) {
			entidades[entitieSelected]->setActiveReflect(1.0f);
			entidades[entitieSelected]->setActiveRefract(0.0f);
		}
		else {
			entidades[entitieSelected]->setActiveReflect(0.0f);
		}
	}


	void TW_CALL RefractActive(void * clientData) {
		if ( entidades[entitieSelected]->getActiveRefract() == 0.0f) {
			entidades[entitieSelected]->setActiveReflect(0.0f);
			entidades[entitieSelected]->setActiveRefract(1.0f);
		}
		else {
			entidades[entitieSelected]->setActiveRefract(0.0f);
		}
	}



	void InitAnt() {
		Scale = 6.00f;
		/*for (int i = 0; i < 4; i++)
		{
			Rotation[i] = 0.0f;
		}*/
		shadowMap = true;
		Rotation[0] = 0.0f; Rotation[1] = -1.0f; Rotation[2] = 0.0f; Rotation[3] = 1.0f;
		

		matAmbientReflectances[0] = 1.0f;
		matAmbientReflectances[1] = 0.8f;
		matAmbientReflectances[2] = 0.2f;
		matDiffuseReflectances[0] = 1.0f;
		matDiffuseReflectances[1] = 0.82f;
		matDiffuseReflectances[2] = 0.68f;
		matSpecularReflectances[0] = 1.0f;
		matSpecularReflectances[1] = 1.0f;
		matSpecularReflectances[2] = 1.0f;


		lightAmbientIntensity[0] = 1.0f;
		lightAmbientIntensity[1] = 1.0f;
		lightAmbientIntensity[2] = 1.0f;
		lightDiffuseIntensity[0] = 1.0f;
		lightDiffuseIntensity[1] = 1.0f;
		lightDiffuseIntensity[2] = 1.0f;
		lightSpecularIntensity[0] = 1.0f;
		lightSpecularIntensity[1] = 1.0f;
		lightSpecularIntensity[2] = 1.0f;

		texture_active = 1.0f;
		interpolacion = 0.0f;
		roughness = 0.1f;
		fresnel = 0.5f;
		albedo = 0.5f;

		atConstant = 0.1f;
		atLineal = 0.1f;
		atCuadratic = 0.1f;
		exponent = 0.1f;
		spotcos = 0.9f;

		TranslationSpeedX_Light = 2.4f;
		TranslationSpeedY_Light = 15.0f;
		TranslationSpeedZ_Light = 22.8f;
		
		TranslationSpeedX = 3.4;
		TranslationSpeedY = 1.8f;
		TranslationSpeedZ = 6.9f;
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
		TwAddVarRW(principalBar, "MatAmbientReflectances", TW_TYPE_COLOR3F, &matAmbientReflectances, "group='Material' label='Ambient Reflectances' opened=true help='Change the material ambient reflectances.' ");
		TwAddVarRW(principalBar, "MatDiffuseReflectances", TW_TYPE_COLOR3F, &matDiffuseReflectances, "group='Material' label='Diffuse Reflectances' opened=true help='Change the material diffuse reflectances.' ");
		TwAddVarRW(principalBar, "MatSpecularReflectances", TW_TYPE_COLOR3F, &matSpecularReflectances, "group='Material' label='Specular Reflactances' opened=true help='Change the material specular reflectances.' ");
		TwDefine("Model/Material opened=false");
		TwAddSeparator(principalBar, "", NULL);
		TwAddVarRW(principalBar, "Fragm: 0 | Vertex: 1 ", TW_TYPE_FLOAT, &interpolacion, "min=0.0 max=1.0 step=1.0 group='Interpolacion'");
		TwDefine("Model/Interpolacion group=Shaders ");
		TwAddVarRW(principalBar, "OFF: 0 | ON: 1 ", TW_TYPE_FLOAT, &texture_active, "min=0.0 max=1.0 step=1.0 group='Texture'");
		TwAddButton(principalBar, "Refract ON_OFF", RefractActive, NULL, "group='Refract'");
		TwAddButton(principalBar, "Reflect ON_OFF", ReflectActive, NULL, "group='Reflect'");

		specularType = TwDefineEnum("SpecularType", typeSpecular, 3);
		TwAddVarRW(principalBar, "Specular", specularType, &specular, NULL);
		difuseType = TwDefineEnum("DifuseType", typeDifuse, 2);
		TwAddVarRW(principalBar, "Difuse", difuseType, &difuse, NULL);
		TwDefine("Model/Specular group=Active|Desactive");
		TwDefine("Model/Difuse group=Active|Desactive");
		TwDefine("Model/Texture group=Active|Desactive");
		TwDefine("Model/Refract group=Active|Desactive");
		TwDefine("Model/Reflect group=Active|Desactive");
		TwDefine("Model/Active|Desactive group=Shaders");
		TwDefine("Model/Shaders opened=false");

		TwAddSeparator(principalBar, "", NULL);
		TwAddVarRW(principalBar, "MatShininess", TW_TYPE_FLOAT, &matShininess, " min=0.1 max=100.0 step=0.1 group='Settings' label='Shininess factor' help='Change the shininess factor.'");		
		TwAddVarRW(principalBar, "Fresnel", TW_TYPE_FLOAT, &fresnel, "min=0.1 max=0.9 step=0.1 group='Settings'");
		TwAddVarRW(principalBar, "Roughness", TW_TYPE_FLOAT, &roughness, "min=0.1 max=0.9 step=0.1 group='Settings'");
		TwAddVarRW(principalBar, "Albedo", TW_TYPE_FLOAT, &albedo, "min=0.1 max=0.9 step=0.1 group='Settings'");
		TwAddVarRW(principalBar, "RefractRatio", TW_TYPE_FLOAT, &refract, "min=1.0 max=5.0 step=0.1 group='Settings'");
		TwDefine("Model/Settings opened=false");
		TwAddSeparator(principalBar, "", NULL);


		/*Parameters of light*/
		TwAddVarRW(principalBar, "LightAmbientIntensity", TW_TYPE_COLOR3F, &lightAmbientIntensity, "group='Light' label='Ambient Intensity' opened=true help='Change the light ambient intensity.' ");
		TwAddVarRW(principalBar, "LightDiffuseIntensity", TW_TYPE_COLOR3F, &lightDiffuseIntensity, "group='Light' label='Diffuse Intensity' opened=true help='Change the light diffuse intensity.' ");
		TwAddVarRW(principalBar, "LightSpecularIntensity", TW_TYPE_COLOR3F, &lightSpecularIntensity, "group='Light' label='Specular Intensity' opened=true help='Change the light specular intensity.' ");
		TwDefine("Model/Light opened=false");
		TwDefine("Model/Light group=SettingLight");
		

		TwAddVarRW(principalBar, "TranslationSpeedX_Light", TW_TYPE_FLOAT, &TranslationSpeedX_Light, " min=-10000 max=10000 step=0.1 group='Translation_Light' label='Translation factor X' ");
		TwAddVarRW(principalBar, "TranslationSpeedY_Light", TW_TYPE_FLOAT, &TranslationSpeedY_Light, " min=-10000 max=10000 step=0.1 group='Translation_Light' label='Translation factor Y' ");
		TwAddVarRW(principalBar, "TranslationSpeedZ_Light", TW_TYPE_FLOAT, &TranslationSpeedZ_Light, " min=-10000 max=10000 step=0.1 group='Translation_Light' label='Translation factor Z' ");
		TwDefine("Model/Translation_Light group=SettingLight opened=false");

		TwAddVarRW(principalBar, "Spot Dir", TW_TYPE_DIR3F, &g_LightDirection, " group='LightDir' label='Light direction' help='Change the light direction.' ");		
		TwDefine("Model/LightDir group=SettingLight opened=false");

		TwAddVarRW(principalBar, "Atenuacion Constante", TW_TYPE_FLOAT, &atConstant, " min=0.1 max=1.0 step=0.1 group='Light_Parameters'");
		TwAddVarRW(principalBar, "Atenuacion Lineal", TW_TYPE_FLOAT, &atLineal, "min=0.1 max=1.0 step=0.1 group='Light_Parameters'");
		TwAddVarRW(principalBar, "Atenuacion Cuadratica", TW_TYPE_FLOAT, &atCuadratic, "min=0.1 max=1.0 step=0.1 group='Light_Parameters'");
		TwAddVarRW(principalBar, "Exponente", TW_TYPE_FLOAT, &exponent, "min=0.1 max=100.0 step=0.1 group='Light_Parameters'");
		TwAddVarRW(principalBar, "SpotCosCutoff", TW_TYPE_FLOAT, &spotcos, "min=-1.0 max=1.0 step=0.1 group='Light_Parameters'");
		TwAddButton(principalBar, "Luz ON_OFF", LuzON, NULL, "group='Light_Parameters'");
		TwDefine("Model/Light_Parameters opened=false group=SettingLight");

		TwAddButton(principalBar, "Luz Direccional", LuzDireccional, NULL, "group='Type_Light'");
		TwAddButton(principalBar, "Luz Puntual", LuzPuntual, NULL, "group='Type_Light'");
		TwAddButton(principalBar, "Luz Spot", LuzSpot, NULL, "group='Type_Light'");
		TwDefine("Model/Type_Light group=SettingLight opened=false");
		TwDefine("Model/SettingLight opened=false");
		TwAddSeparator(principalBar, "", NULL);

		TwAddButton(principalBar, "Animaciones", Animate, NULL, "group='Animate'");
		TwDefine("Model/Animate group=Animation");
		TwDefine("Model/Animation opened=false");
		TwAddSeparator(principalBar, "", NULL);

		TwAddVarRW(principalBar, "Index", TW_TYPE_FLOAT, &select, "min=1.0 max=7.0 step=1.0 group='Select'");
		TwAddButton(principalBar, "Seleccionar objeto", Select, NULL, "group='Select'");
		TwDefine("Model/Select opened=false");
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

	bool firstMouse = true;
	bool click = false;
	GLfloat lastX = 0, lastY = 0;
	GLfloat pitch = 0.0f, yaw = -90.0f;
	GLfloat fov = 45.0f;
	bool MousePicking = false;
	glm::vec3 ray = glm::vec3(1.0f,1.0f,1.0f);
	glm::vec3 punto = glm::vec3(1.0f, 1.0f, 1.0f);
	void cursorPos(GLFWwindow* window, double xpos, double ypos) {
		if (TwEventMousePosGLFW(int(xpos), int(ypos)))
			return;
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		/*if (MousePicking) {

	
			float x = 2.0f * (float)xpos / float(gWidth) - 1.0f;
			float y = 1 - 2.0f * (float)ypos / float(gHeight);
			glm::vec4 clipCoords = glm::vec4(x, y, 0.0f, 1.0f);

			glm::vec4 eyeCoords = glm::inverse(mProjMatrix) * clipCoords;
			glm::vec4 eye = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
			glm::vec4 rayWorld = glm::inverse(mModelViewMatrix) * eye;
			ray = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
			ray = glm::normalize(ray);
			//cout <<"("<< ray.x << "," << ray.y << "," << ray.z << ")" << endl;
			MousePicking = false;
			//system("pause");
		/*	if (ray.x >= entidades[0]->min_aux.x && ray.x <= entidades[0]->max_aux.x && ray.y >= entidades[0]->min_aux.y && ray.y <= entidades[0]->max_aux.y && ray.z >= entidades[0]->min_aux.z && ray.z <= entidades[0]->max_aux.z) {
				cout << "ESTOY HACIENDO PICK EN EL OBJETO" << endl;
			}*/
			
		/*	glm::vec3 rayo = ray;
			punto = camera->getPosition();
		//	u_projMat * u_viewMat;
		//	glm::mat4x4 mProjMatrix, mModelViewMatrix;
			glm::vec4 position_min = mProjMatrix * mModelViewMatrix * entidades[1]->transformation->getTransformMatrix() * glm::vec4(entidades[1]->min.x, entidades[1]->min.y, entidades[1]->min.z, 1.0f);
			glm::vec4 position_max = mProjMatrix * mModelViewMatrix * entidades[1]->transformation->getTransformMatrix() * glm::vec4(entidades[1]->max.x, entidades[1]->max.y, entidades[1]->max.z, 1.0f);
			for (float i = 0.0f; i < 1000000.0f; i = i + 1.0f)
			{
				punto =  punto + (rayo * 1.001f);
				ray = punto;
				//cout << ray.x << "," << ray.y << "," << ray.z << " | DEBE SER MENOR A: " << position_max.x << "," << position_max.y << "," << position_max.z << endl;
				//cout << ray.x << "," << ray.y << "," << ray.z << " | DEBE SER MAYOR A: " << position_min.x << "," << position_min.y << "," << position_min.z << endl;
				//system("pause");
				if (ray.x >= position_min.x && ray.x <= position_max.x && ray.y >= position_min.y && ray.y <= position_max.y && ray.z >= position_min.z && ray.z <= position_max.z) {
					cout << "ESTOY HACIENDO PICK EN EL OBJETO" << endl;
				}
			}
			cout << "SALI" << endl;
			firstMouse = true;
			return;
		}*/

		if (!click) {
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
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
			click = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			MousePicking = true;
		}		
	}


	/*struct GLFWwindow {};
	typedef void(*GLFWcharfun) (GLFWwindow *, unsigned int);

	GLFWcharfun glfwSetCharCallback(GLFWwindow *    window,
		GLFWcharfun    cbfun
	);*/

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
		

		glslProgramTexture_vertex = new CGLSLProgram();
		glslProgramTexture_vertex->loadShader("../shaders/shader_vertex.vert", CGLSLProgram::VERTEX);
		glslProgramTexture_vertex->loadShader("../shaders/shader_vertex.frag", CGLSLProgram::FRAGMENT);
		glslProgramTexture_vertex->create_link();
		//shader_id_vert = glslProgramTexture_vertex->getProgramID();

		glslProgramTexture = new CGLSLProgram();
		glslProgramTexture->loadShader("../shaders/shader_fragment.vert", CGLSLProgram::VERTEX);
		glslProgramTexture->loadShader("../shaders/shader_fragment.frag", CGLSLProgram::FRAGMENT);
		glslProgramTexture->create_link();	

		glslProgramSkybox = new CGLSLProgram();
		glslProgramSkybox->loadShader("../shaders/skybox.vert", CGLSLProgram::VERTEX);
		glslProgramSkybox->loadShader("../shaders/skybox.frag", CGLSLProgram::FRAGMENT);
		glslProgramSkybox->create_link();

		glslProgramShadow = new CGLSLProgram();
		glslProgramShadow->loadShader("../shaders/shadow.vert", CGLSLProgram::VERTEX);
		glslProgramShadow->loadShader("../shaders/shadow.frag", CGLSLProgram::FRAGMENT);
		glslProgramShadow->create_link();


		texture_program = new CGLSLProgram();
		texture_program->loadShader("../shaders/texture.vert", CGLSLProgram::VERTEX);
		texture_program->loadShader("../shaders/texture.frag", CGLSLProgram::FRAGMENT);
		texture_program->create_link();		

		texture_program->enable();

		texture_program->addAttribute("position");
		texture_program->addAttribute("texcoord");

		texture_program->disable();
	}

	void display_shadow() {
		mat4 aux;
		mat4 proy;
		GLfloat near_plane = -20.0f, far_plane = 50.0f;
		vec3 LuzCentro = vec3(entidades[0]->getCenter_2());
		vec3 Dir = entidades[0]->material->getDir();
		if (shadowMap) {
			//aux = glm::lookAt(vec3(entidades[0]->getCenter_2()), entidades[0]->material->getDir(),	glm::vec3(0.0f, 1.0f, 0.0f));
			aux = glm::lookAt(LuzCentro, LuzCentro  + Dir , glm::vec3(0.0f, 1.0f, 0.0f));
			//proy = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
		}
		else {
			aux = glm::lookAt(vec3(entidades[0]->getCenter_2()),
				//glm::vec3(0.0f, 0.0f, 10.0f),
				glm::vec3(0.0f, -1.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
			
			
		}
		proy = glm::ortho(15.0f, -15.0f, -15.0f, 15.0f, near_plane, far_plane);
		

		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
		depthBiasMVP = biasMatrix* proy * aux;

		ID->at(0) = glGetUniformLocation(shader_id, "u_projMat");
		glUniformMatrix4fv(ID->at(0), 1, GL_FALSE, &(proy)[0][0]);

		ID->at(1) = glGetUniformLocation(shader_id, "u_viewMat");
		glUniformMatrix4fv(ID->at(1), 1, GL_FALSE, &(aux)[0][0]);

		for (int i = 0; i < entidades.size(); i++)
		{
			entidades[i]->render_shadow(shader_id);
		}

	}

	void RenderDepthTexture() {

		//glEnable(GL_DEPTH_TEST);
		GLuint text_id = frameBuffer->getImageDepthTexture();
		frameBuffer->bindImageFrameBuffer();
		glClear(GL_DEPTH_BUFFER_BIT);
		frameBuffer->activeImageDepthTexture();
		display_shadow();
		frameBuffer->unbindCurrentFrameBuffer();
		//glDisable(GL_DEPTH_TEST);
		texture_depth = text_id;
	}

	GLuint RenderCubeDynamic(glm::vec4 Center) {
		Camera* Aux = new Camera(vec3(Center), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 mProjMatrix2 = glm::perspective(90.0f, (float)gWidth / (float)gHeight, 1.0f, 1000.0f);
		GLuint text_id = frameBuffer->getCubeMapColorTexture();
		frameBuffer->bindCubeMapFrameBuffer();
		
		for (int i = 0; i < 6; i++)
		{
			frameBuffer->activeCubeMapColorTexture(i);
			Aux->setFace(i);
			display_low(Aux, mProjMatrix2);
			//RENDER BAJA CALIDAD
		}
		frameBuffer->unbindCurrentFrameBuffer();
		
		return text_id;
	}
	GLuint planeVAO, planeVBO;

	void createPane() {
		//texture_program->enable();
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
		//texture_program->disable();
	}


	void Init() {
		if (!initGlfw() || glewInit() != GLEW_OK)
			exit(1);

		/*init Shaders*/
		printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		initShaders();
		Skybox = new skybox();
		Skybox->setProgram(glslProgramSkybox);
		for (int i = 0; i < (int)entidades.size(); i++) {
			entidades[i]->initGLDataBinding();
		}
		frameBuffer = new FrameBuffer();
		createPane();
		entidades[0]->transformation->setScaleMatrix(1.5f);
		entidades[0]->transformation->setTraslationMatrix(0.0f, 10.0f, 5.0f);		
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
	void animaciones() {
		if (!animacion_on) return;

		/*for (int i = 0; i < entidades.size(); i++)
		{
			entidades[i]->transformation->setScale_negative();
		}*/

		//cout << Rotation[0] << " | " << Rotation[1] << " | " << Rotation[2] << " | "<< Rotation[3] << endl;
		//REBOTE DE PELOTA		
			if (contador >= 8.0f && !rebote && !controlador) {
				velocidad_rebote = 0.18f;
			}
			else if (contador >= 8.0f && contador < 12.0f && rebote) {
				velocidad_rebote = 0.1f;
			}
			else if(contador >= 12.0f && rebote) {
				velocidad_rebote -= 0.0008f;
				if (velocidad_rebote <= 0.0f) {
					rebote = false;
					velocidad_rebote = 0.02f;
					controlador = true;
					contador = 0.0f;
				}
			}
			if (contador <= 12.0f && !rebote && controlador) {
				velocidad_rebote += 0.0008f;
			}
			if (!rebote) {
				entidades[3]->transformation->addTraslationMatrix(0.0f, -velocidad_rebote, 0.0f);
				contador = contador + velocidad_rebote;
			}
			else {
				entidades[3]->transformation->addTraslationMatrix(0.0f, velocidad_rebote, 0.0f);
				contador = contador + velocidad_rebote;
			}
			if (!rebote && entidades[3]->transformation->getTraslationMatrix()[3][1] <= 1.5f) {
				rebote = true;
				velocidad_rebote = 0.15f;
				contador = 0.0f;
				controlador = false;
			}
			else if (rebote && entidades[3]->transformation->getTraslationMatrix()[3][1] >= 25.0f) {
				rebote = false;			
				velocidad_rebote = 0.1f;
				contador = 0.0f;
			}
		// FIN DE REBOTE DE PELOTA


	    // Movimiento de carro
			if (contador_carro <= 10.0f) {
				entidades[4]->transformation->addTraslationMatrix(0.0f, 0.0f, -velocidad_carro);
				contador_carro += glm::abs(velocidad_carro);
			}
			else if(contador_carro > 10.0f && contador_carro < 14.0f) {
				entidades[4]->transformation->addTraslationMatrix(0.0f, 0.0f, velocidad_carro/2);
				contador_carro += glm::abs(velocidad_carro/2);
				//Rotar EN -X
				car_rotation = car_rotation + 0.115f;
				//entidades[4]->transformation->addTraslationMatrix(0.0f, 0.0f, velocidad_carro );
				entidades[4]->transformation->setRotationMatrix(0.0f, 0.0f, car_rotation, 0.0f);
				contador_carro += glm::abs(velocidad_carro);
			}			

			if (contador_carro >= 14.0f) {
				
				bool is_negative = velocidad_carro < 0;
				if (is_negative) {
					if (primera_vez) {
						aux_car1 = car_rotation;
						primera_vez = false;
					}
					velocidad_carro = 0.1f;
					car_rotation = aux_car1;
					
				}
				else {
					velocidad_carro = -0.1f;
					if (primera_vez) {
						aux_car2 = car_rotation;
					}
					car_rotation = aux_car2;
				}
					
				contador_carro = 0.0f;
			}
			//ELEMENTAL
			
				elem_rotation += 0.01f;
				entidades[5]->transformation->setRotationMatrix(0.0f, 0.0f, elem_rotation ,0.0f);
					
				//PRUEBA ROTACION CON LA PELOTA FUNCIONA ! <3
				//elem_rotation += 0.01f;				
				entidades[3]->transformation->setRotationMatrix(0.0f, elem_rotation * 1.5f, elem_rotation * 0.5f, elem_rotation * 1.3f);


			//SWORD ANIMATION

				if (contador_sword <= 20.0f) {
					entidades[6]->transformation->addTraslationMatrix(0.0f, velocidad_sword/2, -velocidad_sword/2);
					contador_sword += velocidad_sword;
					sword_rotation += 0.01f;
					entidades[6]->transformation->setRotationMatrix(0.0f, 1.928791f, -1.566111f, -0.226048f + sword_rotation);
				}
				else if (contador_sword > 20.0f && contador_sword <= 25.0f) {
					entidades[6]->transformation->addTraslationMatrix(0.0f, velocidad_sword / 4, -velocidad_sword / 4);
					contador_sword += velocidad_sword/2;
					sword_rotation += 0.01f/2;
					entidades[6]->transformation->setRotationMatrix(0.0f, 1.928791f, -1.566111f, -0.226048f + sword_rotation );
				}
				else if (contador_sword > 25.0f && contador_sword <= 30.0f) {
					sword_rotation += 0.01f/2;
					entidades[6]->transformation->setRotationMatrix(0.0f, 1.928791f, -1.566111f, -0.226048f + sword_rotation);
					entidades[6]->transformation->addTraslationMatrix(0.0f, -velocidad_sword / 4, -velocidad_sword / 4);
					contador_sword += velocidad_sword/2;
				
				}
				else if(contador_sword > 30.0f && contador_sword <=48.8f) {
					sword_rotation += 0.01f;
					entidades[6]->transformation->setRotationMatrix(0.0f, 1.928791f, -1.566111f, -0.226048f + sword_rotation);
					entidades[6]->transformation->addTraslationMatrix(0.0f, -velocidad_sword / 2, -velocidad_sword / 2);
					contador_sword += velocidad_sword;
				}

				if (contador_sword > 48.8f && contador_sword<=60.0f) { //DO NOTHING 					
					contador_sword += velocidad_sword;
				}
				else if(contador_sword > 60.0f) {
					contador_sword = 0.0f;
					entidades[6]->transformation->setTraslationMatrix(-9.2f, 0.5f, 14.4f);
					entidades[6]->transformation->setRotationMatrix(0.0f, 1.928791f, -1.566111f, -0.226048f);
					sword_rotation = 0.01f;
				}




	
	}

	void iniciarPosiciones() {

		//PISO
		entidades[2]->transformation->setScaleMatrix(55.1f);
		entidades[2]->transformation->setTraslationMatrix(-33.5f, -1.5f, 2.6f);

		//ESFERA
		entidades[3]->transformation->setTraslationMatrix(-5.0f, 25.0f, -10.0f);
		entidades[3]->transformation->setScaleMatrix(3.0f);		
		entidades[3]->setActiveRefract(1.0f);
		//CARRO
		entidades[4]->transformation->setScaleMatrix(3.7f);
		entidades[4]->transformation->setTraslationMatrix(13.4f, 0.4f, 12.8);

		//ELEMENTAL
		entidades[5]->transformation->setScaleMatrix(12.0f);
		entidades[5]->transformation->setTraslationMatrix(3.8f, 8.2f, -3.6f);
		entidades[5]->transformation->setRotationMatrix(0.0f, 0.0f, -3.0f, 0.0f);

		//SWORD
		entidades[6]->transformation->setScaleMatrix(6.0);
		entidades[6]->transformation->setTraslationMatrix(-9.2f, 0.5f, 14.4f);
		entidades[6]->transformation->setRotationMatrix(0.0f, 1.928791f, -1.566111f, -0.226048f);

		//COLORES:
		float ambient[3];
		float diffuse[3];
		float specular[3];
		ambient[0] = 0.0f; ambient[1] = 0.5f; ambient[2] = 0.6f; diffuse[0] = 0.5f; diffuse[1] = 0.2f; diffuse[2] = 0.6f; specular[0] = 0.8f; specular[1] = 0.9f; specular[2] = 0.6f;		
		entidades[2]->setMaterial(ambient, diffuse, specular);

		ambient[0] = 0.3f; ambient[1] = 0.4f; ambient[2] = 0.4f; diffuse[0] = 0.9f; diffuse[1] = 0.8f; diffuse[2] = 0.4f; specular[0] = 0.6f; specular[1] = 0.2f; specular[2] = 0.9f;
		entidades[4]->setMaterial(ambient, diffuse, specular);

		ambient[0] = 0.9f; ambient[1] = 0.3f; ambient[2] = 0.5f; diffuse[0] = 0.3f; diffuse[1] = 0.6f; diffuse[2] = 0.9f; specular[0] = 0.2f; specular[1] = 0.3f; specular[2] = 0.4f;
		entidades[5]->setMaterial(ambient, diffuse, specular);
		entidades[5]->setActiveReflect(1.0f);

		ambient[0] = 0.5f; ambient[1] = 0.2f; ambient[2] = 0.5f; diffuse[0] = 0.9f; diffuse[1] = 0.5f; diffuse[2] = 0.6f; specular[0] = 0.2f; specular[1] = 0.3f; specular[2] = 0.6f;
		entidades[6]->setMaterial(ambient, diffuse, specular);

		entidades[7]->transformation->setScaleMatrix(15.7f);
		entidades[7]->transformation->setTraslationMatrix(-3.3f, 2.1f, 11.4f);
		entidades[7]->transformation->setRotationMatrix(0.0f, -0.5f, 0.50f, 0.53f);
	}


	void update_Interface() {
		if (selected) {

			entidades[entitieSelected]->transformation->setScaleMatrix(Scale);
			entidades[entitieSelected]->transformation->setRotationMatrix(glm::toMat4(glm::normalize(glm::quat(Rotation[3], Rotation[0], Rotation[1], Rotation[2]))));
			entidades[entitieSelected]->transformation->setTraslationMatrix(TranslationSpeedX, TranslationSpeedY, TranslationSpeedZ);
			entidades[entitieSelected]->setParameters(matShininess, roughness, fresnel, albedo);

			float difuso=0, especular=0;

			if (difuse == LAMBERT) 	difuso = 0;			
			else difuso = 1;
			if (specular == COOK) especular = 0;
			else if (specular == PHONG) especular = 1;
			else especular = 2;

			for (int i = 0; i <entidades.size(); i++)
			{
				entidades[i]->setSettings(especular,difuso,texture_active);
			}
			
			entidades[entitieSelected]->setMaterial(matAmbientReflectances, matDiffuseReflectances, matSpecularReflectances);
			entidades[entitieSelected]->setRatioRefract(refract);

			//LUZ		
			entidades[0]->transformation->setTraslationMatrix(TranslationSpeedX_Light, TranslationSpeedY_Light, TranslationSpeedZ_Light);
			entidades[0]->setMaterial(lightAmbientIntensity, lightDiffuseIntensity, lightSpecularIntensity);
			entidades[0]->material->setDir(glm::vec3(g_LightDirection[0], g_LightDirection[1], g_LightDirection[2]));			
		}	
	}

	void display_low(Camera* Aux, mat4 mProjMatrix2) {
		Light*light = entidades[0]->material;
		glm::vec4 CentroLuz = entidades[0]->getCenter_2();
		if (luces_on) {

			ID->at(0) = glGetUniformLocation(shader_id, "u_lightAmbientIntensitys");
			glUniform3f(ID->at(0), light->getAmbient().r, light->getAmbient().g, light->getAmbient().b);

			ID->at(1) = glGetUniformLocation(shader_id, "u_lightDiffuseIntensitys");
			glUniform3f(ID->at(1), light->getDiffuse().r, light->getDiffuse().g, light->getDiffuse().b);

			ID->at(2) = glGetUniformLocation(shader_id, "u_lightSpecularIntensitys");
			glUniform3f(ID->at(2), light->getSpecular().r, light->getSpecular().g, light->getSpecular().b);
		}
		else {
			ID->at(0) = glGetUniformLocation(shader_id, "u_lightAmbientIntensitys");
			glUniform3f(ID->at(0), 0.0f, 0.0f, 0.0f);

			ID->at(1) = glGetUniformLocation(shader_id, "u_lightDiffuseIntensitys");
			glUniform3f(ID->at(1), 0.0f, 0.0f, 0.0f);

			ID->at(2) = glGetUniformLocation(shader_id, "u_lightSpecularIntensitys");
			glUniform3f(ID->at(2), 0.0f, 0.0f, 0.0f);
		}

		ID->at(3) = glGetUniformLocation(shader_id, "u_lightPosition");
		glUniform3f(ID->at(3), CentroLuz.x, CentroLuz.y, CentroLuz.z);

		ID->at(4) = glGetUniformLocation(shader_id, "eye_position");
		glUniform3f(ID->at(4), Aux->getPosition().x, Aux->getPosition().y, Aux->getPosition().z);

		ID->at(5) = glGetUniformLocation(shader_id, "u_viewMat");
		glUniformMatrix4fv(ID->at(5), 1, GL_FALSE, &(Aux->getMatrix())[0][0]);

		//mProjMatrix = glm::perspective(fov, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 100.0f);

		ID->at(6) = glGetUniformLocation(shader_id, "u_projMat");
		glUniformMatrix4fv(ID->at(6), 1, GL_FALSE, &(mProjMatrix2)[0][0]);

		ID->at(7) = glGetUniformLocation(shader_id, "u_normalMat");
		glUniformMatrix4fv(ID->at(7), 1, GL_FALSE, &(glm::inverse(glm::transpose(Aux->getMatrix())))[0][0]);

		ID->at(8) = glGetUniformLocation(shader_id, "u_lightDir");
		glUniform3f(ID->at(8), light->getDir().x, light->getDir().y, light->getDir().z);

		ID->at(9) = glGetUniformLocation(shader_id, "u_atConstant");
		glUniform1f(ID->at(9), atConstant);

		ID->at(10) = glGetUniformLocation(shader_id, "u_atLineal");
		glUniform1f(ID->at(10), atLineal);

		ID->at(11) = glGetUniformLocation(shader_id, "u_atCuadratic");
		glUniform1f(ID->at(11), atCuadratic);

		ID->at(12) = glGetUniformLocation(shader_id, "u_exponent");
		glUniform1f(ID->at(12), exponent);

		ID->at(13) = glGetUniformLocation(shader_id, "u_spotcos");
		glUniform1f(ID->at(13), spotcos);

		ID->at(14) = glGetUniformLocation(shader_id, "u_type_light");
		glUniform3f(ID->at(14), Ldirect, Lpoint, Lspot);
		/*Render all the entities*/

		/*Active CUBE_MAP SAMPLER FOR ALL OF ENTTITIES */
		for (int i = 0; i < (int)entidades.size(); i++) {			
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox->getTextureID());
			glUniform1i(glGetUniformLocation(shader_id, "cubeMap"), 1);	

			entidades[i]->render(shader_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}

		//FOR SKYBOX
	/*	Skybox->glslProgramSkybox->enable();

		ID->at(15) = glGetUniformLocation(Skybox->glslProgramSkybox->getProgramID(), "u_viewMat");
		glUniformMatrix4fv(ID->at(15), 1, GL_FALSE, &(Aux->getMatrix())[0][0]);

		ID->at(16) = glGetUniformLocation(Skybox->glslProgramSkybox->getProgramID(), "u_projMat");
		glUniformMatrix4fv(ID->at(16), 1, GL_FALSE, &(mProjMatrix2)[0][0]);

		Skybox->renderSkybox();

		Skybox->glslProgramSkybox->disable();*/

	}

	void display() {		
		glEnable(GL_DEPTH_TEST);
		animaciones();
		update_Interface();

		Light*light = entidades[0]->material;
		glm::vec4 CentroLuz = entidades[0]->getCenter_2();
		if (luces_on) {

			ID->at(0) = glGetUniformLocation(shader_id, "u_lightAmbientIntensitys");
			glUniform3f(ID->at(0), light->getAmbient().r, light->getAmbient().g, light->getAmbient().b);

			ID->at(1) = glGetUniformLocation(shader_id, "u_lightDiffuseIntensitys");
			glUniform3f(ID->at(1), light->getDiffuse().r, light->getDiffuse().g, light->getDiffuse().b);

			ID->at(2) = glGetUniformLocation(shader_id, "u_lightSpecularIntensitys");
			glUniform3f(ID->at(2), light->getSpecular().r, light->getSpecular().g, light->getSpecular().b);
		}
		else {
			ID->at(0) = glGetUniformLocation(shader_id, "u_lightAmbientIntensitys");
			glUniform3f(ID->at(0), 0.0f, 0.0f, 0.0f);

			ID->at(1) = glGetUniformLocation(shader_id, "u_lightDiffuseIntensitys");
			glUniform3f(ID->at(1),0.0f, 0.0f, 0.0f);

			ID->at(2) = glGetUniformLocation(shader_id, "u_lightSpecularIntensitys");
			glUniform3f(ID->at(2), 0.0f, 0.0f, 0.0f);
		}

		ID->at(3) = glGetUniformLocation(shader_id, "u_lightPosition");
		glUniform3f(ID->at(3), CentroLuz.x, CentroLuz.y, CentroLuz.z);

		ID->at(4) = glGetUniformLocation(shader_id, "eye_position");
		glUniform3f(ID->at(4), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

		ID->at(5) = glGetUniformLocation(shader_id, "u_viewMat");
		glUniformMatrix4fv(ID->at(5), 1, GL_FALSE, &(camera->getMatrix())[0][0]);

		//mProjMatrix = glm::perspective(fov, (GLfloat)gWidth / (GLfloat)gHeight, 0.1f, 100.0f);

		ID->at(6) = glGetUniformLocation(shader_id, "u_projMat");
		glUniformMatrix4fv(ID->at(6), 1, GL_FALSE, &(mProjMatrix)[0][0]);

		ID->at(7) = glGetUniformLocation(shader_id, "u_normalMat");
		glUniformMatrix4fv(ID->at(7), 1, GL_FALSE, &(glm::inverse(glm::transpose(camera->getMatrix())))[0][0]);

		ID->at(8) = glGetUniformLocation(shader_id, "u_lightDir");
		glUniform3f(ID->at(8), light->getDir().x, light->getDir().y, light->getDir().z);

		ID->at(9) = glGetUniformLocation(shader_id, "u_atConstant");
		glUniform1f(ID->at(9), atConstant);

		ID->at(10) = glGetUniformLocation(shader_id, "u_atLineal");
		glUniform1f(ID->at(10), atLineal);

		ID->at(11) = glGetUniformLocation(shader_id, "u_atCuadratic");
		glUniform1f(ID->at(11), atCuadratic);

		ID->at(12) = glGetUniformLocation(shader_id, "u_exponent");
		glUniform1f(ID->at(12), exponent);

		ID->at(13) = glGetUniformLocation(shader_id, "u_spotcos");
		glUniform1f(ID->at(13), spotcos);

		ID->at(14) = glGetUniformLocation(shader_id, "u_type_light");
		glUniform3f(ID->at(14), Ldirect, Lpoint, Lspot);

		ID->at(18) = glGetUniformLocation(shader_id, "u_depth_matrix");
		glUniformMatrix4fv(ID->at(18), 1, GL_FALSE, &(depthBiasMVP)[0][0]);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture_depth);
		glUniform1i(glGetUniformLocation(shader_id, "shadowMap"), 3);

		/*Render all the entities*/		

		/*Active CUBE_MAP SAMPLER FOR ALL OF ENTTITIES */

		

		for (int i = 0; i < (int)entidades.size(); i++) {
			if (entidades[i]->getActiveRefract() == 1.0f || entidades[i]->getActiveReflect() == 1.0f) {
				GLuint id_text = entidades[i]->getTextureCube();
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, id_text);
				glUniform1i(glGetUniformLocation(shader_id, "cubeMap"), 1);
			}
			else {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_CUBE_MAP, Skybox->getTextureID());
				glUniform1i(glGetUniformLocation(shader_id, "cubeMap"), 1);
			}
			
			entidades[i]->render(shader_id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);	
		}

		//FOR SKYBOX
		//Skybox->setTextureID(RenderDepthTexture(CentroLuz));
		Skybox->glslProgramSkybox->enable();
	
		
		ID->at(15) = glGetUniformLocation(Skybox->glslProgramSkybox->getProgramID(), "u_viewMat");
		glUniformMatrix4fv(ID->at(15), 1, GL_FALSE, &(camera->getMatrix())[0][0]);

		ID->at(16) = glGetUniformLocation(Skybox->glslProgramSkybox->getProgramID(), "u_projMat");
		glUniformMatrix4fv(ID->at(16), 1, GL_FALSE, &(mProjMatrix)[0][0]);

		Skybox->renderSkybox();

		Skybox->glslProgramSkybox->disable();
		
	}

	/*void ConfigureShaderAndMatrices() {
		GLfloat near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;
	}*/

	void paintPlane(GLuint Texture) {
		texture_program->enable();

		mat4 model =  glm::scale(vec3(0.08, 0.08, 0.08)) * glm::translate(vec3(7.4, -4.4, 1.0)) * mat4(1.0f);
		mat4 view = lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		mat4 projection = glm::perspective(45.0f, (float)gWidth / (float)gHeight, 0.1f, 1000.0f);

		ID->at(0) = glGetUniformLocation(texture_program->getProgramID(), "view");
		glUniformMatrix4fv(ID->at(0), 1, GL_FALSE, &(view)[0][0]);

		ID->at(1) = glGetUniformLocation(texture_program->getProgramID(), "projection");
		glUniformMatrix4fv(ID->at(1), 1, GL_FALSE, &(projection)[0][0]);

		ID->at(2) = glGetUniformLocation(texture_program->getProgramID(), "model");
		glUniformMatrix4fv(ID->at(2), 1, GL_FALSE, &(model)[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(glGetUniformLocation(texture_program->getProgramID(), "texture1"), 0);

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		texture_program->disable();
	
	}


	int infinity() {
		double time = 0.0;
		double aux;
		int contador = 0;
		iniciarPosiciones();
		while (!glfwWindowShouldClose(gWindow)) {
			aux = glfwGetTime();
			contador++;

			glClearColor(0.2f, 0.4f, 0.6f, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glCullFace(GL_FRONT);
			glslProgramShadow->enable();
	     	shader_id = glslProgramShadow->getProgramID();
			RenderDepthTexture();
			glslProgramShadow->disable();
			glCullFace(GL_BACK);
			glClearColor(0.2f, 0.4f, 0.6f, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			paintPlane(texture_depth);

			if (interpolacion == 0.0f) {
				glslProgramTexture->enable();
				shader_id = glslProgramTexture->getProgramID();
			}
			else {
				glslProgramTexture_vertex->enable();
				shader_id = glslProgramTexture_vertex->getProgramID();
			}

			//entidades[2]->getTexture()->setTextureID(texture_);
			
		    for (int i = 0; i < entidades.size(); i++)
			{
				if (entidades[i]->getActiveRefract() == 1.0f || entidades[i]->getActiveReflect() == 1.0f)
					entidades[i]->setTextureCube(RenderCubeDynamic(entidades[i]->getCenter_2()));
					//entidades[i]->setTextureCube(texture_);
			}

		/*	glslProgramShadow->enable();
			shader_id = glslProgramShadow->getProgramID();
			// 1. first render to depth map
			ID->at(0) = glGetUniformLocation(shader_id, "lightSpaceMatrix");
			glUniformMatrix4fv(ID->at(0), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

			glViewport(0, 0, frameBuffer->getImageWidth(), frameBuffer->getImageHeight());
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getImageFrameBuffer());
			glClear(GL_DEPTH_BUFFER_BIT);
			ConfigureShaderAndMatrices();
			display();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glslProgramShadow->disable();
			//PRUEBA
			glslProgramTexture->enable();
			// 2. then render scene as normal with shadow mapping (using depth map)
			glViewport(0, 0, gWidth, gWidth);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ConfigureShaderAndMatrices();
			glBindTexture(GL_TEXTURE_2D, frameBuffer->getImageDepthTexture());*/
			//display();

			display();
			TwDraw();
			//glslProgramTexture->disable();			
			glfwSwapBuffers(gWindow);
			glfwPollEvents();
			time = (glfwGetTime() - aux) + time;	
			if (time >= 1.0) {
				time = 0.0;
				//cout << contador << endl;
				contador = 0;
			}
		}
		return EXIT_SUCCESS;
	}

	void Init_Models() {
		//Crash
		/*Model* crash = new Model("../models/players/Crash/crash.jpg");
		crash->Leer("../models/players/Crash/crash.obj");*/

		//Soccer-Ball
		/*Model *crash3 = new Model("../models/wall/wall.jpg");		
		crash3->Leer("../models/wall/wall.obj");*/
		//Soccer-Ball
		Model *crash = new Model("../models/linterna/linterna.png", "null");
		crash->Leer("../models/linterna/linterna.obj");		
		//crash->transformation->addScaleMatrix(1.0f);

		/*Model *crash2 = new Model("../models/shield/shield.jpg");
		crash2->Leer("../models/shield/shield.obj");*/

		

		Model *crash2 = new Model("../models/drag/drag.jpg", "../models/drag/drag_n.jpg");
		crash2->Leer("../models/drag/drag.obj");

		Model *crash3 = new Model("../models/tile/tile.jpg", "null");
		crash3->Leer("../models/tile/tile.obj");

		Model *crash4 = new Model("../models/futbol/soccer_ball.jpg", "null");
		crash4->Leer("../models/futbol/soccer_ball.obj");

		Model *crash5 = new Model("../models/car/car.png", "null");
		crash5->Leer("../models/car/car.obj");

		Model *crash6 = new Model("../models/elemental/elemental.png", "null");
		crash6->Leer("../models/elemental/elemental.obj");

		
	    Model *crash7 = new Model("../models/sword/sword.jpg", "null");
		crash7->Leer("../models/sword/sword.obj");

		Model *crash8 = new Model("../models/arma/arma.jpg", "../models/arma/arma_n.jpg");
		crash8->Leer("../models/arma/arma.obj");

		/*Model *crash5 = new Model("../models/tile/tile.jpg");
		crash5->Leer("../models/tile/tile.obj");*/

		

		entidades.push_back(crash);	
		entidades.push_back(crash2);
		entidades.push_back(crash3); 
		entidades.push_back(crash4);
		entidades.push_back(crash5);
		entidades.push_back(crash6);
		entidades.push_back(crash7);
		entidades.push_back(crash8);

		/*Model *crash6 = new Model("../models/tile/tile.jpg");
		crash3->Leer("../models/tile/tile.obj");
		Model *crash7 = new Model("../models/tile/tile.jpg");
		crash3->Leer("../models/tile/tile.obj");

		entidades.push_back(crash2);
		entidades.push_back(crash6);
		entidades.push_back(crash7);*/



	}

	int main() {
		luces_on = true;
		Init_Models(); // Leo los modelos y los agrego al vector entidades
		Init();        // Inicializo todos los parámetros y las entidades
		InitAnt();
		//glfwSwapInterval(0);
		infinity();    // Bucle infinito
		return 0;
	}