#ifndef GAME_MACROS_H
#define GAME_MACROS_H

#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 450
#define BOTTOM_MARGIN 50
#define DEFAULT_JUMP_SPEED 660.f // 660
#define MSG_Y 200
#define OBSTACLE_SPEED -300.f//-400.f       // 
#define FPS 60
#define ACTIVE_OBSTACLES 10
#define TOTAL_OBSTACLES 29      // was 28
#define OUT_OF_SCREEN -75
#define PLAYER_POSITION_X 10
#define MAX_OBSTACLE_SPEED -1000.f
#define PLAYER_WIDTH 44     // while standing   was 44
#define PLAYER_HEIGHT 47    // while standing   was 43      was latest 44
#define GAME_STARTING 0
#define GAME_RUNNING 1
#define GAME_OVER -1
#define FLYING_OBSTACLE_INDEX 28
#define PLAYER_STANDING_COLLISIONS 5
#define PLAYER_DUCKING_COLLISIONS 6
#define FLYING_OBSTACLE_COLLISIONS 6

//#define DEBUG       

#ifdef DEBUG
    #define printDebugI(a) printf("%s = %d\n", #a, a)
    #define printDebugF(a) printf("%s = %f\n", #a, a)
    #define printDebugS(a) printf("%s = %s\n", #a, a)
#else
    #define printDebugI(a)
    #define printDebugF(a)
    #define printDebugS(a)
#endif
#endif