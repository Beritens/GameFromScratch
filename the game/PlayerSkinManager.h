#pragma once
#include <string>
#include <stb_image/stb_image.h>
#include "rendering/GLHelper.h"
#include <cstdint>
#include "components.h"

class PlayerSkinManager{
public:
    PlayerSkinManager() = default;
    static void Init(uint32_t id);
    static void changePlayerSkin(ItemHolder ih);
};