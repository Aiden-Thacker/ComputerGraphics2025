#pragma once

#include "Entity.hpp"

#include "World.hpp"

class Paddle : public Entity {
public:
    void Start();
    void Update(float _dt);
    void Draw();
    void OnDestroy();

    float speed = 200.0f;
    float scaleModifier = 1.0f; 
    float bounceTimer = 0.0f;   
};