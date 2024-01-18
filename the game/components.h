#pragma once
#include "utility.h"
#include "entity.h"

struct Transform {
    float2 position;
    float2 scale;
    float rotation;
};
struct Controller {
};

struct Sprite {
    int spriteId;
    float2 pos;
    float2 scale;
    float2 offset;
};
struct Animation {
    int animId;
    float time;
};

struct Particle {
    float lifetime;
};
struct Collider {
    float radius;

};
struct Follower {
    two::Entity target;
    float speed;
};
struct PhyObj {
    float2 velocity;
    bool rigid;
    float mass;
};

struct Emitter {
};
struct ItemHolder {
    bool items[5];
};