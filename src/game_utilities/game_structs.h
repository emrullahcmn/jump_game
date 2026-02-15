#ifndef GAME_STRUCTS_H
#define GAME_STRUCTS_H

#include <raylib.h>

// static obstacle struct whose instances we put into an array and use that array to create obstacles on the game screen
typedef struct Obstacle{
    Rectangle rect;
    Color color;
} Obstacle;

// obstacle index and its position on game screen
typedef struct ActiveObstacle{
    int index;
    Vector2 pos;
} ActiveObstacle;

typedef struct Animation{
    Rectangle *frameRecStatic;   
    Rectangle frameRec;
    int currentFrame;
    int framesCounter;
    int framesSpeed;
} Animation;

typedef struct Collision{
    Vector2 relativePos;
    float radius;
} Collision;

typedef struct Level{
    int start;
    int end;
} Level;
#endif