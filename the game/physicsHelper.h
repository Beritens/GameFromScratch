#pragma once
#include "entity.h"
#include "utility.h"

struct Collision {
    two::Entity a;
    two::Entity b;
    float2 pointA;
    float2 pointB;
    float2 velA;
    float2 velB;
    float2 normal;
};