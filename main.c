#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 450
#define BOTTOM_MARGIN 50
#define DEFAULT_JUMP_SPEED 660.f//660
#define MSG_Y 200
#define OBSTACLE_SPEED -400.f
#define FPS 60
#define ACTIVE_OBSTACLES 10
#define TOTAL_OBSTACLES 28
#define OUT_OF_SCREEN -75
#define PLAYER_POSITION_X 10
#define MAX_OBSTACLE_SPEED -1700.f
#define PLAYER_WIDTH 44
#define PLAYER_HEIGHT 43

#define GAME_STARTING 0
#define GAME_RUNNING 1
#define GAME_OVER -1

//#define DEBUG

float getMax(float a, float b){
    return a > b ? a : b;
}

float getMin(float a, float b){
    return a > b ? b : a;
}

#ifdef DEBUG
    #define printDebugI(a) printf("%s = %d\n", #a, a)
    #define printDebugF(a) printf("%s = %f\n", #a, a)
    #define printDebugS(a) printf("%s = %s\n", #a, a)
#else
    #define printDebugI(a)
    #define printDebugF(a)
    #define printDebugS(a)
#endif


// static obstacle struct whose instances we put into an array and use that array to create obstacles on the game screen
typedef struct Obstacle
{
    Rectangle rect;
    Color color;
} Obstacle;

// obstacle index and its position on game screen
typedef struct ActiveObstacle
{
    int index;
    Vector2 pos;
} ActiveObstacle;

typedef struct Animation{
    Rectangle frameRec;
    int currentFrame;
    int framesCounter;
    int framesSpeed;
} Animation;


void updateObstaclePositions(ActiveObstacle *activeObstacles, float speed, float frameTime, float *distToRight)
{
    *distToRight -= speed * frameTime;
    for (int i = 0; i < ACTIVE_OBSTACLES; i++){
        if (activeObstacles[i].pos.x > OUT_OF_SCREEN){
            activeObstacles[i].pos.x += speed * frameTime;
        }
    }
}

void clearObstaclePositions(ActiveObstacle *activeObstacles){
    for (int i = 0; i < ACTIVE_OBSTACLES; i++){
        activeObstacles[i].pos.x = OUT_OF_SCREEN;
    }
}

// [0, 17] small obstacles
// [18, 27] big obstacles
void makeANewObstacle(ActiveObstacle *activeObstacles, Obstacle *obstacles, int *latestWidth, float *distToRight)
{
    int index = GetRandomValue(0, TOTAL_OBSTACLES - 1); // use this
    //int index = GetRandomValue(18, TOTAL_OBSTACLES - 1); 
    //int index = 18;
    //int index = 3;
    //int index = 27;
    //printf("random value = %d\n", index);
    for (int i = 0; i < ACTIVE_OBSTACLES; i++)
    {
        if (activeObstacles[i].pos.x <= OUT_OF_SCREEN)
        {
            activeObstacles[i].pos.x = GAME_SCREEN_WIDTH - obstacles[index].rect.width;
            activeObstacles[i].pos.y = GAME_SCREEN_HEIGHT-BOTTOM_MARGIN - obstacles[index].rect.height;
            activeObstacles[i].index = index;
            *latestWidth = obstacles[index].rect.width;
            *distToRight = 0;
            return;
        }
    }

}
// obstacles array consists of just static obstacle blueprints, we can active any number of them using activeObstacle
void drawActiveObstacles(ActiveObstacle *activeObstacles, Obstacle *obstacles, Texture2D sprite)
{
    int index;
    for (int i = 0; i < ACTIVE_OBSTACLES; i++){
        if (activeObstacles[i].pos.x > OUT_OF_SCREEN){
            index = activeObstacles[i].index;
            // this rectangle is actually frameRec - position and frame width & height on the texture.
            // then we give the position on the game screen.
            DrawTextureRec(sprite, obstacles[index].rect, activeObstacles[i].pos, obstacles[index].color);
        }
    }
}

// game must end if player collides with an obstacle
bool checkCollisionWithObstacles(ActiveObstacle *activeObstacles, Obstacle *obstacles, Rectangle player)
{
    for(int i = 0; i<ACTIVE_OBSTACLES; i++){
        int index = activeObstacles[i].index;
        Vector2 gamePos = activeObstacles[i].pos;
        if(gamePos.x > OUT_OF_SCREEN){
            Rectangle obstacleRect = (Rectangle){gamePos.x, gamePos.y, obstacles[index].rect.width, obstacles[index].rect.height};
            if(CheckCollisionRecs(player, obstacleRect)){
                return true;
            }
        }
    }
    return false;
}

float findLandingDistance(float speed, float gravity, float jumpSpeed, float frameTime, int *latestWidth, int *rcnt)
{
    (*rcnt)++;
    //rcnt %= 2;
    //rcnt %= 3;
    (*rcnt) %= 4;

    if(speed < 0) speed = -speed;
    if(jumpSpeed < 0) jumpSpeed = -jumpSpeed;
    int t = (int)ceil(jumpSpeed / gravity);
    //t += FPS;
    //return t * speed * frameTime + (60-t)*speed*frameTime;//* 2;// + 90;   // 75 max sprite length.
    //return 45*speed*frameTime;//* 2;// + 90;   // 75 max sprite length.

    //printf("time = %f", t + (*latestWidth + *latestWidth) / (speed * frameTime));
    //printf("speed=%f, gravitiy=%f, jumpSpeed=%f, frameTime=%f, latestWidth=%d\n", speed, gravity, jumpSpeed, frameTime, *latestWidth);

    //return (t + *latestWidth / (speed * frameTime)) * speed * frameTime;
    /* return rcnt % 2 ? 
    t * speed * frameTime + *latestWidth + *latestWidth/2 
    : GetRandomValue(43, 70) * speed * frameTime + *latestWidth + *latestWidth/2; */
    /* return rcnt % 2 ? 
    (t + (*latestWidth) / (speed * frameTime)) * speed * frameTime  
    : (GetRandomValue(43, 60) + (*latestWidth) / (speed * frameTime))* speed * frameTime; */

    /* if(rcnt % 3 == 1)
        return (t + (*latestWidth) / (speed * frameTime) + 60*0.2) * speed * frameTime;
    else if(rcnt % 3 == 2)
        return (GetRandomValue(45, 55) + (*latestWidth) / (speed * frameTime)) * speed * frameTime;
    else 
        return (GetRandomValue(65, 75) + (*latestWidth) / (speed * frameTime)) * speed * frameTime; */
    if(*rcnt == 1)
        return (t + (*latestWidth) / (speed * frameTime) + 60*0.2) * speed * frameTime;
    else if(*rcnt== 2)
        return (GetRandomValue(40, 50) + (*latestWidth) / (speed * frameTime)) * speed * frameTime;
    else if(*rcnt== 3)
        return (GetRandomValue(50, 60) + (*latestWidth) / (speed * frameTime)) * speed * frameTime;
    else
        return (GetRandomValue(45, 55) + (*latestWidth) / (speed * frameTime)) * speed * frameTime;
}

/* void runAnimation(Animation *animation, void (*animationFunc)(Animation *) ){
    animationFunc(animation);
} */

void playerAnimate(Animation *animation, int *collisionDetected, int *frameRecX){
    animation->framesCounter++;
    if (animation->framesCounter >= (FPS / animation->framesSpeed))
    { 
        animation->framesCounter = 0;
        if (animation->currentFrame == 1)
            animation->currentFrame++; // skip frame 1

        if (!(*collisionDetected))
        { // if on air, fix player to some frame
            animation->currentFrame = 0;
        }

        animation->frameRec.x = *frameRecX + (float)animation->currentFrame * PLAYER_WIDTH;

        animation->currentFrame++;
        if (animation->currentFrame > 3)
            animation->currentFrame = 0;
    }
}

void roadAnimate(Animation *animation, float *obstacleSpeed, float *frameTime){
    animation->frameRec.x -= (*obstacleSpeed) * (*frameTime); // obstacle speed negative so this must be positive, going to right
    if (animation->frameRec.x > 1200.f)
    {
        animation->frameRec.x -= 1200;
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = GAME_SCREEN_WIDTH;
    const int screenHeight = GAME_SCREEN_HEIGHT;
    int lastestWidth = 0;
    int rcnt = 0;
    float distToRight = 0;
    int score = 0;

    InitWindow(screenWidth, screenHeight, "TREX JUMP GAME");

    SetTargetFPS(FPS); // Set our game to run at 60 frames-per-second

    int bigFontSize = 30;
    int smallFontSize = 20;
    int wdGameOver = MeasureText("GAME OVER", bigFontSize);
    int wdStart = MeasureText("START (press SPACE)", bigFontSize);
    int wdScore = MeasureText("Score: 100", smallFontSize);

    // new code
    Texture2D trexGameSprite = LoadTexture("./resources/sprites.png");
    
    // for player
    Rectangle player = {PLAYER_POSITION_X, 0, PLAYER_WIDTH, PLAYER_HEIGHT};
    int frameRecX = 673;
    int frameRecY = 4;
    int frameWidth = 44;
    int frameHeight = 43;
    // number of spritesheet frames shown by second --> framesSpeed
    Animation playerAnimation = {
        {frameRecX, frameRecY, frameWidth, frameHeight},
        .currentFrame = 0, .framesCounter = 0, .framesSpeed=15};


    // for the road
    Vector2 roadPosition = {0, 390}; // on game screen, always the same
    int frameRecXOnSprite = 0;       // -2
    int frameRecYOnSprite = 54;
    int frameRecWidthOnSprite = GAME_SCREEN_WIDTH; // total of 1201 px, 800
    int frameRecHeightOnSprite = 14;
    Animation roadAnimation = {{
        frameRecXOnSprite,
        frameRecYOnSprite,
        frameRecWidthOnSprite,
        frameRecHeightOnSprite
    }, .currentFrame = 0, .framesCounter = 0, .framesSpeed = 15};

    bool start = true;
    bool highscoreCheck = false;
    int highestScore = -1;

    // active obstacles
    ActiveObstacle activeObstacles[ACTIVE_OBSTACLES];
    for (int i = 0; i < ACTIVE_OBSTACLES; i++)
    {
        activeObstacles[i].pos.x = OUT_OF_SCREEN;
    }

    // obstacle small generator
    int obstacleSmallFrameRecXOnSprite = 226; // -2
    int obstacleSmallFrameRecYOnSprite = 2;
    int obstacleSmallFrameRecWidthOnSprite = 17; // total of 1201 px, 800
    int obstacleSmallFrameRecHeightOnSprite = 34;

    Obstacle *obstacles = malloc(80 * sizeof(Obstacle));
    int ix = 0;
    for (int i = 0; i < 6; i++)
    {
        for(int j = 0; j<4 && i+j<6; j++)
        {
            obstacles[ix] = (Obstacle){
                {obstacleSmallFrameRecXOnSprite + obstacleSmallFrameRecWidthOnSprite * i,
                 obstacleSmallFrameRecYOnSprite,
                 obstacleSmallFrameRecWidthOnSprite + obstacleSmallFrameRecWidthOnSprite * j,
                 obstacleSmallFrameRecHeightOnSprite},
                WHITE};
            ix++;
        }
    }

    // obstacle big generator
    float obstacleBigFrameRecXOnSprite = 330;
    float obstacleBigFrameRecYOnSprite = 2;
    float obstacleBigFrameRecWidthOnSprite = 25;
    float obstacleBigFrameRecHeightOnSprite = 46;
    for (int i = 0; i < 4; i++)
    {
        for(int j = 0; j<3 && i+j<4; j++)
        {
            obstacles[ix] = (Obstacle){
                {obstacleBigFrameRecXOnSprite + obstacleBigFrameRecWidthOnSprite * i,
                 obstacleBigFrameRecYOnSprite,
                 obstacleBigFrameRecWidthOnSprite + obstacleBigFrameRecWidthOnSprite * j,
                 obstacleBigFrameRecHeightOnSprite},
                WHITE};
            ix++;
        }
    }
    obstacles[ix] = (Obstacle){ {405, 2, 75, 49}, WHITE};
    ix++;
    
    float obstacleSpeed = OBSTACLE_SPEED;           // the main speed.  was -400 -1100
    
    // jump info
    float jumpPower = DEFAULT_JUMP_SPEED;
    float jumpSpeed = 0.f;

    // recent random val
    int recentVal = GetRandomValue(200, 500);   // was 90 - 500

    // for collision
    int collisionDetected = 0;

    // gravity
    float gravity = 32.f;

    // main active rectangle below the road
    Rectangle mainCollisionControlRectangle = {0, 400, 1000, 200};

    // game state
    int gameState = GAME_STARTING;
    float frameTime = 0.015f;

    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        if(gameState == GAME_STARTING){
            if(IsKeyDown(KEY_SPACE)){
                gameState = GAME_RUNNING;
            }
        }
        if(gameState == GAME_RUNNING){
            score++;
            if(score/10 % 25 == 0){
                if(obstacleSpeed > MAX_OBSTACLE_SPEED)
                    obstacleSpeed -= 5; 
            }

            // this is for player
            playerAnimate(&playerAnimation, &collisionDetected, &frameRecX);
            // player sprite ends here.

            // road sprite starts
            roadAnimate(&roadAnimation, &obstacleSpeed, &frameTime);
            // road sprite ends

            updateObstaclePositions(activeObstacles, obstacleSpeed, frameTime, &distToRight);

            if(start){
                start = false;
                makeANewObstacle(activeObstacles, obstacles, &lastestWidth, &distToRight);
            }else{
                if((int)distToRight > recentVal){
                    recentVal = findLandingDistance(obstacleSpeed, gravity, jumpPower, frameTime, &lastestWidth, &rcnt);
                    printDebugI(recentVal);
                    makeANewObstacle(activeObstacles, obstacles, &lastestWidth, &distToRight);
                }
            }

            if(checkCollisionWithObstacles(activeObstacles, obstacles, player)){
                gameState = GAME_OVER;
            }

            if (collisionDetected && IsKeyDown(KEY_SPACE))
            { // on stepping now
                jumpSpeed = -jumpPower;
                player.y += jumpSpeed * frameTime; // minus, rocket it up

                printDebugF(jumpSpeed*frameTime);

                jumpSpeed += gravity;
            }

            if (!collisionDetected)
            { // on flying now
                player.y += jumpSpeed * frameTime;
                jumpSpeed += gravity;
            }

            collisionDetected = 0;
            if (CheckCollisionRecs(player, mainCollisionControlRectangle))
            {
                printDebugS("some collision.\n");
                collisionDetected = 1;
                jumpSpeed = 0.f;
                player.y = mainCollisionControlRectangle.y - 42; // bottom part must be equal to the collision point but this is complicated for now
            }
        }

        if(gameState == GAME_OVER){ 
            if(IsKeyReleased(KEY_SPACE)){
                score = 0;
                roadAnimation.frameRec.x = 0;
                player.x = PLAYER_POSITION_X;
                player.y = 0;
                playerAnimation.currentFrame = 0;
                playerAnimation.framesCounter = 0;
                clearObstaclePositions(activeObstacles);
                obstacleSpeed = OBSTACLE_SPEED;
                gameState = GAME_RUNNING;
                highscoreCheck = false;
            }
        }


        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground((Color){255, 255, 255, 255});

        // text
        DrawText(TextFormat("Score: %d", score/10), screenWidth/2 - wdScore/2, 20, smallFontSize, DARKGRAY);

        // road
        DrawTextureRec(trexGameSprite, roadAnimation.frameRec, roadPosition, WHITE);
        
        drawActiveObstacles(activeObstacles, obstacles, trexGameSprite); // new

        // player
        DrawTextureRec(trexGameSprite, playerAnimation.frameRec, (Vector2){player.x, player.y}, WHITE);

        if(gameState == GAME_STARTING){
            DrawText("START (press SPACE)", screenWidth/2 - wdStart/2, MSG_Y, bigFontSize, GRAY);
        }
        if(gameState == GAME_OVER){
            
            DrawText("GAME OVER", screenWidth/2 - wdGameOver/2, MSG_Y, bigFontSize, BLACK);
            char str[100];
            sprintf(str, "Your Score: %d", score/10);
            int wdScoreGO = MeasureText(str, bigFontSize);
            int msgMargin = 40;
            DrawText(TextFormat("Your Score: %d", score / 10), screenWidth / 2 - wdScoreGO / 2, MSG_Y + msgMargin, bigFontSize, BLACK);

            // highscore part!
            if(!highscoreCheck){
                highscoreCheck = true;
                FILE *stream = fopen("./resources/game.dat", "r+");
                if(stream != NULL){
                    
                    fscanf(stream, "HighestScore=%d", &highestScore);
                    printf("hs = %d", highestScore);
                    
                    if(score/10 > highestScore){
                        fseek(stream, 0, SEEK_SET);
                        fprintf(stream, "HighestScore=%d", score/10);
                        highestScore = score/10;
                    }
                    fclose(stream);
                }else{
                    printf("game.dat not found.\n");
                }
            }
            int wdHighscore = MeasureText("Highest Score: 0", bigFontSize);
            DrawText(TextFormat("Highest Score: %d", highestScore), screenWidth / 2 - wdHighscore / 2, MSG_Y + msgMargin + msgMargin , bigFontSize, BLACK);

            int wdRestart = MeasureText("PRESS SPACE TO RESTART", smallFontSize);
            DrawText("PRESS SPACE TO RESTART", screenWidth / 2 - wdRestart / 2, MSG_Y + msgMargin + msgMargin+ msgMargin, smallFontSize, GRAY);
        }
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    free(obstacles);
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}