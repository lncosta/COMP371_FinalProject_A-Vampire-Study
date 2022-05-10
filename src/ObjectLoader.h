#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>  
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>  

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>




#include "Shader.h"
using namespace std;
#define MAX_BONE_INFLUENCE 4

class Material;
class Vertex {
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
    glm::vec4 color; //In RGBA
    float useTexture; 

    //Additional parameters for debugging, as shown in LearnOpenGL.
    glm::vec3 Tangent;
   
    glm::vec3 Bitangent;
   
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

};

class Texture {
public:
    unsigned int id;
    string path; 
    string type;

    unsigned int loadFromFile(const char* path, string directory);

};


class Mesh { //Based off the Mesh class definition presented in the LearnOpenGL textbook - "https://learnopengl.com/Model-Loading/Mesh "
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO, VBO, EBO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void draw(Shader& shader);
    void setupMesh();
};

class Model {
public:
    vector<Mesh> meshes;
    vector<Texture> loaded_textures;
    string directory; 


    Model(string path);
    void draw(Shader& shader);
    void load(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        string typeName);

    Material loadMaterial(aiMaterial* mat); //For obj files without textures.

};
class Object {
public:
    vector< glm::vec3 > vertices;
    vector< glm::vec2 > uvs;
    vector< glm::vec3 > normals;

    Object(vector <glm::vec3>& out_vertices,
        vector <glm::vec2>& out_uvs,
        vector <glm::vec3>& out_normals);
};
class ObjectLoader {
public:
    vector<Object*> loaded; 
    bool loadObj(string path,
        vector <glm::vec3>& out_vertices,
        vector <glm::vec2>& out_uvs,
        vector <glm::vec3>& out_normals);


};

using namespace glm;


class Material {
public:
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float pc;
};
