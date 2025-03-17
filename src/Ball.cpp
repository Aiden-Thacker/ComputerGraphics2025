#include "Ball.hpp"

#include "Paddle.hpp"

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

    // Store position at fixed time intervals for smoother trails
    static float trailTimer = 0.0f;
    trailTimer += _dt;
    if (trailTimer > 0.05f) {
        previousPositions.push_front(position);
        if (previousPositions.size() > 5) {
            previousPositions.pop_back();
        }
        trailTimer = 0.0f;
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
        SDL_Event quitEvent;  //temporary
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);  
        //end game winner function here Endgame();
    }


    // detect if ball hits left paddle
    Paddle* leftPaddle = world->FindByName<Paddle>("LeftPaddle"); 
    if (EntityOverlap2D(*this ,*leftPaddle)) {
        dir.x = abs(dir.x);

        scaleModifier = bounceScaleFactor;
        bounceTimer = bounceDuration;
        leftPaddle->scaleModifier = bounceScaleFactor;
        leftPaddle->bounceTimer = bounceDuration;
    }

    // detect if ball hits right paddle
    Paddle* rightPaddle = world->FindByName<Paddle>("RightPaddle"); 
    if (EntityOverlap2D(*this ,*rightPaddle)) {
        dir.x = abs(dir.x) * -1.0f;

        scaleModifier = bounceScaleFactor;
        bounceTimer = bounceDuration;
        rightPaddle->scaleModifier = bounceScaleFactor;
        rightPaddle->bounceTimer = bounceDuration;
    }

    if (bounceTimer > 0.0f) {
        bounceTimer -= _dt; 
        if (bounceTimer <= 0.0f) {
            scaleModifier = 1.0f; //reset size
        }
    }

    float speedMultiplier = 1.35f; //change ball speed
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

    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Render ghost images
    float alpha = 0.5f;
    float scaleFactor = 0.8f;

    shader.Use();

    for (size_t i = 0; i < previousPositions.size(); i++) {
        mat4 ghostTransform = mat4(1.0f);
        ghostTransform = translate(ghostTransform, previousPositions[i]);
        ghostTransform = glm::scale(ghostTransform, scale * scaleFactor);

        shader.SetVec4("COLOR", vec4(color.r, color.g, color.b, alpha));
        shader.SetMat4("TRANSFORM", ghostTransform);

        glBindVertexArray(world->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        alpha *= 0.7f;
        scaleFactor *= 0.8f;
    }
}

void Ball::OnDestroy() {
    
}