#pragma once
#include <string>
#include "GLHelper.h"

class Texture {
public:
	static GLuint createTexture(const std::string& path);
	static GLuint createWhiteTexture();
	static void deleteTexture(const GLuint id);

	static void Bind(const GLuint id, unsigned int slot = 0);
	static void Unbind(const GLuint id);

};