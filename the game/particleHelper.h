#pragma once
#include "entity.h"
#include "components.h"

constexpr float MinLifetime = 1.f;
constexpr float MaxLifetime = 5.f;
constexpr float MaxSpeed = 15.f;
constexpr float MinSize = 0.2f;
constexpr float MaxSize = 0.5f;

static void spawn_particle(two::World* world, two::Entity entity,
    const float2& origin) {
    world->pack(entity,
        Transform{ origin, float2(randf(MinSize, MaxSize)), 45 },
        Sprite{ 1, {0,0}, {1,1}, {0,0} },
        Particle{ randf(MinLifetime, MaxLifetime) },
        PhyObj{ rand_dir(MaxSpeed), true, 1 });
}

static void spawn_particle_dir(two::World* world, two::Entity entity,
    const float2& origin, const float2& dir) {
    world->pack(entity,
        Transform{ origin, float2(randf(MinSize, MaxSize)), 45 },
        Sprite{ 1, {0,0}, {1,1}, {0,0} },
        Particle{ randf(MinLifetime, MaxLifetime) },
        PhyObj{ dir, true, 0.5f },
        Collider{ 0.2f });
}