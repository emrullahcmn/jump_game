#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "game_utilities/game_macros.h"
#include "game_utilities/game_structs.h"
#include "game_utilities/game_functions.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

int main(void){
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = GAME_SCREEN_WIDTH;
    const int screenHeight = GAME_SCREEN_HEIGHT;
    int lastestWidth = 0;
    int lastHeight = 0;
    int rcnt = 0;
    float distToRight = 0;        
    int score = 0;
    bool ducking = false;       // NEW

    // Levels
    Level levels[] = {
        {70, 85},
        {5, 25},
        {45, 75},
        {5, 30},
        {35, 55}
    };
    int levelCounter = 0;
    bool levelIsHard = false;  

    // for player
    Rectangle player = {PLAYER_POSITION_X, 0, PLAYER_WIDTH, PLAYER_HEIGHT};

    Rectangle standingUpFrameRec = {675, 2, 44, 47};    // 4 player   x, y, w, h was {673, 4, 44, 43}   latest {675, 3, 44, 44};
    Rectangle duckingFrameRec = {937, 19, 59, 30};      // 4 player
    // number of spritesheet frames shown by second --> framesSpeed
    Animation playerAnimation = {
        .frameRec = {0, 0, 0, 0},          // default standingUpFrame.
        .currentFrame = 0,
        .framesCounter = 0,
        .framesSpeed = 15
    };
    setPlayerStandingUp(&playerAnimation, &player, &standingUpFrameRec);    // default standing up

    // nuclear weapon's coordinates :D
    Collision playerStandingCollisions[PLAYER_STANDING_COLLISIONS] = {
        {{706.94-standingUpFrameRec.x, 13.38-standingUpFrameRec.y}, 9.5},
        {{696.37-standingUpFrameRec.x, 28.13-standingUpFrameRec.y}, 8},
        {{693.56-standingUpFrameRec.x, 40-standingUpFrameRec.y}, 7},
        {{680.28-standingUpFrameRec.x, 27.59-standingUpFrameRec.y}, 3.5}, 
        {{685.77-standingUpFrameRec.x, 32.27-standingUpFrameRec.y}, 5}, 
        {{678.06-standingUpFrameRec.x, 22.28-standingUpFrameRec.y}, 2} 
    };
    Collision playerDuckingCollisions[PLAYER_DUCKING_COLLISIONS] = {
        {{984.44-duckingFrameRec.x, 29.56-duckingFrameRec.y}, 9},
        {{966.12-duckingFrameRec.x, 31.50-duckingFrameRec.y}, 9.5},
        {{950.81-duckingFrameRec.x, 30.50-duckingFrameRec.y}, 6},
        {{969.87-duckingFrameRec.x, 40.44-duckingFrameRec.y}, 3.5},
        {{956.31-duckingFrameRec.x, 41.63-duckingFrameRec.y}, 5.5},
        {{955.92-duckingFrameRec.x, 25.11-duckingFrameRec.y}, 3},
        {{942.31-duckingFrameRec.x, 25.06-duckingFrameRec.y}, 4}
    };

    // for the road
    Vector2 roadPosition = {0, 390}; // on game screen, always the same
    int frameRecXOnSprite = 0;       // -2
    int frameRecYOnSprite = 54;
    int frameRecWidthOnSprite = GAME_SCREEN_WIDTH; // total of 1201 px, 800
    int frameRecHeightOnSprite = 14;
    Animation roadAnimation = {
        .frameRec={frameRecXOnSprite,  frameRecYOnSprite, frameRecWidthOnSprite, frameRecHeightOnSprite},
        .currentFrame = 0,
        .framesCounter = 0,
        .framesSpeed = 15};

    bool start = true;
    bool highscoreCheck = false;
    int highestScore = -1;

    // msg margin
    int msgMargin = 40;

    // active obstacles
    ActiveObstacle activeObstacles[ACTIVE_OBSTACLES];
    for (int i = 0; i < ACTIVE_OBSTACLES; i++){
        activeObstacles[i].pos.x = OUT_OF_SCREEN;
    }

    // head new empty position
    // tail oldest active obstacle if present
    int head = 0;
    int tail = 0;

    // obstacle small generator
    int obstacleSmallFrameRecXOnSprite = 226; // -2
    int obstacleSmallFrameRecYOnSprite = 2;
    int obstacleSmallFrameRecWidthOnSprite = 17; // total of 1201 px, 800
    int obstacleSmallFrameRecHeightOnSprite = 34;

    Obstacle *obstacles = malloc(80 * sizeof(Obstacle));
    int ix = 0;
    for (int i = 0; i < 6; i++){
        for (int j = 0; j < 4 && i + j < 6; j++){
            obstacles[ix] = (Obstacle){
                {   obstacleSmallFrameRecXOnSprite + obstacleSmallFrameRecWidthOnSprite * i,
                    obstacleSmallFrameRecYOnSprite,
                    obstacleSmallFrameRecWidthOnSprite + obstacleSmallFrameRecWidthOnSprite * j,
                    obstacleSmallFrameRecHeightOnSprite},
                WHITE}; // was WHITE
            ix++;
        }
    }

    // obstacle big generator 
    float obstacleBigFrameRecXOnSprite = 330;
    float obstacleBigFrameRecYOnSprite = 2;
    float obstacleBigFrameRecWidthOnSprite = 25;
    float obstacleBigFrameRecHeightOnSprite = 46;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 3 && i + j < 4; j++){
            obstacles[ix] = (Obstacle){
                {   obstacleBigFrameRecXOnSprite + obstacleBigFrameRecWidthOnSprite * i,
                    obstacleBigFrameRecYOnSprite,
                    obstacleBigFrameRecWidthOnSprite + obstacleBigFrameRecWidthOnSprite * j,
                    obstacleBigFrameRecHeightOnSprite},
                WHITE};   //was WHITE
            ix++;
        }
    }
    obstacles[ix] = (Obstacle){{405, 2, 75, 49}, WHITE};  // was WHITE
    ix++;
    obstacles[ix] = (Obstacle){{132, 8, 46, 40}, WHITE};  // was WHITE
    int animatedObstacleIx = ix;    
    Animation animatedObstacle = {
        .frameRec = {132, 8, 46, 40},
        .frameRecStatic = &obstacles[animatedObstacleIx].rect,
        .currentFrame = 0,
        .framesCounter = 0,
        .framesSpeed = 5
    };
    
    Collision flyingObstacleFirstCollisions[FLYING_OBSTACLE_COLLISIONS_FIRST] = {
        {{137.06-animatedObstacle.frameRec.x, 18.19-animatedObstacle.frameRec.y}, 2.5},
        {{143.38-animatedObstacle.frameRec.x, 15.06-animatedObstacle.frameRec.y}, 4.5},
        {{148.95-animatedObstacle.frameRec.x, 20-animatedObstacle.frameRec.y}, 3},
        {{157.05-animatedObstacle.frameRec.x, 25.83-animatedObstacle.frameRec.y}, 7.5},
        {{151.67-animatedObstacle.frameRec.x, 36.14-animatedObstacle.frameRec.y}, 3.25},
        {{169.70-animatedObstacle.frameRec.x, 25.69-animatedObstacle.frameRec.y}, 5.5}
    };
    Collision flyingObstacleSecondCollisions[FLYING_OBSTACLE_COLLISIONS_SECOND] = {
        {{182.74-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 18.53-animatedObstacle.frameRec.y}, 2.5},
        {{189.30-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 15.16-animatedObstacle.frameRec.y}, 4.5},
        {{196.69-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 8.24-animatedObstacle.frameRec.y}, 4},
        {{195   -animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 19.97-animatedObstacle.frameRec.y}, 3},
        {{202.66-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 20.89-animatedObstacle.frameRec.y}, 8.5},
        {{210.28-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 27.43-animatedObstacle.frameRec.y}, 3},
       {{216.97-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 25.31-animatedObstacle.frameRec.y}, 4.5}
    };

    //  

    float obstacleSpeed = OBSTACLE_SPEED; // the main speed.  was -400 -1100 

    // jump info
    float jumpPower = DEFAULT_JUMP_SPEED;
    float jumpSpeed = 0.f;

    // recent random val
    int recentVal = GetRandomValue(200, 500); // was 90 - 500

    // for collision
    int collisionDetected = 0;

    // gravity
    float gravity = 32;

    // difficulty transition
    int transition = 0;

    // main active rectangle below the road
    Rectangle mainCollisionControlRectangle = {0, 400, 1000, 200};

    // game state
    int gameState = GAME_STARTING;
    //float frameTime = 0.015f;
    int waitDurationCounter = 0;
    int totalWaitDuration = FPS * 0.4;

    SetTraceLogLevel(0); // release
    InitWindow(screenWidth, screenHeight, "TREX JUMP GAME");
    SetTargetFPS(FPS); // Set our game to run at 60 frames-per-second
    //ToggleFullscreen();   // for full screen.

    int bigFontSize = 30;
    int smallFontSize = 20;
    int wdGameOver = MeasureText("GAME OVER", bigFontSize);
    int wdStart = MeasureText("START (press SPACE) ", bigFontSize);
    int wdStartInfoJumping = MeasureText(">>SPACE<< for jumping", bigFontSize);
    int wdStartInfoDucking = MeasureText(">>S<< for ducking", bigFontSize);
    int wdScore = MeasureText("Score: 100", smallFontSize);
    char str[100]; // for score text

    // texture
    Texture2D trexGameSprite = LoadTexture("./resources/sprites.png");
    
    //--------------------------------------------------------------------------------------
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        float frameTime = GetFrameTime();

        switch (gameState){
            case GAME_STARTING:
                /* code */
                if (IsKeyDown(KEY_SPACE)){
                    gameState = GAME_RUNNING;
                }
                break;

            case GAME_RUNNING:
                /* code */
                // main rectangle
                collisionDetected = 0;
                if (CheckCollisionRecs(player, mainCollisionControlRectangle)){
                    collisionDetected = 1;
                    jumpSpeed = 0.f;
                    if(!ducking) 
                        player.y = mainCollisionControlRectangle.y - 42;
                    else
                        player.y = mainCollisionControlRectangle.y - 25;
                }
                // with obstacles
                if (checkCollisionWithObstacles(activeObstacles, obstacles, player,
                ducking, playerStandingCollisions, playerDuckingCollisions, flyingObstacleFirstCollisions,
            flyingObstacleSecondCollisions, &animatedObstacle, head, tail)){  // many parameters here
                    gameState = GAME_OVER;
                    waitDurationCounter = 0;
                    break;
                }

                score++;
                if (score % 700 == 0){
                    transition = 1;
                }

                if(!ducking){
                    if(collisionDetected && (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))){
                        ducking = true;
                        setPlayerDucking(&playerAnimation, &player, &duckingFrameRec);
                    }
                }else{
                    if(IsKeyReleased(KEY_DOWN) || IsKeyReleased(KEY_S)){
                        ducking = false;
                        setPlayerStandingUp(&playerAnimation, &player, &standingUpFrameRec);
                    }
                }

                // player animation
                playerAnimate(&playerAnimation, collisionDetected, &player, ducking);
                // obstacle animation
                obstacleAnimate(&animatedObstacle);
                // road animation
                roadAnimate(&roadAnimation, obstacleSpeed, frameTime);
                // update obstacle positions
                updateObstaclePositions(activeObstacles, obstacleSpeed, frameTime, &distToRight, &head, &tail); 

                if (transition || distToRight >= recentVal){
                    if(transition){
                        if (head == tail){  // so that all obstacles leave the screen.
                            transition = 0;
                            recentVal = 100;  // nw
                            distToRight = 0;    // this is independent from obstacles
                            if(obstacleSpeed > MAX_OBSTACLE_SPEED){
                                obstacleSpeed -= 100;
                            }
                            if(obstacleSpeed < -600){
                                levelCounter = 0;
                                levelIsHard = true;
                            }
                        }
                    }else{
                        Level *level = &levels[levelCounter];
                        levelCounter++;
                        levelCounter %= 5;
                        recentVal = ceilf(findLandingDistance(obstacleSpeed, gravity, jumpPower, frameTime, lastestWidth, lastHeight, level, levelIsHard));
                        makeANewObstacle(activeObstacles, obstacles, &lastestWidth, &lastHeight,  &distToRight, animatedObstacleIx, &head, &tail, levelIsHard);
                    }
                }

                if(collisionDetected){
                    if(IsKeyDown(KEY_SPACE)){
                        jumpSpeed = -jumpPower;
                        player.y += jumpSpeed * frameTime; // minus, rocket it up
                        printDebugF(jumpSpeed * frameTime);
                        jumpSpeed += gravity;
                        if(ducking){
                            ducking = false;
                            setPlayerStandingUp(&playerAnimation, &player, &standingUpFrameRec);
                        }
                    }
                }else{
                    player.y += jumpSpeed * frameTime;
                    jumpSpeed += gravity;
                }
                break;

            case GAME_OVER:
                /* code */
                if(waitDurationCounter < totalWaitDuration)
                    waitDurationCounter++;

                if (IsKeyDown(KEY_SPACE) && waitDurationCounter >= totalWaitDuration){
                    jumpSpeed = 0;
                    start = true;
                    lastestWidth = 0;
                    distToRight = 0;
                    recentVal = GetRandomValue(200, 500);
                    head = 0;
                    tail = 0;
                    setPlayerStandingUp(&playerAnimation, &player, &standingUpFrameRec);
                    ducking = false;
                    score = 0;
                    roadAnimation.frameRec.x = 0;
                    collisionDetected = 0;
                    player.x = PLAYER_POSITION_X;
                    player.y = 0;
                    playerAnimation.currentFrame = 0;
                    playerAnimation.framesCounter = 0;
                    animatedObstacle.currentFrame = 0;
                    animatedObstacle.framesCounter = 0;
                    obstacleSpeed = OBSTACLE_SPEED;
                    transition = 0;
                    gameState = GAME_RUNNING;
                    highscoreCheck = false;
                    levelCounter = 0;
                    levelIsHard = false;  
                }
                break;
            default:
                break;
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground((Color){255, 255, 255, 255});

        // text
        DrawText(TextFormat("Score: %d", score / 10), screenWidth / 2 - wdScore / 2, 20, smallFontSize, DARKGRAY);
        // road
        DrawTextureRec(trexGameSprite, roadAnimation.frameRec, roadPosition, WHITE);
        // obstacles
        drawActiveObstacles(activeObstacles, obstacles, trexGameSprite, animatedObstacleIx, &animatedObstacle, gameState, head, tail); // new
        // draw player
        DrawTextureRec(trexGameSprite, playerAnimation.frameRec, (Vector2){player.x, player.y}, WHITE);  // GREEN
        
        // draw collisions -- debugging purposes
        #ifdef DEBUG
        drawCollisions(activeObstacles, obstacles, player, ducking, 
            playerStandingCollisions, playerDuckingCollisions, flyingObstacleFirstCollisions, 
            flyingObstacleSecondCollisions, &animatedObstacle, &standingUpFrameRec, head, tail);
        #endif
        
        switch (gameState)
        {
        case GAME_STARTING:
            /* code */
            DrawText(">>SPACE<< for jumping", screenWidth / 2 - wdStartInfoJumping / 2, MSG_Y, bigFontSize, GREEN);
            DrawText(">>S<< for ducking", screenWidth / 2 - wdStartInfoDucking / 2, MSG_Y + msgMargin, bigFontSize, ORANGE);
            DrawText("START (press SPACE)", screenWidth / 2 - wdStart / 2, MSG_Y + msgMargin + msgMargin, bigFontSize, GRAY);
            break;
        case GAME_RUNNING:
            /* code */
            break;
        case GAME_OVER:
            /* code */
            DrawText("GAME OVER", screenWidth / 2 - wdGameOver / 2, MSG_Y, bigFontSize, BLACK);
            sprintf(str, "Your Score: %d", score / 10);
            int wdScoreGO = MeasureText(str, bigFontSize);
            DrawText(TextFormat("Your Score: %d", score / 10), screenWidth / 2 - wdScoreGO / 2, MSG_Y + msgMargin, bigFontSize, BLACK);

            // highscore part!
            if (!highscoreCheck){
                highestScore = -1;
                highscoreCheck = true;
                FILE *stream = fopen("./resources/game.dat", "r+");
                if (stream != NULL){

                    fscanf(stream, "HighestScore=%d", &highestScore);
                    printDebugI(highestScore);

                    if (score / 10 > highestScore){
                        fseek(stream, 0, SEEK_SET);
                        fprintf(stream, "HighestScore=%d", score / 10);
                        highestScore = score / 10;
                    }
                    fclose(stream);
                }else{
                    printDebugS("game.dat not found.\n");
                }
            }
            int wdHighscore = MeasureText("Highest Score: 0", bigFontSize);
            if(highestScore == -1){
                DrawText(TextFormat("Highest Score: %s", "ERROR"), screenWidth / 2 - wdHighscore / 2, MSG_Y + msgMargin + msgMargin, bigFontSize, BLACK);
            }else{
                DrawText(TextFormat("Highest Score: %d", highestScore), screenWidth / 2 - wdHighscore / 2, MSG_Y + msgMargin + msgMargin, bigFontSize, BLACK);
            }

            int wdRestart = MeasureText("PRESS SPACE TO RESTART", smallFontSize);
            DrawText("PRESS SPACE TO RESTART", screenWidth / 2 - wdRestart / 2, MSG_Y + msgMargin + msgMargin + msgMargin, smallFontSize, GRAY);
            break;

        default:
            break;
        }

        DrawFPS(GAME_SCREEN_WIDTH-90, 0);

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
