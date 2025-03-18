#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>

#include <SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Canis/Canis.hpp"
#include "Canis/IOManager.hpp"
#include "Canis/FrameRateManager.hpp"

#include "Entity.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

// git restore .
// git fetch
// git pull

// From Kaden: Hi!

// move out to external class
unsigned int vertexShader;
unsigned int VBO, VAO, EBO;

void InitModel();

#ifdef _WIN32
#define main SDL_main
extern "C" int main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    Canis::Init();

    Canis::Window window;
    window.Create("Computer Graphics 2025", 640, 640, 0);

    Canis::InputManager inputManager;
    Canis::FrameRateManager frameRateManager;
    frameRateManager.Init(60.0f);
    float deltaTime = 0.0f;
    float fps = 0.0f;

    Canis::Shader spriteShader;
    spriteShader.Compile("assets/shaders/sprite.vs", "assets/shaders/sprite.fs");
    spriteShader.AddAttribute("aPos");
    spriteShader.AddAttribute("aUV");
    spriteShader.Link();

    InitModel();

    Canis::GLTexture texture = Canis::LoadImageGL("assets/textures/White_Wool_Block.png", true);
    Canis::GLTexture texture2 = Canis::LoadImageGL("assets/textures/Blue_Wool_Texture.png", true);
    Canis::GLTexture texture3 = Canis::LoadImageGL("assets/textures/Red_Wool_Texture.png", true);
    Canis::GLTexture gridTexture = Canis::LoadImageGL("assets/textures/grid.png", true);
    
    if (gridTexture.id == 0) {
        std::cerr << "Grid texture failed to load." << std::endl;
    } else {
        std::cout << "Grid texture loaded successfully with ID: " << gridTexture.id << std::endl;
    }
    
    int textureSlots = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);

    Canis::Log(std::to_string(textureSlots));

    spriteShader.SetInt("texture1", 0);

    glBindTexture(GL_TEXTURE_2D, texture.id);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    World world;
    world.VAO = VAO;
    world.window = &window;
    world.inputManager = &inputManager;

    Ball *ball = world.Instantiate<Ball>();
    ball->shader = spriteShader;
    ball->texture = texture;

    {
        Paddle *paddle = world.Instantiate<Paddle>();
        paddle->shader = spriteShader;
        paddle->texture = texture3;
        paddle->name = "RightPaddle";
        paddle->position = glm::vec3(window.GetScreenWidth() - (10.0f*0.5f), window.GetScreenHeight() * 0.5f, 0.0f);
    }

    {
        Paddle *paddle = world.Instantiate<Paddle>();
        paddle->shader = spriteShader;
        paddle->texture = texture2;
        paddle->name = "LeftPaddle";
        paddle->position = glm::vec3(10.0f*0.5f, window.GetScreenHeight() * 0.5f, 0.0f);
    }

    while (inputManager.Update(window.GetScreenWidth(), window.GetScreenHeight()))
    {
        deltaTime = frameRateManager.StartFrame();
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        using namespace glm;

        mat4 projection = ortho(0.0f, (float)window.GetScreenWidth(), 0.0f, (float)window.GetScreenHeight(), 0.001f, 100.0f);
        
        mat4 view = mat4(1.0f);
        view = translate(view, vec3(0.0f, 0.0f, 0.5f));
        view = inverse(view);

        // Draw grid
        spriteShader.Use();

        // Applies scaling transformation to cover the entire screen
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(window.GetScreenWidth(), window.GetScreenHeight(), 1.0f));

        // Applies a translation to center the grid
        // Calculates the position where the texture should be placed as centered
        float gridWidth = gridTexture.width;
        float gridHeight = gridTexture.height;
        glm::vec3 gridOffset = glm::vec3((window.GetScreenWidth() - gridWidth) * -1.73f, (window.GetScreenHeight() - gridHeight) * -1.73f, 0.0f);
        
        glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), gridOffset);

        // Combines scaling and translation
        glm::mat4 transformMat = translationMat * scaleMat;  // Applies translation first, then scales

        spriteShader.SetMat4("TRANSFORM", transformMat);  // Applies scaling and translation to the grid texture

        spriteShader.SetInt("texture1", 0);

        // Debug: Ensures the texture is bound and valid
        if (gridTexture.id == 0) {
            std::cerr << "Error: Grid texture is not valid!" << std::endl;
        } else {
            glBindTexture(GL_TEXTURE_2D, gridTexture.id);
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        
        world.Update(view, projection, deltaTime);

        window.SwapBuffer();

        fps = frameRateManager.EndFrame();

        Canis::Log(std::to_string(fps));
    }

    return 0;
}

void InitModel()
{
    float vertices[] = {
        // position         // uv
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}