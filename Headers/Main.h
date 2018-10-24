
#ifndef __MyTutLearnOpenGL_H_INCLUDED__
#define __MyTutLearnOpenGL_H_INCLUDED__
#include "ModelLoader.h"
#include "MyShader.h"
#include "CameraControls.h"
#include <glm\gtc\type_ptr.hpp>
//#include "MyOBJLoader.h";

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

bool MyOGLInitialize();

#endif

