#include "Model.h"


Model::Model(string rutaTextura, string rutaTextura_normal) {
	//this->glVBO = (GLfloat*)malloc(sizeof(GLfloat));

	/*Init Uniforms ID*/
	this->ID = new vector<GLint>(60, 0);

	/*Init Tranformation*/
	this->transformation = new Transformation();
	this->size_tangents = 0;
	/*Init Material Colors*/
	this->material = new Light();
	this->shininess = 4.0f;
	this->roughness = 0.1f;
	this->ruta_textura = rutaTextura;
	this->ruta_textura_normal = rutaTextura_normal;
	this->fresnel = 0.5f;
	this->albedo = 0.5f;
	this->type_interpol = 0.0f;
	this->one_nayar = 1.0f;
	this->cook_torrance = 0.0f;
	this->texture_active = 1.0f;
	this->phong_active = 1.0f;
	this->ratioRefract = 3.0f;
	this->activeRefract = 0.0f;
	this->activeReflect = 0.0f;
	/*this->material->setAmbient(vec3(0.0f, 1.0f, 1.0f));
	this->material->setDiffuse(vec3(1.0f, 0.0f, 1.0f));
	this->material->setSpecular(vec3(0.0f, 1.0f, 1.0f));
	this->transformation->addScaleMatrix(4.0f);*/
	this->transformation->setScaleMatrix(0.09f);
	//min = this->transformation->getTransformMatrix() * min;
	//max = this->transformation->getTransformMatrix() * max;
	
	//this->transformation->addScaleMatrix(5.0f);
}

float Model::getActiveReflect()
{
	return this->activeReflect;
}

GLuint Model::getTextureCube()
{
	return this->textureCubeID;
}

glm::vec4 Model::getCenter() {
	return ( (this->min + this->max) / 2.0f);
}

glm::vec4 Model::getCenter_2()
{
	return ((this->min_aux + this->max_aux) / 2.0f);
}

void Model::render(GLuint shader_id) {
	/*Bind Uniforms*/
	//cout << min[0] << "," << min[1] << min[2] << endl;
	//this->transformation->addTraslationMatrix(4.0f, 5.0f, 6.0f);

	min_aux = glm::normalize(this->transformation->getTransformMatrix() * min);
	max_aux = glm::normalize(this->transformation->getTransformMatrix() * max);

	//cout << min_aux[0] << "," << min_aux[1] << ","<< min_aux[2] << endl;

	//system("pause");

	this->ID->at(0) = glGetUniformLocation(shader_id, "u_matAmbientReflectances");
	glUniform3f(this->ID->at(0), this->material->getAmbient().r, this->material->getAmbient().g, this->material->getAmbient().b);

	this->ID->at(1) = glGetUniformLocation(shader_id, "u_matDiffuseReflectances");
	glUniform3f(this->ID->at(1), this->material->getDiffuse().r, this->material->getDiffuse().g, this->material->getDiffuse().b);

	this->ID->at(2) = glGetUniformLocation(shader_id, "u_matSpecularReflectances");
	glUniform3f(this->ID->at(2), this->material->getSpecular().r, this->material->getSpecular().g, this->material->getSpecular().b);

	this->ID->at(3) = glGetUniformLocation(shader_id, "u_matShininess");
	glUniform1f(this->ID->at(3), this->shininess);

	//this->transformation->setScaleMatrix(10000.0f);

	this->ID->at(4) = glGetUniformLocation(shader_id, "u_modelMat");
	glUniformMatrix4fv(this->ID->at(4), 1, GL_FALSE, &(this->transformation->getTransformMatrix())[0][0]);

	/*glm::mat4 TInverse = glm::mat4(1.0f);
	TInverse[3][0] = -this->getCenter().x;
	TInverse[3][1] = -this->getCenter().y;
	TInverse[3][2] = -this->getCenter().z;

	glm::mat4x4 transformInverse = TInverse*this->transformation->getRotationMatrix()*this->transformation->getScaleMatrix();

	this->ID->at(4) = glGetUniformLocation(shader_id, "u_modelMat");
	glUniformMatrix4fv(this->ID->at(4), 1, GL_FALSE, &(transformInverse)[0][0]);*/

	this->ID->at(5) = glGetUniformLocation(shader_id, "u_roughness");
	glUniform1f(this->ID->at(5), this->roughness);

	this->ID->at(6) = glGetUniformLocation(shader_id, "u_fresnel");
	glUniform1f(this->ID->at(6), this->fresnel);

	this->ID->at(7) = glGetUniformLocation(shader_id, "u_albedo");
	glUniform1f(this->ID->at(7), this->albedo);

	// PRUEBA
	this->ID->at(8) = glGetUniformLocation(shader_id, "u_specular");
	glUniform1f(this->ID->at(8), this->specular);

	this->ID->at(9) = glGetUniformLocation(shader_id, "u_difuse");
	glUniform1f(this->ID->at(9), this->difuse);

	/*this->ID->at(8) = glGetUniformLocation(shader_id, "u_settings");
	glUniform4f(this->ID->at(8),this->one_nayar , this->cook_torrance , this->texture_active, this->phong_active);

	this->ID->at(9) = glGetUniformLocation(shader_id, "u_type_interpol");
	glUniform1f(this->ID->at(9), this->type_interpol);*/

	this->ID->at(10) = glGetUniformLocation(shader_id, "u_ratioRefract");
	glUniform1f(this->ID->at(10), this->ratioRefract);

	this->ID->at(11) = glGetUniformLocation(shader_id, "u_activeRefract");
	glUniform1f(this->ID->at(11), this->activeRefract);

	this->ID->at(12) = glGetUniformLocation(shader_id, "u_activeReflect");
	glUniform1f(this->ID->at(12), this->activeReflect);

	this->ID->at(13) = glGetUniformLocation(shader_id, "u_texture_active");
	glUniform1f(this->ID->at(13), this->texture_active);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture->getID());
	glUniform1i(glGetUniformLocation(shader_id, "texture1"), 0);	

	if (this->texture_normal->getRoute() != "null") {
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, this->texture_normal->getID());
		glUniform1i(glGetUniformLocation(shader_id, "texture_n"), 2);

		this->ID->at(14) = glGetUniformLocation(shader_id, "u_texture_active_normal");
		glUniform1f(this->ID->at(14), 1.0f);

	}
	else {
		this->ID->at(15) = glGetUniformLocation(shader_id, "u_texture_active_normal");
		glUniform1f(this->ID->at(15), 0.0f);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(glGetUniformLocation(shader_id, "texture_n"), 2);
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->glVBO_dir);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void*)0);
	/*Normals*/
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	/*Texture Coord*/
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	/*Tangent*/	
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void*)(9 * sizeof(GLfloat)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glVBO_indexes_dir);

	glDrawElements(GL_TRIANGLES, this->glVBO_indexes_size, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Model::render_shadow(GLuint shader_id)
{
	this->ID->at(0) = glGetUniformLocation(shader_id, "u_modelMat");
	glUniformMatrix4fv(this->ID->at(0), 1, GL_FALSE, &(this->transformation->getTransformMatrix())[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, this->glVBO_dir);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glVBO_indexes_dir);

	glDrawElements(GL_TRIANGLES, this->glVBO_indexes_size, GL_UNSIGNED_INT, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



vector<GLfloat> Model::getGLVBO() {
	return this->glVBO;
}

Transformation * Model::getTransformation() {
	return this->transformation;
}


Texture * Model::getTexture() {
	return this->texture;
}

GLuint Model::getGLVBO_dir() {
	return this->glVBO_dir;
}

GLuint Model::getGLVBO_indexes_dir() {
	return this->glVBO_indexes_dir;
}

vector<GLuint> Model::getGLVBO_indexes() {
	return this->glVBO_indexes;
}

GLuint Model::getGLVBO_indexes_size() {
	return this->glVBO_indexes_size;
}

float Model::getShininess() {
	return this->shininess;
}

void Model::setTransformation(Transformation * transformation) {
	this->transformation = transformation;
}

void Model::setTexture(Texture * texture) {
	this->texture = texture;
}

float Model::getActiveRefract()
{
	return this->activeRefract;
}

float Model::getRatioRefract()
{
	return this->ratioRefract;
}

float Model::getRoughness()
{
	return this->roughness;
}

float Model::getFresnel()
{
	return this->fresnel;
}

float Model::getAlbedo()
{
	return this->albedo;
}


void Model::setActiveReflect(float reflect)
{
	this->activeReflect = reflect;
}

void Model::setTextureCube(GLuint textureID)
{
	this->textureCubeID = textureID;
}

void Model::initGLDataBinding() {

	this->glVBO_indexes_size = this->glVBO_indexes.size();
	/*Rendering using VBO*/
	glGenBuffers(1, &(this->glVBO_dir));											// create a vbo
	glBindBuffer(GL_ARRAY_BUFFER, this->glVBO_dir);
	glBufferData(GL_ARRAY_BUFFER, (this->glVBO.size()) * sizeof(GLfloat), &(this->glVBO[0]), GL_STATIC_DRAW);	// upload data to video card
	//glBufferData(GL_ARRAY_BUFFER, (this->glVBO.size()) * sizeof(GLfloat), 0, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	//glEnableVertexAttribArray(4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &(this->glVBO_indexes_dir));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->glVBO_indexes_dir);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->glVBO_indexes.size() * sizeof(GLuint), &(this->glVBO_indexes[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->texture = new Texture(this->ruta_textura);
	if (this->ruta_textura_normal != "null") {
		this->texture_normal = new Texture(this->ruta_textura_normal);
	}
	else {
		this->texture_normal = new Texture();
	}
	/*Enable Backface Culling and Z Buffer*/
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


inline glm::vec3 aiToGlm(aiVector3D v)
{
	return glm::vec3(v.x, v.y, v.z);
}

void Model::processNode(aiNode * node, const aiScene * scene) {


	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		for (GLuint j = 0; j < mesh->mNumVertices; j++) {
			this->glVBO.push_back(mesh->mVertices[j].x);
			this->glVBO.push_back(mesh->mVertices[j].y);
			this->glVBO.push_back(mesh->mVertices[j].z);
			this->glVBO.push_back(mesh->mNormals[j].x);
			this->glVBO.push_back(mesh->mNormals[j].y);
			this->glVBO.push_back(mesh->mNormals[j].z);
			if (mesh->mTextureCoords[0]) {
				texturize = true;
				this->glVBO.push_back(mesh->mTextureCoords[0][j].x);
				this->glVBO.push_back(mesh->mTextureCoords[0][j].y);
				this->glVBO.push_back(mesh->mTextureCoords[0][j].z);
			}
		    this->glVBO.push_back(mesh->mTangents[j].x);
			this->glVBO.push_back(mesh->mTangents[j].y);
			this->glVBO.push_back(mesh->mTangents[j].z);
		}
		const aiFace& face = mesh->mFaces[i];
		for (GLuint k = 0; k < mesh->mNumFaces; k++) {
			aiFace face = mesh->mFaces[k];
			for (GLuint j = 0; j < face.mNumIndices; j++)
				this->glVBO_indexes.push_back(face.mIndices[j]);
		}
	}
	for (GLuint i = 0; i < node->mNumChildren; i++)
		this->processNode(node->mChildren[i], scene);

}

void Model::Normalize_vec() {
	float max = 0.0f;

	if (this->max.x >= this->max.y && this->max.x >= this->max.z) {
		max = this->max.x;
	}
	else if (this->max.y >= this->max.x && this->max.y >= this->max.z){
		max = this->max.y;	
	}
	else {
		max = this->max.z;
	}

	float centro_x = (this->max.x + this->min.x) / 2.0f;
	float centro_y = (this->max.y + this->min.y) / 2.0f;
	float centro_z = (this->max.z + this->min.z) / 2.0f;

	for (int i = 0; i < this->glVBO.size(); i++)
	{
		this->glVBO[i] = (this->glVBO[i] - centro_x) / max;
		this->glVBO[i+1] = (this->glVBO[i+1] - centro_y) / max;
		this->glVBO[i+2] = (this->glVBO[i+2] - centro_z) / max;

		if (this->texturize) {
			i = i + 11;
		}
		else {
			i = i + 8;
		}
	}
	this->max.x = (this->max.x - centro_x) / max;
	this->max.y = (this->max.y - centro_y) / max;
	this->max.z = (this->max.z - centro_z) / max;	
	this->min.x = (this->min.x - centro_x) / max;
	this->min.y = (this->min.y - centro_y) / max;
	this->min.z = (this->min.z - centro_z) / max;
}


void Model::setParameters(float matShininess, float roughness, float fresnel, float albedo)
{
	this->shininess = matShininess;
	this->albedo = albedo;
	this->roughness = roughness;
	this->fresnel = fresnel;
}

void Model::setSettings(float type_interpol, float one_nayar, float cook_torrance, float texture_active, float phong_active)
{
	this->type_interpol = type_interpol;
	this->one_nayar = one_nayar;
	this->cook_torrance = cook_torrance;
	this->texture_active = texture_active;
	this->phong_active = phong_active;
}

void Model::setSettings(float specular, float difuse, float texture_active)
{
	this->texture_active = texture_active;
	this->specular = specular;
	this->difuse = difuse;
}

void Model::setMaterial(float matAmbientReflectances[3], float matDiffuseReflectances[3], float matSpecularReflectances[3])
{
	this->material->setAmbient(glm::vec3(matAmbientReflectances[0], matAmbientReflectances[1], matAmbientReflectances[2]));
	this->material->setDiffuse(glm::vec3(matDiffuseReflectances[0], matDiffuseReflectances[1], matDiffuseReflectances[2]));
	this->material->setSpecular(glm::vec3(matSpecularReflectances[0], matSpecularReflectances[1], matSpecularReflectances[2]));
}

void Model::setActiveRefract(float active)
{
	this->activeRefract = active;
}

void Model::setRatioRefract(float refract)
{
	this->ratioRefract = refract;
}



void Model::Leer(string routes) {

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(routes.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |aiProcess_CalcTangentSpace );

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	/*Process ASSIMP's root node recursively*/
	this->processNode(scene->mRootNode, scene);
	Max_min();
	Normalize_vec();
	return;

}


void Model::Max_min() {
	for (int i = 0; i < this->glVBO.size(); i = i+9) {
		if (i == 0) {
			min.x = this->glVBO[i];
			max.x = this->glVBO[i];
			min.y = this->glVBO[i + 1];
			max.y = this->glVBO[i + 1];
			min.z = this->glVBO[i + 2];
			max.z = this->glVBO[i + 2];
			min[3] = 1.0f;
			max[3] = 1.0f;
		}
		else {
			if (this->glVBO[i] < min.x) min.x = this->glVBO[i];
			if (this->glVBO[i] > max.x) max.x = this->glVBO[i];
			if (this->glVBO[i + 1] < min.y) min.y = this->glVBO[i + 1];
			if (this->glVBO[i + 1] > max.y) max.y = this->glVBO[i + 1];
			if (this->glVBO[i + 2] < min.z) min.z = this->glVBO[i + 2];
			if (this->glVBO[i + 2] > max.z) max.z = this->glVBO[i + 2];
		}
	}
}


