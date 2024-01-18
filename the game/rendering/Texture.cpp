#include "Texture.h"
#include "stb_image/stb_image.h"
#include "GLHelper.h"

GLuint Texture::createTexture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(0);
	//4 channels rgba
	int width, height, bpp;
	GLuint id;
	unsigned char*  localBuffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	printf("%d renderer id", id);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	printf("HEIGHT                           %d\n", height);
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,localBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	if (localBuffer) {
		//for now, might keep it around later (for example to combine sprite with item sprite)
		stbi_image_free(localBuffer);
	}
	return id;
}
GLuint Texture::createWhiteTexture()
{
	stbi_set_flip_vertically_on_load(0);
	//4 channels rgba
	GLuint id;
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	printf("%d renderer id", id);

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	uint32_t color = 0xffffffff;
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	return id;
}

void Texture::deleteTexture(GLuint id)
{
	GLCall(glDeleteTextures(1, &id));
}

void Texture::Bind(const GLuint id, unsigned int slot)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot))
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
}

void Texture::Unbind(const GLuint id)
{
}
