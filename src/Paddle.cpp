#include "Paddle.hpp"

#include "World.hpp"

#include "Ball.hpp"

using namespace glm;

static const float bounceScaleFactor = 2.2f; // Increase size
static const float bounceDuration = 0.2f; 

void Paddle::Start() {
    scale = vec3(20.0f, 100.0f, 0.0f);

    
    if (name == "LeftPaddle") {
        position = glm::vec3(50.0f, window->GetScreenHeight() * 0.5f, 0.0f);
    } else if (name == "RightPaddle") {
        position = glm::vec3(window->GetScreenWidth() - 50.0f, window->GetScreenHeight() * 0.5f, 0.0f);
    }
}

void Paddle::Update(float _dt) {
    vec2 dir;

    if (name == "LeftPaddle")
    {
        dir.y += inputManager->GetKey(SDL_SCANCODE_W);
        dir.y += inputManager->GetKey(SDL_SCANCODE_S) * -1;
    }
    else if (name == "RightPaddle") {
        
        Ball* ball = world->FindByName<Ball>("Ball"); 

        if (ball) { 
            float ballY = ball->position.y;
            float paddleY = position.y;

           
            if (ballY > paddleY + 10.0f) {
                position.y += speed * _dt; 
            } else if (ballY < paddleY - 10.0f) {
                position.y -= speed * _dt; 
            }
        }
    }

    if (bounceTimer > 0.0f) {
        bounceTimer -= _dt; 
        if (bounceTimer <= 0.0f) {
            scaleModifier = 1.0f; //reset
        }
    }

    float speedMultiplier = 1.5f; //can change padddle speed 
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