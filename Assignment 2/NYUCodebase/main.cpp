#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

using namespace std;

bool checkCollision (Entity& a, Entity& b) {
    if ((a.y - a.height/2) > (b.y + b.height/2)) {
        return true;
    }
    if ((a.y + a.height/2) < (b.y - b.height/2)) {
        return true;
    }
    if ((a.x - a.width/2) > (b.x + b.width/2)) {
        return true;
    }
    if ((a.x + a.width/2) < (b.x - b.width/2)) {
        return true;
    }
    
    return false;
}

void genImage (ShaderProgram& program, float vertex[], float texCoords[]) {
    
    // Defines array of Vertex Data
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertex);
    glEnableVertexAttribArray(program.positionAttribute);
    
    // Defines an array of Texture Data
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    // Draws the triangles that will be textured
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Disables program attributes after use
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
}

GLuint LoadTexture(const char *filePath) {
    
    // Load pixel data from an image file
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    
    // Checks if image exists in path provided
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint retTexture;
    
    // Generates a new OpenGL texture ID
    glGenTextures(1, &retTexture);
    
    // Binds a texture to a texture target
    glBindTexture(GL_TEXTURE_2D, retTexture);
    
    // Sets texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // Sets a texture parameter of the specified texture target
    // GL_NEAREST is used for pixelated art
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Frees the image after use
    stbi_image_free(image);
    
    // Returns the texture
    return retTexture;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Sets the size and offset of rendering area (in pixels).
    glViewport(0, 0, 640, 360);
    
    // Opens textures files for vertex and fragment
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    ShaderProgram program2(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    
    GLuint pongBackground = LoadTexture(RESOURCE_FOLDER"pongBackground.png");
    
    srand((unsigned int) time (NULL));
    
    int x_dir, y_dir;
    
    int ran1 = rand() % 2;
    if (ran1 == 1) x_dir = 1;
    else x_dir = -1;
    ran1 = rand() % 2;
    if (ran1 == 1) y_dir = 1;
    else y_dir = -1;
    
    Entity pongBall (0, 0, 0, 0.2, 0.2, 0.1, x_dir, y_dir);
    Entity leftPaddle (-3.45, 0, 0, 0.1, 0.8, 2, 0, 0);
    Entity rightPaddle (3.45, 0, 0, 0.1, 0.8, 2, 0, 0);
    Entity topWall (0, 1.9, 0, 7, 0.1, 0, 0, 0);
    Entity botWall (0, -1.9, 0, 7, 0.1, 0, 0, 0);
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    Matrix modelMatrix;
    
    projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    
    float lastFrameTicks = 0.0f;
    
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if(keys[SDL_SCANCODE_UP] && checkCollision(topWall, rightPaddle)) {
            rightPaddle.y += elapsed * rightPaddle.speed;
        } else if(keys[SDL_SCANCODE_DOWN] && checkCollision(botWall, rightPaddle)) {
            rightPaddle.y -= elapsed * rightPaddle.speed;
        }
        if(keys[SDL_SCANCODE_W] && checkCollision(topWall, leftPaddle)) {
            leftPaddle.y += elapsed * leftPaddle.speed;
        }
        else if (keys[SDL_SCANCODE_S] && checkCollision(botWall, leftPaddle)) {
            leftPaddle.y -= elapsed * leftPaddle.speed;
        }
        
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float bVertices[] = {3.55, 2.0, -3.55, 2.0, -3.55, -2.0, -3.55, -2.0, 3.55, -2.0, 3.55, 2.0};
        float bTextures[] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
        
        glUseProgram(program.programID);
        
        modelMatrix.identity();
        
        program.setModelMatrix(modelMatrix);
        program.setViewMatrix(viewMatrix);
        program.setProjectionMatrix(projectionMatrix);
        
        glBindTexture(GL_TEXTURE_2D, pongBackground);
        genImage(program, bVertices, bTextures);
        
        glUseProgram(program2.programID);
        program2.setProjectionMatrix(projectionMatrix);
        program2.setViewMatrix(viewMatrix);

        // Top Wall
        modelMatrix.identity();
        program2.setModelMatrix(modelMatrix);
        float WallVOne[] = {3.55, 2.0, -3.55, 2.0, -3.55, 1.9, -3.55, 1.9, 3.55, 1.9, 3.55, 2.0};
        topWall.Draw(program2, WallVOne);
        
        // Bottom Wall
        modelMatrix.identity();
        program2.setModelMatrix(modelMatrix);
        float WallVTwo[] = {3.55, -1.9, -3.55, -1.9, -3.55, -2.0, -3.55, -2.0, 3.55, -2.0, 3.55, -1.9};
        botWall.Draw(program2, WallVTwo);
        
        // Right Paddle
        modelMatrix.identity();
        modelMatrix.Translate(0, rightPaddle.y - rightPaddle.init_y, 0);
        program2.setModelMatrix(modelMatrix);
        float paddleOne[] = {3.55, 0.4, 3.45, 0.4, 3.45, -0.4, 3.45, -0.4, 3.55, -0.4, 3.55, 0.4};
        rightPaddle.Draw(program2, paddleOne);
        
        // Left Paddle
        modelMatrix.identity();
        modelMatrix.Translate(0, leftPaddle.y - leftPaddle.init_y, 0);
        program2.setModelMatrix(modelMatrix);
        float paddleTwo[] = {-3.55, 0.4, -3.45, 0.4, -3.45, -0.4, -3.45, -0.4, -3.55, -0.4, -3.55, 0.4};
        leftPaddle.Draw(program2, paddleTwo);
        
        // Ball
        if (!checkCollision(topWall, pongBall)) pongBall.direction_y = -1;
        if (!checkCollision(botWall, pongBall)) pongBall.direction_y = 1;
        if (!checkCollision(leftPaddle, pongBall)) pongBall.direction_x = 1;
        if (!checkCollision(rightPaddle, pongBall)) pongBall.direction_x = -1;
        pongBall.x += pongBall.direction_x*pongBall.speed;
        pongBall.y += pongBall.direction_y*pongBall.speed;
        modelMatrix.identity();
        modelMatrix.Translate(pongBall.x-pongBall.init_x, pongBall.y-pongBall.init_y, 0);
        program2.setModelMatrix(modelMatrix);
        float ballV[] = {0.1, 0.1, -0.1, 0.1, -0.1, -0.1, -0.1, -0.1, 0.1, -0.1, 0.1, 0.1};
        pongBall.Draw(program2, ballV);
        
        if (pongBall.x > 3.55 || pongBall.x < -3.55 || pongBall.y > 2.0 || pongBall.y < -2.0) {
            rightPaddle.y = rightPaddle.init_y;
            leftPaddle.y = leftPaddle.init_y;
            pongBall.x = pongBall.init_x;
            pongBall.y = pongBall.init_y;
            glClearColor(1.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
