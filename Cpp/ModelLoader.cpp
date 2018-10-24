
#include "stdafx.h"
#include "ModelLoader.h"

using namespace std;
bool debugerInit = false;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

void Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO); // returns 'n' vertex array object names in 'arrays'.
	glGenBuffers(1, &VBO); // returns 'n' buffer object names in 'buffers'.
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO); // binds the vertex array object with name 'array'. Any previous vertex array object binding is broken.
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // binds a buffer object to the specified buffer binding point. Calling glBindBuffer with target set to one of the accepted symbolic constants and buffer set to the name of a buffer object binds that buffer object name to the target.
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); // Create a new data store for a buffer object. In case of glBufferData, the buffer object currently bound to target is used.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// glEnableVertexAttribArray(index); enable the generic vertex attribute array specified by index.
	// vertex Positions
	glEnableVertexAttribArray(0); 
	//glVertexAttribPointer(GLuint index,GLint size,GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer); Specify the location and data format of the array of generic vertex attributes at index 'index' to use when rendering. size specifies the number of components per attribute and must be 1, 2, 3, 4, or GL_BGRA. type specifies the data type of each component, and stride specifies the byte stride from one attribute to the next.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); 
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

// render the mesh
void Mesh::Draw(GLuint programID)
{
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream
		// now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(programID, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	
	// draw mesh
	glBindVertexArray(VAO);
	
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}






/*  Functions   */
// constructor, expects a filepath to a 3D model.
Model::Model(string const &path, bool gamma = false, GLuint programID = 0)
{
	gammaCorrection = gamma;
	shaderProgramID = programID;
	// set the name of the model as name of the OBJ file
	name = path.substr(0, path.find_last_of('/'));
	string const debugTxtPath = "MyDebuggers/MyDebugLog.txt";
	TxtDebugFile txtDebugFile(debugTxtPath, false);
	PmodelDebugger = &txtDebugFile;
	loadModel(path);
}

// draws the model, and thus all its meshes
void Model::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shaderProgramID);
	}
}

/*  Functions   */
// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(string const &path)
{
	std::cout << "Loading model " << name <<'\n';
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));
	
	PmodelDebugger->WriteOnTxt("Nubmer of nodes: " + scene->mRootNode->mNumChildren);
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		std::string s = { "Current node: " };
		PmodelDebugger->WriteOnTxt(s + node->mName.C_Str());
		s.~basic_string();
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

	

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	PmodelDebugger->WriteOnTxt("Mesh vertices of model: " + name + " = " + to_string(mesh->mNumVertices));
	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
			
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		if (mesh->mTangents != NULL) {
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}
		// bitangent
		if (mesh->mBitangents != NULL) {
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		vertices.push_back(vertex);
	}
	PmodelDebugger->WriteOnTxt("Vertices of model: " + name + " loaded. Total vertices: " +  to_string(vertices.size()));
	unsigned int totalNormals = 0;
	for (unsigned int i = 0; i != vertices.size(); i++) {
		if (vertices[i].Normal != glm::vec3(0,0,0)) {
			totalNormals++;
		}	
	}
	PmodelDebugger->WriteOnTxt("Normals of model: " + name + " loaded. Total normals: " + to_string(totalNormals));
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	PmodelDebugger->WriteOnTxt("Faces of model: " + name + " loaded. Total faces: " + to_string(mesh->mNumFaces));
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	PmodelDebugger->WriteOnTxt("Number of diffuse textures: " + to_string(diffuseMaps.size()));
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	PmodelDebugger->WriteOnTxt("Number of specular textures: " + to_string(diffuseMaps.size()));
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	PmodelDebugger->WriteOnTxt("Number of normal textures: " + to_string(diffuseMaps.size()));
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	PmodelDebugger->WriteOnTxt("Number of heighmap textures: " + to_string(diffuseMaps.size()));
	std::string s = "Mesh ";
	PmodelDebugger->WriteOnTxt(s + mesh->mName.C_Str());
	s.~basic_string();
	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				PmodelDebugger->WriteOnTxt("Texture loaded: " + textures_loaded[j].path);
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	PmodelDebugger->WriteOnTxt("Material textures: " + to_string(textures.size()));
	return textures;
}





unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		SOIL_free_image_data(data);
	}

	return textureID;
}
void Model::Translate(float x, float y, float z) {
	position += glm::vec3(x, y, z);
	for (unsigned int currentMesh = 0; currentMesh < meshes.size(); currentMesh++) {
		for (unsigned int currentVertex = 0; currentVertex < meshes[currentMesh].vertices.size(); currentVertex++) {
			meshes[currentMesh].vertices[currentVertex].Position = meshes[currentMesh].vertices[currentVertex].Position + position;
		}
		meshes[currentMesh] = Mesh(meshes[currentMesh].vertices, meshes[currentMesh].indices, meshes[currentMesh].textures);
	}
}
void Model::Rotate(glm::vec3 axis, float angle) {
	for (unsigned int currentMesh = 0; currentMesh < meshes.size(); currentMesh++) {
		for (unsigned int currentVertex = 0; currentVertex < meshes[currentMesh].vertices.size(); currentVertex++) {
			meshes[currentMesh].vertices[currentVertex].Position = meshes[currentMesh].vertices[currentVertex].Position - position;
			const glm::mat4 rot = rotate(angle, axis);
			glm::mat4 trans = glm::translate(glm::mat4(1.f), meshes[currentMesh].vertices[currentVertex].Position);
			glm::mat4 finalMatrix = rot * trans;
			meshes[currentMesh].vertices[currentVertex].Position = glm::vec3(finalMatrix[3]);
			meshes[currentMesh].vertices[currentVertex].Position = meshes[currentMesh].vertices[currentVertex].Position + position;
		}
		meshes[currentMesh] = Mesh(meshes[currentMesh].vertices, meshes[currentMesh].indices, meshes[currentMesh].textures);
	}
}
