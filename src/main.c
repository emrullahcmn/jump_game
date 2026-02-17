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
    Collision playerStandingCollisions[5] = {
        {{706.94-standingUpFrameRec.x, 13.38-standingUpFrameRec.y}, 9.5},
        {{696.56-standingUpFrameRec.x, 28.31-standingUpFrameRec.y}, 8.5},
        {{693.56-standingUpFrameRec.x, 40-standingUpFrameRec.y}, 7},
        {{681.66-standingUpFrameRec.x, 31.09-standingUpFrameRec.y}, 5},
        {{678.13-standingUpFrameRec.x, 22.91-standingUpFrameRec.y}, 3.5}
    };
    Collision playerDuckingCollisions[6] = {
        {{983.87-duckingFrameRec.x, 29.81-duckingFrameRec.y}, 9.5},
        {{965.44-duckingFrameRec.x, 30.62-duckingFrameRec.y}, 9.5},
        {{952.69-duckingFrameRec.x, 29.56-duckingFrameRec.y}, 7.5},
        {{969.87-duckingFrameRec.x, 40.44-duckingFrameRec.y}, 3.5},
        {{956.31-duckingFrameRec.x, 41.63-duckingFrameRec.y}, 5.5},
        {{942.31-duckingFrameRec.x, 25.06-duckingFrameRec.y}, 5}
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
    
    Collision flyingObstacleFirstCollisions[6] = {
        {{137.94-animatedObstacle.frameRec.x, 18.38-animatedObstacle.frameRec.y}, 4},
        {{144.06-animatedObstacle.frameRec.x, 15.06-animatedObstacle.frameRec.y}, 4.5},
        {{148.95-animatedObstacle.frameRec.x, 20-animatedObstacle.frameRec.y}, 3},
        {{156.48-animatedObstacle.frameRec.x, 25.64-animatedObstacle.frameRec.y}, 8},
        {{151.67-animatedObstacle.frameRec.x, 36.14-animatedObstacle.frameRec.y}, 3.5},
        {{169.70-animatedObstacle.frameRec.x, 25.19-animatedObstacle.frameRec.y}, 6},
    };
    Collision flyingObstacleSecondCollisions[6] = {
        {{183.99-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 18.34-animatedObstacle.frameRec.y}, 4},
        {{190.11-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 15.03-animatedObstacle.frameRec.y}, 4.5},
        {{196.69-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 8.36-animatedObstacle.frameRec.y}, 4},
        {{195-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 19.97-animatedObstacle.frameRec.y}, 3},
        {{203.10-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 20.83-animatedObstacle.frameRec.y}, 9},
        {{215.66-animatedObstacle.frameRec.x-animatedObstacle.frameRec.width, 25.5-animatedObstacle.frameRec.y}, 6},
    };

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

    // new code
    Texture2D trexGameSprite = LoadTexture("./resources/sprites.png");
    SetTextureFilter(trexGameSprite, TEXTURE_FILTER_POINT);
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
                score++;
                if (score % 700 == 0){
                    transition = 1;
                }

                if(!ducking && collisionDetected && (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))){
                    ducking = true;
                    setPlayerDucking(&playerAnimation, &player, &duckingFrameRec);
                }

                if(ducking && (IsKeyReleased(KEY_DOWN) || IsKeyReleased(KEY_S))){
                    ducking = false;
                    setPlayerStandingUp(&playerAnimation, &player, &standingUpFrameRec);
                }


                // this is for player
                playerAnimate(&playerAnimation, collisionDetected, &player, ducking);
                // player sprite ends here.

                // road sprite starts
                roadAnimate(&roadAnimation, obstacleSpeed, frameTime);
                // road sprite ends

                updateObstaclePositions(activeObstacles, obstacleSpeed, frameTime, &distToRight, &head, &tail); 

                if (transition || distToRight >= recentVal){
                    if(transition){
                        recentVal = GAME_SCREEN_WIDTH*1.5;
                        distToRight = 0;
                        if (head == tail){  // so that all obstacles leave the screen.
                            transition = 0;
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
                
                if (checkCollisionWithObstacles(activeObstacles, obstacles, player,
                ducking, playerStandingCollisions, playerDuckingCollisions, flyingObstacleFirstCollisions,
            flyingObstacleSecondCollisions, &animatedObstacle, head, tail)){  // many parameters here
                    gameState = GAME_OVER;
                    waitDurationCounter = 0;
                }

                if (collisionDetected && IsKeyDown(KEY_SPACE)){ // on stepping now
                    jumpSpeed = -jumpPower;
                    player.y += jumpSpeed * frameTime; // minus, rocket it up
                    printDebugF(jumpSpeed * frameTime);
                    jumpSpeed += gravity;
                    if(ducking){
                        ducking = false;
                        setPlayerStandingUp(&playerAnimation, &player, &standingUpFrameRec);
                    }
                }

                if (!collisionDetected){ // on flying now
                    player.y += jumpSpeed * frameTime;
                    jumpSpeed += gravity;
                }

                collisionDetected = 0;
                if (CheckCollisionRecs(player, mainCollisionControlRectangle)){
                    collisionDetected = 1;
                    jumpSpeed = 0.f;
                    if(!ducking) 
                        player.y = mainCollisionControlRectangle.y - 42;
                    else
                        player.y = mainCollisionControlRectangle.y - 25;
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
                    clearObstaclePositions(activeObstacles, head, tail);
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

        // draw player
        DrawTextureRec(trexGameSprite, playerAnimation.frameRec, (Vector2){player.x, player.y}, WHITE);  // GREEN

        // obstacles
        drawActiveObstacles(activeObstacles, obstacles, trexGameSprite, animatedObstacleIx, &animatedObstacle, gameState, head, tail); // new
        
        

        // draw collisions
        //#ifdef DEBUG
        /* drawCollisions(activeObstacles, obstacles, player, ducking, 
            playerStandingCollisions, playerDuckingCollisions, flyingObstacleFirstCollisions, 
            flyingObstacleSecondCollisions, &animatedObstacle, &standingUpFrameRec); */
        //#endif
        int msgMargin = 40;
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
