#include "Ball.hpp"

#include "Paddle.hpp"

#include "World.hpp"

#include "Canis/IOManager.hpp"
#include <SDL.h>
#include <GL/glew.h>

using namespace glm;

int leftScore = 0;
int rightScore = 0;
const float posSaveDistance = 60.0f;
static const float bounceScaleFactor = 2.2f; // Increase size
static const float bounceDuration = 0.2f; 

void Ball::Start() {


    name = "Ball";
    position = vec3(window->GetScreenWidth() * 0.5f, window->GetScreenHeight() * 0.5f, 0.0f);
    scale = vec3(100.0f, 100.0f, 0.0f);
    previousPositions.clear();

    spriteShader.Compile("assets/shaders/sprite.vs", "assets/shaders/sprite.fs");
    spriteShader.AddAttribute("aPos");
    spriteShader.AddAttribute("aUV");
    spriteShader.Link();

    texture = texture;

    int textureSlots = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);
    Canis::Log(std::to_string(textureSlots));
    spriteShader.SetInt("texture1", 0);
}

void Ball::Update(float _dt) {

    if (dir == vec2(0.0f))
    {
        if (inputManager->GetKey(SDL_SCANCODE_SPACE))
        {
            vec2 directions[] = {vec2(1.0f, 1.0f), vec2(1.0f, -1.0f), vec2(-1.0f, 1.0f), vec2(-1.0f, -1.0f)};
            dir = directions[rand()%4];
        }
    }
    
    if (position.y > window->GetScreenHeight() - (scale.y * 0.5f)) {
        position.y = window->GetScreenHeight() - (scale.y * 0.5f);
        dir.y = -dir.y; 
    }
    
    if (position.y < scale.y * 0.5f) {
        position.y = scale.y * 0.5f;
        dir.y = abs(dir.y); 
    }

    static float trailTimer = 0.0f;
    if (dir != vec2(0.0f))
    {
        trailTimer += _dt;
        if (trailTimer > 0.05f) {
            // Interpolate new position for a smooth trailing effect
            if (!previousPositions.empty()) {
                vec3 interpolatedPos = mix(previousPositions.front(), position, 0.1f);
                previousPositions.push_front(interpolatedPos);
            } else {
                previousPositions.push_front(position);
            }

            if (previousPositions.size() > 5) {
                previousPositions.pop_back();
            }

            trailTimer = 0.0f;
        }
    }

    // detect score
    if (position.x > window->GetScreenWidth() - (scale.x * 0.5f)) {
        position = vec3(window->GetScreenWidth()*0.5f, window->GetScreenHeight()*0.5f, 0.0f);
        leftScore++;
        dir = vec2(0.0f);
        previousPositions.clear();
    }
    if (position.x < scale.x * 0.5f) {
        position = vec3(window->GetScreenWidth()*0.5f, window->GetScreenHeight()*0.5f, 0.0f);
        rightScore++;
        dir = vec2(0.0f);
        previousPositions.clear();
    }

    window->SetWindowName("Pong -- Left Score: " + std::to_string(leftScore) + " -- Right Score: " + std::to_string(rightScore));

    if (leftScore >= 5 || rightScore >= 5) {
        window->SetWindowName("Pong -- Left Score: " + std::to_string(leftScore) + " -- Right Score: " + std::to_string(rightScore) + " GAME OVER!!");
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);  
        //end game winner function here Endgame();
    }


    // detect if ball hits left paddle
    Paddle* leftPaddle = world->FindByName<Paddle>("LeftPaddle"); 
    if (EntityOverlap2D(*this ,*leftPaddle)) {
        dir.x = abs(dir.x);
        texture = leftPaddle->texture;

        scaleModifier = bounceScaleFactor;
        bounceTimer = bounceDuration;
        leftPaddle->scaleModifier = bounceScaleFactor;
        leftPaddle->bounceTimer = bounceDuration;
    }

    // detect if ball hits right paddle
    Paddle* rightPaddle = world->FindByName<Paddle>("RightPaddle"); 
    if (EntityOverlap2D(*this ,*rightPaddle)) {
        dir.x = abs(dir.x) * -1.0f;
        texture = rightPaddle->texture;

        scaleModifier = bounceScaleFactor;
        rightPaddle->scaleModifier = bounceScaleFactor;
        bounceTimer = bounceDuration;
        rightPaddle->bounceTimer = bounceDuration;
    }

    if (bounceTimer > 0.0f) {
        bounceTimer -= _dt; 
        if (bounceTimer <= 0.0f) {
            scaleModifier = 1.0f; //reset size
        }
    }

    float speedMultiplier = 2.5f; //change ball speed
    if (dir != vec2(0.0f))
    {
        position += vec3(dir.x, dir.y, 0.0f) * speed * speedMultiplier * _dt;
    }
}

void Ball::Draw() {mat4 transform = mat4(1.0f);
    transform = translate(transform, position - (scale * (scaleModifier - 1.0f) * 0.5f));
    transform = glm::scale(transform, scale * scaleModifier);

    // set shader variables
    shader.SetVec4("COLOR", color);
    shader.SetMat4("TRANSFORM", transform);

    shader.SetInt("texture1", 0);  // Set the uniform for the texture
    glBindTexture(GL_TEXTURE_2D, texture.id);  // Bind the texture
    float alpha = 0.5f;
    float scaleFactor = 0.8f;

    shader.Use();

    for (size_t i = 0; i < previousPositions.size(); i++) {
        float offsetFactor = 1.0f - (i * 0.2f); // Decrease offset over time for gradual trailing effect
        vec3 offsetPos = mix(previousPositions[i], position, offsetFactor); // Blend positions for a smoother effect

        mat4 ghostTransform = mat4(1.0f);
        ghostTransform = translate(ghostTransform, offsetPos);
        ghostTransform = glm::scale(ghostTransform, scale * scaleFactor);

        shader.SetVec4("COLOR", vec4(color.r, color.g, color.b, alpha));
        shader.SetMat4("TRANSFORM", ghostTransform);

        glBindVertexArray(world->VAO);
    }
        scaleFactor *= 0.8f;
        alpha *= 0.7f;

        glBindVertexArray(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // Render ghost images

}

void Ball::OnDestroy() {
    
}