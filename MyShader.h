#pragma once



GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

class MyShader {
	public:
		MyShader(const char* vertexShaderPath, const char* fragmentShaderPath, const char* nameOfShader);
		GLuint programID;
		GLuint MatrixID;
		GLuint ViewMatrixID;
		GLuint ModelMatrixID;
		const char* shaderName;
		GLuint GetUniformLocation(const char* variableName);

};