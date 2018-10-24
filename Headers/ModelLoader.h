#pragma once
#ifndef _VERTICIESSTRUCTURES_H_INCLUDED_
#define _VERTICIESSTRUCTURES_H_INCLUDED_

#include <soil.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <assimp\Importer.hpp>
#include "CameraControls.h"

using std::vector;
using std::string;

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};
struct Texture {
	unsigned int id;
	string type;
	string path;
};
class Mesh {
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO;

	/*  Functions  */
	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	
	// render the mesh
	void Draw(GLuint programID);

private:
	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh();
	
};

class Model
{
	
public:
	Model(string const &path, bool gamma, GLuint programID);
	/*  Model Data */
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	glm::vec3 position;
	string directory;
	bool gammaCorrection;
	string name;
	TxtDebugFile* PmodelDebugger;
	/*  Functions   */
	void Translate(float x, float y, float z);
	void Rotate(glm::vec3 axis, float angle);
	// draws the model, and thus all its meshes
	void Draw();

private:
	GLuint shaderProgramID;
	/*  Functions   */
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path);
	

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene);

	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};
#endif