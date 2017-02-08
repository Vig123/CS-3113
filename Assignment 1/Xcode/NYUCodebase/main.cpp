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

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


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
    ShaderProgram program("/Users/QuanhaoLi/Desktop/CS3113/project_template/Xcode/NYUCodebase/vertex_textured.glsl", "/Users/QuanhaoLi/Desktop/CS3113/project_template/Xcode/NYUCodebase/fragment_textured.glsl");
    
    // Loads a blue alien
    GLuint blueAlien = LoadTexture(RESOURCE_FOLDER"alienBlue_hurt.png");
    
    // Loads a green alien
    GLuint greenAlien = LoadTexture(RESOURCE_FOLDER"p1_hurt.png");
    
    GLuint plane = LoadTexture(RESOURCE_FOLDER"planeBlue1.png");
    
    // Initializes 2 matrices
    Matrix projectionMatrix; // for aspect ratio
    Matrix viewMatrix;
    
    // Initializes a model matrix for every object
    Matrix modelMatrixForBlue;
    Matrix modelMatrixForGreen;
    Matrix modelMatrixForPlane;
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    // Shift the matrices for the emojis either a bit left or a bit right
    program.setModelMatrix(modelMatrixForBlue);
    modelMatrixForBlue.Translate(-0.7f, 0, 0);
    program.setModelMatrix(modelMatrixForGreen);
    modelMatrixForGreen.Translate(0.7f, 0, 0);
    
    // Initialized the last frame tick for animation
    float lastFrameTicks = 0.0f;
    
    // Movement variables
    float xDir (0), yDir (0);
    bool up (false), right (true), down (false), left (false);
    
    srand((unsigned int) time (NULL));
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        // Time since last frame
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        float red = ((float) rand() / (RAND_MAX));
        float blue = ((float) rand() / (RAND_MAX));
        float green = ((float) rand() / (RAND_MAX));
        
        // Sets clear color as white
        glClearColor(red, green, blue, 1.0);
        
        // Clears screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        // Initializes the locations of the emojis and textures
        float regV[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        float planeV[] = {-3.5, -2, -2.5, -2, -2.5, -1, -3.5, -2, -2.5, -1, -3.5, -1};
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        
        program.setModelMatrix(modelMatrixForBlue);
        // Binds the blue alien
        glBindTexture(GL_TEXTURE_2D, blueAlien);
        // Displays the blue alien
        genImage(program, regV, texCoords);
        // Rotates the alien
        modelMatrixForBlue.Rotate(elapsed*5);
        
        program.setModelMatrix(modelMatrixForGreen);
        // Binds the green alien
        glBindTexture(GL_TEXTURE_2D, greenAlien);
        // Displays the green alien
        genImage(program, regV, texCoords);
        // rotates the alien
        modelMatrixForGreen.Rotate(elapsed*5);
        
        // Sets the model Matrix as the model Matrix for the plane
        program.setModelMatrix(modelMatrixForPlane);
        // Binds the plane emoji
        glBindTexture(GL_TEXTURE_2D, plane);
        // Displays the plane emoji
        genImage(program, planeV, texCoords);
        //modelMatrixForPlane.Translate(ticks*0.1f, 0.0, 0.0);
        
        
        // Moving the plane
        // Several cases to check which way to move and when to change direction
        // Change to moving up
        if (xDir >= 5.9) {
            right = false;
            up = true;
            left = false;
            down = false;
            yDir = 0;
            xDir = 0;
        }
        // Change to moving left
        else if (yDir >= 2.9) {
            right = false;
            up = false;
            left = true;
            down = false;
            xDir = 0;
            yDir = 0;
        }
        // Change to moving down
        else if (xDir <= -5.9) {
            right = false;
            up = false;
            left = false;
            down = true;
            xDir = 0;
            yDir = 0;
        }
        // Change to moving up
        else if (yDir <= -2.9) {
            right = true;
            up = false;
            left = false;
            down = false;
            xDir = 0;
            yDir = 0;
            // resets the movelMatrix for the plane
            modelMatrixForPlane.identity();
        }
        
        // Moving right
        if (right) {
            modelMatrixForPlane.Translate(0.1, 0, 0);
            xDir += 0.1;
        }
        // Moving up
        else if (up) {
            modelMatrixForPlane.Translate(0, 0.1, 0);
            yDir += 0.1;
        }
        // Moving left
        else if (left) {
            modelMatrixForPlane.Translate(-0.1, 0, 0);
            xDir -= 0.1;
        }
        // Moving down
        else if (down) {
            modelMatrixForPlane.Translate(0, -0.1, 0);
            yDir -= 0.1;
        }
        
        
        // Swaps to the display window
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}


