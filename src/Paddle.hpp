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
};