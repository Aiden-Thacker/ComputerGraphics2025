#include "Paddle.hpp"

#include "World.hpp"

using namespace glm;

static const float bounceScaleFactor = 2.2f; // Increase size
static const float bounceDuration = 0.2f; 

void Paddle::Start() {
    scale = vec3(20.0f, 100.0f, 0.0f);
}

void Paddle::Update(float _dt) {
    vec2 dir;

    if (name == "LeftPaddle")
    {
        dir.y += inputManager->GetKey(SDL_SCANCODE_W);
        dir.y += inputManager->GetKey(SDL_SCANCODE_S) * -1;
    }
    else if (name == "RightPaddle")
    {
        dir.y += inputManager->GetKey(SDL_SCANCODE_UP);
        dir.y += inputManager->GetKey(SDL_SCANCODE_DOWN) * -1;
    }

    if (bounceTimer > 0.0f) {
        bounceTimer -= _dt; 
        if (bounceTimer <= 0.0f) {
            scaleModifier = 1.0f; //reset
        }
    }

    float speedMultiplier = 3.5f; //can change padddle speed 
    position.y += dir.y * speed * speedMultiplier * _dt;

    if (position.y > window->GetScreenHeight() - (scale.y * 0.5f))
        position.y = window->GetScreenHeight() - (scale.y * 0.5f);
    if (position.y < scale.y * 0.5f)
        position.y = scale.y * 0.5f;
}

void Paddle::Draw() {
    mat4 transform = mat4(1.0f);
    transform = translate(transform, position);
    transform = glm::scale(transform, scale * scaleModifier);

    // set shader variables
    shader.SetVec4("COLOR", color);
    shader.SetMat4("TRANSFORM", transform);
}

void Paddle::OnDestroy() {
    
}