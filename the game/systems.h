#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "glad/glad.h"
#include <stdlib.h>


#include "utility.h"
#include "entity.h"
#include "components.h"
#include "inputManager.h"
#include "particleHelper.h"
#include "physicsHelper.h"
#include "rendering/Renderer.h"
#include "rendering/Texture.h"
#include "PlayerSkinManager.h"



//extern SDL_Renderer* gfx;
extern InputManager* g_InputManager;

class ParticleSystem : public two::System {
public:
    void update(two::World* world, float dt) override {
        two::Optional<two::Entity> attractor = world->view_one<Controller>();
        Transform at;
        if (attractor.has_value) {
            at = world->unpack<Transform>(attractor.value());
        }
        world->each<Transform, Particle, PhyObj>(
            [&](two::Entity entity, Transform& tf, Particle& p, PhyObj phy) {

                p.lifetime -= dt;


                /*sp.color.a = remap(MinLifetime - 1.f, MaxLifetime + 1.f,
                    0, 255.f, p.lifetime);*/

                if (p.lifetime <= 0.f) {
                    world->destroy_entity(entity);
                }
            });
    }
};

class SpriteRenderer : public two::System {
public:
    void load(two::World*) override {
        m_textures[0] = Texture::createTexture("./Assets/character-sprite.png");
        
        PlayerSkinManager::Init(m_textures[0]);
        m_textures[1] = Texture::createTexture("./Assets/items.png");

    }

    void update(two::World* world, float dt) override {
        
        for (two::Entity ent : world->view<Sprite, Transform>()) {
            auto& sprite = world->unpack<Sprite>(ent);
            auto& t = world->unpack<Transform>(ent);

            Renderer::DrawQuad(m_textures[sprite.spriteId], t.position, t.scale, t.rotation, (sprite.pos + sprite.offset),( sprite.pos + sprite.offset + sprite.scale));
        }
        
    }
    void unload(two::World* world) override {
        
    }
private:
    std::unordered_map<int,uint32_t> m_textures;
};

//class MoveSystem : public two::System {
//public:
//    void update(two::World* world, float) override {
//        int x, y;
//        SDL_GetMouseState(&x, &y);
//        world->each<Emitter>([x, y](Emitter& emitter) {
//            emitter.origin = float2(x, y);
//            });
//    }
//};

class ControlSystem : public two::System {
public:
    void update(two::World* world, float dt) override {
        float2 input = g_InputManager->GetMovement();
        world->each< Controller, PhyObj, Transform>([input, dt, world](two::Entity ent, Controller& controller, PhyObj& phyObj, Transform& t) {

            phyObj.velocity = input * (15);
            if (input.x != 0) {
                t.scale = { abs(t.scale.x)* input.x, t.scale.y };
            }
            
            if (world->contains<Animation>(ent)) {
                auto& anim = world->unpack<Animation>(ent);
                anim.animId = length(input) > 0.01f ? 1 : 0;
            }
            });
    }
};

class ShootSystem : public two::System {
public:
    void update(two::World* world, float dt) override {
        float2 input = g_InputManager->GetShootDir();
        float2 normed = norm(input);
        if (length(input) > 0.25f) {
            world->each<Controller, Transform>([input, dt, world, normed](Controller& controller, Transform& transform) {
                spawn_particle_dir(world, world->make_entity(), transform.position + normed*3, normed * 20 + rand_dir(1));
                });
        }


    }
};

class AnimationSystem : public two::System {
public:
    void load(two::World*) override {
        //runAnim
        
        animations.push_back(std::vector<float2>{{0,0}});
        animations.push_back(std::vector<float2>{{1.f/9.f, 0}, { 2.f / 9.f,0 }, { 3.f / 9.f,0 }, { 4.f / 9.f,0 }, { 5.f / 9.f,0 }, { 6.f / 9.f,0 }, { 7.f / 9.f,0 }, { 8.f / 9.f,0 }});
    }
    void update(two::World* world, float dt) override {
        for (two::Entity ent : world->view<Animation, Sprite>()) {
            auto& anim = world->unpack<Animation>(ent);
            anim.time += dt * 10;
            anim.time = std::fmod(anim.time, animations[anim.animId].size());
            int curFrame = static_cast<int>(anim.time);
            auto& sprite = world->unpack<Sprite>(ent);
            sprite.offset = animations[anim.animId][curFrame];

        }



    }
private:
    std::vector<std::vector<float2>> animations;
    
};

class FollowSystem : public two::System {
public:
    void update(two::World* world, float dt) override {
        world->each<Follower, PhyObj, Transform>([dt, world](Follower& f, PhyObj& phy, Transform& tf) {
            Transform target = world->unpack<Transform>(f.target);
            float2 diff = target.position - tf.position;
            diff = (norm(diff) * (f.speed));
            diff = diff - phy.velocity;
            phy.velocity = phy.velocity + diff * dt;
            });



    }
};
class PhysicsSystem : public two::System {
public:



    //}
    void update(two::World* world, float dt) override {
        //important lessons. unpacking is like the most expensive shit. Doing this n^2 times will cause huge laggs.
        //instead the first time arounf put those components in a vector. And then you can go over that... way faster
        std::vector<Collision> collisions;
        std::vector<Transform> transforms;
        std::vector<Collider> colliders;
        std::vector<two::Entity> colEntities;
        std::vector<float2> velocities;
        for (auto ent : world->view<Transform, PhyObj>()) {
            auto& tf = world->unpack<Transform>(ent);
            auto& phy = world->unpack<PhyObj>(ent);

            tf.position = tf.position + (phy.velocity * dt);
            if (phy.rigid && world->contains<Collider>(ent)) {

                auto& col = world->unpack<Collider>(ent);
                transforms.emplace_back(tf);
                colliders.emplace_back(col);
                colEntities.emplace_back(ent);
                velocities.emplace_back(phy.velocity);
            }

        }
        for (int i = 0; i < transforms.size(); i++) {
            for (int j = 0; j < transforms.size(); j++) {
                if (i >= j) {
                    continue;
                }
                float2 dirr = transforms[j].position - transforms[i].position;
                float dist = length(dirr);
                dirr = dirr / dist;
                if (dist < colliders[i].radius + colliders[j].radius) {
                    //hardcoded studd... should be deleted later
                    if (world->contains<Controller>(colEntities[i])) {
                        world->destroy_entity(colEntities[j]);
                        ItemHolder& ih = world->unpack<ItemHolder>(colEntities[i]);
                        if (ih.items[0]) {
                            ih.items[0] = 0;
                            ih.items[3] = 1;
                        }
                        else {
                            ih.items[1] = 0;
                            ih.items[4] = 1;
                        }
                        PlayerSkinManager::changePlayerSkin(ih);
                    }
                    else if (world->contains<Controller>(colEntities[j])) {
                        world->destroy_entity(colEntities[i]);
                        ItemHolder& ih = world->unpack<ItemHolder>(colEntities[j]);
                        if (ih.items[0]) {
                            ih.items[0] = 0;
                            ih.items[3] = 1;
                        }
                        else {
                            ih.items[1] = 0;
                            ih.items[4] = 1;
                        }
                        
                        PlayerSkinManager::changePlayerSkin(ih);
                    }
                    else {
                        collisions.emplace_back(colEntities[i], colEntities[j], transforms[i].position + dirr * colliders[i].radius, transforms[j].position - dirr * colliders[j].radius, velocities[i], velocities[j], dirr);
                    }


                    
                }
            }
        }
        //solve
        for (Collision col : collisions) {
            float2 diff = col.pointB - col.pointA;
            float2 normal = col.normal;

            auto& ta = world->unpack<Transform>(col.a);
            auto& tb = world->unpack<Transform>(col.b);
            auto& phyA = world->unpack<PhyObj>(col.a);
            auto& phyB = world->unpack<PhyObj>(col.b);
            float2 aVel = phyA.velocity;
            float2 bVel = phyB.velocity;
            float aMass = phyA.mass;
            float bMass = phyB.mass;
            float aInvMass = 1.f / aMass;
            float bInvMass = 1.f / bMass;

            //collision solver

            // Calculate total mass
            float totalMass = aMass + bMass;

            // Calculate the ratio of each object's mass to the total mass
            float ratioA = aMass / totalMass;
            float ratioB = bMass / totalMass;


            ta.position = ta.position + (diff * ratioB);
            tb.position = tb.position - (diff * ratioA);
            //impulse solver
            //assuming everything is a physics object for now lul
            float2 rVel = bVel - aVel;
            float nSpd = dot(rVel, normal);
            if (nSpd >= 0) {
                continue;
            }
            float j = (-1.0f * nSpd) / (aInvMass + bInvMass);




            float2 impulse = normal * j;
            aVel = aVel - impulse * aInvMass;
            bVel = bVel + impulse * bInvMass;


            phyA.velocity = aVel;
            phyB.velocity = bVel;
        }



    }
};

class ItemHandler : public two::System {
public:
    void load(two::World* world) override {
    }
    void update(two::World* world, float dt) override {
       



    }
private:
    std::vector<std::vector<float2>> animations;

};