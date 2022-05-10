#include "ObjectLoader.h"
#include <iostream>
#include <sstream>
#include <string>
#include <stb_image.h>

//Based off the LearnOpenGL Assimp tutorial. 


Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	setupMesh(); 
}

void Mesh::draw(Shader& shader)
{
	//Using standard naming convention for texture names.
	unsigned int diffuseNumber = 1;
	unsigned int specularNumber = 1;
	unsigned int normalNumber = 1;
	unsigned int heightNumber = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		string num;
		string name = textures[i].type;
		if (name == "texture_diffuse"){
			num = to_string(diffuseNumber++); 
		}
		else if (name == "texture_specular") {
			num = to_string(specularNumber++);
		}
		else if (name == "texture_normal") {
			num = to_string(normalNumber++);
		}

		else if (name == "texture_height") {
			num = to_string(heightNumber++); 
		}

		//shader.setFloat((name + num).c_str(), i);
		glUniform1i(glGetUniformLocation(shader.id, (name + num).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	/*for (unsigned int i = 0; i < vertices.size(); i++) {
		string color= "Color";
		shader.setVec4((color + std::to_string(i+1)).c_str(), vertices[i].color);
	}*/

	

	//Drawing mesh:
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	 
	
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//Positions:
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	//Normals:
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	
	//Texture Coordinates:
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates));

	//Colors
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	//Use Textures:
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, useTexture));
	
	//Additional paraters - DEBUG for missing meshes:
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// ids
	glEnableVertexAttribArray(7);
	glVertexAttribIPointer(7, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

	// weights
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

	//Binding:
	glBindVertexArray(0);
}

Model::Model(string path)
{
	cout << "Loading " << path << endl; 
	load(path); 
}

void Model::draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::load(string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "Error in loading Assimp:" << import.GetErrorString() << endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/')); 
	cout << "Directory: " << directory << endl;
	cout << "Root node: " << scene->mRootNode << endl;
	processNode(scene->mRootNode, scene);
	cout << "Model has been loaded." << endl;

}

void Model::processNode(aiNode* node, const aiScene* scene)
{ //Adds all meshes to vector as it reads through the nodes from parent to child. 


	//Process individual meshes: 
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		cout << "Processing mesh" << endl;
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	//Process node's children:

	for (unsigned int j = 0; j < node->mNumChildren; j++)
	{
		cout << "Processing child" << endl;
	
		processNode(node->mChildren[j], scene);
	}
	cout << "Nodes processed." << endl;

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vert;
	vector<unsigned int> indx;
	vector<Texture> text;
	unsigned int i, j;

	//Process vertices:
	for (i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex; 
		glm::vec3 vector; //Auxiliary holder for information

		//Vertices:
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;


		//Normals:
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		//Texture coordinates:

		if (mesh->mTextureCoords[0]) { //If there are texture coordinates
			glm::vec2 twoDvector;
			twoDvector.x = mesh->mTextureCoords[0][i].x;
			twoDvector.y = mesh->mTextureCoords[0][i].y;
			vertex.texture_coordinates = twoDvector;

			//DEBUG: missing meshes
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;

		}
		else { //Empty texture
			vertex.texture_coordinates = glm::vec2(0.0f, 0.0f); 
		}


		//Solution for loading object files straight from Blender as explained by Tomas Bacsi:
		if (mesh->mMaterialIndex < scene->mNumMaterials)
		{
			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
			aiColor4D diffuse(1.0f, 1.0f, 1.0f, 1.0f);
			float f[4]; 
			if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) {
				
				f[0] = diffuse.r;
				f[1] = diffuse.g;
				f[2] = diffuse.b;
				f[3] = diffuse.a;
				
				vertex.color = glm::vec4(f[0], f[1], f[2], 1);
			}
			else {
				cout << "Color loading error!" << endl; 
			}
				

			//cout << vertex.color.r << endl;
			
			//Set flag for whether textures should be used or not. 
			if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				vertex.useTexture = 1.f;
			}
			else
			{
				vertex.useTexture = 0.f;
			}
		}


		vert.push_back(vertex); 

	}

	//Process indexes of faces:
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		if (face.mNumIndices < 3) {
			continue;
		}
		assert(face.mNumIndices == 3);

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indx.push_back(face.mIndices[j]);
		}		
	}

	//Process materials:


	if (mesh->mMaterialIndex >= 0){ //If there are materials
	
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");	
		vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		
		text.insert(text.end(), diffuseMaps.begin(), diffuseMaps.end());
		text.insert(text.end(), specularMaps.begin(), specularMaps.end());
		text.insert(text.end(), normalMaps.begin(), normalMaps.end());
		text.insert(text.end(), heightMaps.begin(), heightMaps.end());
	}

	
	cout << "Meshes processed" << endl; 
	return Mesh(vert, indx, text); 
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{//Iterates through texture locations to retrieve the file information and generate the respective texture.

	vector<Texture> textures; 
	unsigned int i, j;

	for (i = 0; i < mat->GetTextureCount(type); i++){
		aiString line;
		mat->GetTexture(type, i, &line); 
		bool loaded = false; 

		for (j = 0; j < loaded_textures.size(); j++) { //Check if texture already has been loaded
			if (strcmp(loaded_textures[j].path.data(), line.C_Str()) == 0) { //Must use this comparison method because lhs must be variable
				loaded = true; 
				textures.push_back(loaded_textures[j]); 
				break; 
			}
		}
		if (!loaded) {
			Texture texture; 
			string path = string(line.C_Str());
			texture.loadFromFile(line.C_Str(), directory); 
			texture.type = typeName;
			texture.path = path;

			textures.push_back(texture);
			loaded_textures.push_back(texture); 
		}

	}
	cout << "Material textures loaded." << endl;
	return textures;
}

Material Model::loadMaterial(aiMaterial* mat)
{
	//For objects with material specifications rather than textures:

	Material material;
	aiColor3D color(0.f, 0.f, 0.f);

	float shine; 

	mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	material.ambient = glm::vec3(color.r, color.b, color.g);

	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	material.diffuse = glm::vec3(color.r, color.b, color.g);

	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	material.specular = glm::vec3(color.r, color.b, color.g);

	mat->Get(AI_MATKEY_SHININESS, shine);
	material.pc = shine;

	return material; 


}

unsigned int  Texture::loadFromFile(const char* path, string directory)
{
	unsigned int textID;
	int num, height, width; 
	string name = string(path); 

	cout << "Searching for texture in " << directory << "/" << name << endl;

	string folders = "../../../assets/project_room_4/"; 

	glGenTextures(1, &textID);
	unsigned char* result = stbi_load((folders + name).c_str(), &width, &height, &num, 0);


	if (result != NULL) {
		GLenum type;

		if (num == 1) {
			type = GL_RED;
		}
		else if (num == 3) {
			type = GL_RGB;
		}
		else if (num == 4) {
			type = GL_RGBA;
		}

		//Binding the texture:
		glBindTexture(GL_TEXTURE_2D, textID);
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, result);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(result); 
	}
	else {
		cout << "Error - Texture could not be loaded." << endl;
		stbi_image_free(result);
	}


	id = textID; 
	cout << "Texture loaded from file." << endl;

	return id; 
}
