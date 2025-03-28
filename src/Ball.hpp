#pragma once

#include "Entity.hpp"

#include "World.hpp"
#include <deque>

#include <SDL.h>
#include <GL/glew.h>

class Ball : public Entity {
public:
    void Start();
    void Update(float _dt);
    void Draw();
    void OnDestroy();

    float speed = 100.0f;
    glm::vec2 dir = glm::vec2(0.0f, 0.0f);
    std::deque<glm::vec3> previousPositions;
    float scaleModifier = 1.0f;
    float bounceTimer = 0.0f;   

private:
    Canis::GLTexture leftPaddleTexture;
    Canis::GLTexture rightPaddleTexture;
    Canis::Shader spriteShader;
};