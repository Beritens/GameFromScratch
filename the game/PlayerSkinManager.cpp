#include "PlayerSkinManager.h"

void over(unsigned char* a, unsigned char* b) {
	// Assuming RGBA format where each channel is a byte (8 bits)
	const int channelCount = 4;  // RGBA

	// Extract RGBA components from each pixel
	unsigned char aR = a[0];
	unsigned char aG = a[1];
	unsigned char aB = a[2];
	unsigned char aA = a[3];

	unsigned char bR = b[0];
	unsigned char bG = b[1];
	unsigned char bB = b[2];
	unsigned char bA = b[3];

	// Calculate blended components using alpha blending formula
	float alphaA = static_cast<float>(aA) / 255.0f;
	float alphaB = static_cast<float>(bA) / 255.0f;
	float blendedAlpha = alphaA * (1.0f - alphaB) + alphaB ;

	unsigned char resultR = static_cast<unsigned char>(((alphaB * bR + alphaB * (1.0f - alphaB) * aR) / blendedAlpha));
	unsigned char resultG = static_cast<unsigned char>((alphaB * bG + alphaB * (1.0f - alphaB) * aG) / blendedAlpha);
	unsigned char resultB = static_cast<unsigned char>((alphaB * bB + alphaB * (1.0f - alphaB) * aB) / blendedAlpha);
	unsigned char resultA = static_cast<unsigned char>(blendedAlpha * 255.0f);

	// Update pixel a with the blended values
	a[0] = resultR;
	a[1] = resultG;
	a[2] = resultB;
	a[3] = resultA;
}

uint32_t s_id = 0;
//probably be better to load those in the beginning and store the buffers. but I guess that would mean all textures have to be in the memory all the time. not quite sure what is better
std::string skinPaths[5] = { "Assets/character-guy-hair.png" , "Assets/character-hoodie.png" , "Assets/character-jeans.png" ,"Assets/character-hair.png" , "Assets/character-dress.png" };

void PlayerSkinManager::Init(uint32_t id)
{
	s_id = id;
}

void PlayerSkinManager::changePlayerSkin(ItemHolder ih)
{
	stbi_set_flip_vertically_on_load(0);
	//4 channels rgba
	int width, height, bpp;
	unsigned char* localBuffer = stbi_load("Assets/character-sprite.png", &width, &height, &bpp, 4);
	for (int i = 0; i < 5; i++) {
		if (!ih.items[i]) {
			continue;
		}
		unsigned char* localBufferOverlay = stbi_load(skinPaths[i].c_str(), &width, &height, &bpp, 4);
		for (int j = 0; j < width * height; j++) {
			if (localBufferOverlay[j * 4 + 3] != '\0') {
				over(localBuffer + j * 4, localBufferOverlay + j * 4);

			}
		}

	}
	
	GLCall(glBindTexture(GL_TEXTURE_2D, s_id));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	printf("HEIGHT                           %d\n", s_id);
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	if (localBuffer) {
		//for now, might keep it around later (for example to combine sprite with item sprite)
		stbi_image_free(localBuffer);
	}
}