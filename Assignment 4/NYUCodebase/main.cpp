#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include "Matrix.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "Entity.h"

using namespace std;

SDL_Window* displayWindow;

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

int mapWidth;
int mapHeight;
unsigned char** levelData;
float TILE_SIZE = 0.5;
int SPRITE_COUNT_X = 16;
int SPRITE_COUNT_Y = 8;
vector<Entity*> e;


void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}

bool isSolid (int x, int y) {
    if (x > mapWidth || x < 0 || y > mapHeight || y < 0) return false;
    return levelData[y][x] != 0;
}

void entityTileCollision (Entity* e) {
    int gridX, gridY;
    
    worldToTileCoordinates(e->x + e->sprite.size/2, e->y, &gridX, &gridY);
    if (isSolid(gridX, gridY)) {
        e->velocity_x = 0;
        e->x -= 0.05f + TILE_SIZE*gridX + TILE_SIZE - e->x-e->sprite.size/2 ;
        e->collidedRight = true;
    }
    else {e->collidedRight = false;}
    
    worldToTileCoordinates(e->x - e->sprite.size/2, e->y, &gridX, &gridY);
    if (isSolid(gridX, gridY)) {
        e->velocity_x = 0;
        e->x += 0.05f + e->x+e->sprite.size/2 - TILE_SIZE*gridX;
        e->collidedLeft = true;
    }
    else {e->collidedLeft = false;}
    
    worldToTileCoordinates(e->x, e->y + e->sprite.size/2, &gridX, &gridY);
    if (isSolid(gridX, gridY)) {
        e->velocity_y = 0;
        e->y -= e->y + e->sprite.size/2 - ((-TILE_SIZE*gridY) - TILE_SIZE);
        e->collidedTop = true;
    }
    else {e->collidedTop = false;}
    
    worldToTileCoordinates(e->x, e->y - e->sprite.size/2, &gridX, &gridY);
    if (isSolid(gridX, gridY)) {
        e->velocity_y = 0;
        e->y += (-TILE_SIZE*gridY) - (e->y - e->sprite.size/2);
        e->collidedBottom = true;
    }
    else {e->collidedBottom = false;}
}

void placeEntity(string& type, float placeX, float placeY) {
    Entity* ent = new Entity;
    if (type == "Hero") {
        ent->entityType = ENTITY_PLAYER;
        ent->isStatic = false;
    }
    else if (type == "Collectable") {
        ent->entityType = ENTITY_COIN;
        ent->isStatic = true;
    }
    ent->x = placeX;
    ent->y = placeY;
    e.push_back(ent);
}

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    SDL_FreeSurface(surface);
    
    return textureID;
}

void DrawTiles (ShaderProgram *program, GLuint spriteSheetTexture) {
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    
    int amtToDraw = 0;
    
    for(int y=0; y < mapHeight; y++) {
        for(int x=0; x < mapWidth; x++) {
            if (levelData[y][x]!= 0) {
                amtToDraw += 1;
                float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
                float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
                float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
                float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
                vertexData.insert(vertexData.end(), {
                    TILE_SIZE * x, -TILE_SIZE * y,
                    TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
                    (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                    TILE_SIZE * x, -TILE_SIZE * y,
                    (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                    (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
                });
                texCoordData.insert(texCoordData.end(), {
                    u, v,
                    u, v+(spriteHeight),
                    u+spriteWidth, v+(spriteHeight),
                    u, v,
                    u+spriteWidth, v+(spriteHeight),
                    u+spriteWidth, v
                });
            }
        }
    }
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
    glDrawArrays(GL_TRIANGLES, 0, amtToDraw*6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool readHeader(std::ifstream &stream) {
    string line;
    mapWidth = -1;
    mapHeight = -1;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "width") {
            mapWidth = atoi(value.c_str());
        } else if(key == "height"){
            mapHeight = atoi(value.c_str());
        } }
    if(mapWidth == -1 || mapHeight == -1) {
        return false;
    } else { // allocate our map data
        levelData = new unsigned char*[mapHeight];
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
        }
        return true;
    }
}

bool readLayerData(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "data") {
            for(int y=0; y < mapHeight; y++) {
                getline(stream, line);
                istringstream lineStream(line);
                string tile;
                for(int x=0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val =  (unsigned char)atoi(tile.c_str());
                    if(val > 0) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        levelData[y][x] = val-1;
                    } else {
                        levelData[y][x] = 0;
                    }
                    //printf("%i", levelData[y][x]);
                }
            }
        }
    }
    return true;
}

bool readEntityData(std::ifstream &stream) {
    string line;
    string type;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "type") {
            type = value;
        }
        else if(key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            placeEntity(type, placeX, placeY);
        }
    }
    return true;
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
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    Matrix modelMatrix;
    
    projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
    
    GLuint spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"arne_sprites.png");
    GLuint playerTexture = LoadTexture(RESOURCE_FOLDER"characters_1.png");
    SpriteSheet playerSprite (playerTexture, 0, 32.0f/128.0f, 16.0f/192.0f, 16.0f/128.0f, 0.5f);
    SpriteSheet coinSprite (spriteSheetTexture, 48.0f/256.0f, 48.0f/128.0f, 16.0f/256.0f, 16.0f/128.0f, 0.5f);
    
    ifstream infile(RESOURCE_FOLDER"mymap.txt");
    string line;
    int l = 0;
    while (getline(infile, line)) {
        if(line == "[header]") {
            if(!readHeader(infile)) {
                return 0; }
        }
        else if(line == "[layer]") {
            readLayerData(infile);
        }
        else if(line == "[Object Layer]") {
            readEntityData(infile);
            l++;
        }
    }
    infile.std::__1::ios_base::clear();
    infile.seekg(0);
    infile.close();
    
    for (int i = 0; i < e.size(); i++) {
        if (e[i]->entityType == ENTITY_PLAYER) {
            e[i]->init(playerSprite, playerSprite.width, playerSprite.height, 0, 0, 0, 0, 1.5, 1);
        }
        else if (e[i]->entityType == ENTITY_COIN) {
            e[i]->init(coinSprite, coinSprite.width, coinSprite.height, 0, 0, 0, 0, 0, 0);
        }
    }

    
    program.setProjectionMatrix(projectionMatrix);
    
    float lastFrameTicks = 0.0f;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (e[0] && (event.key.keysym.scancode == SDL_SCANCODE_UP) && e[0]->collidedBottom) {
                    e[0]->acceleration_y = 3.0f;
                }
            }
            else if (event.type == SDL_KEYUP) {
                if (e[0] && (event.key.keysym.scancode == SDL_SCANCODE_LEFT || event.key.keysym.scancode == SDL_SCANCODE_RIGHT)) {
                    e[0]->acceleration_x = 0.0f;
                }
            }
        }
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
            e[0]->Update(FIXED_TIMESTEP);
        }
        e[0]->Update(fixedElapsed);
        
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setProjectionMatrix(projectionMatrix);
        glUseProgram(program.programID);
        
        modelMatrix.identity();
        program.setModelMatrix(modelMatrix);
        DrawTiles(&program, spriteSheetTexture);
        
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if(e[0] && keys[SDL_SCANCODE_LEFT] && !e[0]->collidedLeft) {
            e[0]->acceleration_x -= 0.1f;
        }
        else if(e[0] && keys[SDL_SCANCODE_RIGHT] && !e[0]->collidedRight) {
            e[0]->acceleration_x +=  0.1f;
        }
        
        viewMatrix.identity();
        viewMatrix.Translate(-(e[0]->x - e[0]->init_x), -(e[0]->y - e[0]->init_y), 0);
        program.setViewMatrix(viewMatrix);
        entityTileCollision(e[0]);
        
        for (int i = 0; i < e.size(); i++) {
            if (e[i] == nullptr) continue;
            e[i]->Update(FIXED_TIMESTEP);
            e[i]->Render(&program, modelMatrix);
        }
        
        for (int i = 1; i < e.size(); i++) {
            if (e[i]!= nullptr) e[0]->collidesWith(e[i]);
        }
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
