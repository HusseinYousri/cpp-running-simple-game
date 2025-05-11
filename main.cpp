#include "raylib.h"

struct Character
{
    Texture2D texture;
    Rectangle source;
    Vector2 pos;
    
    // Animation Data
    int frame;
    int maxFrame;
    float RunningTime;
    float UpdateTime;
};

// Functions
bool isOnGround(const Character& newScarfy, const int& window_height)
{
    return newScarfy.pos.y >= window_height - newScarfy.source.height;
}

void UpdateAnimation(Character& characterName, float deltaTime)
{
    characterName.RunningTime += deltaTime;
    if (characterName.RunningTime >= characterName.UpdateTime)
    {
        characterName.RunningTime = 0;
        characterName.source.x = characterName.frame * characterName.source.width;
        characterName.frame++;
        if (characterName.frame >= characterName.maxFrame) characterName.frame = 0;
    }
}

int main()
{
    // Window Creation
    int windowDimensions[2];
    windowDimensions[0] = 512;
    windowDimensions[1] = 380;

    InitWindow(windowDimensions[0], windowDimensions[1], "runner-improved");
    SetTargetFPS(60);

    // Gravity
    const int gravity{1000};

    // Air Check
    bool isInAir{false};
    
    // Jump
    const int jump_vel{-600};

    // far background
    Texture2D far_background = LoadTexture("textures/far-buildings.png");
    float bgX = 0.0f;
    const int bgVel{-100};
    const float bgScale{2.0f};

    // back background
    Texture2D back_background = LoadTexture("textures/back-buildings.png");
    float b_bgX{0.0f};

    // fore background
    Texture2D fore_background = LoadTexture("textures/foreground.png");
    float f_bgX{0.0f};

    Character Nebulas[2];
    int NebulaSize = sizeof(Nebulas) / sizeof(Nebulas[0]);
    const int nebula_vel{-200};

    // Game variables
    bool collision = false;
    bool gameOver = false;

    for (int i = 0; i < NebulaSize; i++)
    {
        // Nebula Data
        Nebulas[i].texture = LoadTexture("textures/12_nebula_spritesheet.png");
        Nebulas[i].source.width = Nebulas[i].texture.width/8;
        Nebulas[i].source.height = Nebulas[i].texture.height/8;
        Nebulas[i].source.x = 0;
        Nebulas[i].source.y = 0;
        Nebulas[i].pos.y = windowDimensions[1] - Nebulas[i].source.height;
        Nebulas[i].pos.x = ((i + 1) * windowDimensions[0]) + 300;

        // Nebula Animation
        Nebulas[i].frame = 0;
        Nebulas[i].maxFrame = 8;
        Nebulas[i].RunningTime = 0.0f;
        Nebulas[i].UpdateTime = 1.0f / 15.0f;
    }
    
    // Scarfy Data
    Character Scarfy;
    Scarfy.texture = LoadTexture("textures/scarfy.png");
    Scarfy.source.width = Scarfy.texture.width/6;
    Scarfy.source.height = Scarfy.texture.height;
    Scarfy.source.x = 0;
    Scarfy.source.y = 0;
    Scarfy.pos.x = windowDimensions[0] / 2 - Scarfy.source.width / 2;
    Scarfy.pos.y = windowDimensions[1] - Scarfy.source.height;

    // Scarfy Animation
    Scarfy.frame = 0;
    Scarfy.maxFrame = 6;
    Scarfy.RunningTime = 0.0f;
    Scarfy.UpdateTime = 1.0f / 12.0f;

    int scarfy_vel{0};

    while (!WindowShouldClose())
    {
        // Player Collision Boundries
        Rectangle scarfyRec = {
            Scarfy.pos.x, Scarfy.pos.y,
            Scarfy.source.width, Scarfy.source.height
        };

        // Win Line Position
        const float winLine{Nebulas[NebulaSize - 1].pos.x + 150};

        // Collision Check
        for (Character& nebula : Nebulas)
        {
            float pad{60};
            Rectangle nebulaRec = {
                nebula.pos.x + pad, 
                nebula.pos.y + pad,
                nebula.source.width - 2*pad, 
                nebula.source.height - 2*pad
            };

            if (CheckCollisionRecs(scarfyRec, nebulaRec))
            {
                collision = true;
            }
        }
        
        BeginDrawing();
        ClearBackground(WHITE);
        // DeltaTime is the time since last frame
        const float dT{GetFrameTime()};

        // Update bg X-Axis
        bgX += bgVel * dT;
        b_bgX += bgVel * dT;
        f_bgX += bgVel * dT;

        if (bgX <= -far_background.width * bgScale) bgX = 0.0;
        if (b_bgX <= -back_background.width * bgScale) b_bgX = 0.0f;
        if (f_bgX <= -fore_background.width * bgScale) f_bgX = 0.0f;
        
        Vector2 bg1Pos{bgX, 0.0};
        DrawTextureEx(far_background, bg1Pos, 0.0f, bgScale, WHITE);
        Vector2 bg2Pos{bgX + far_background.width * 2, 0.0};
        DrawTextureEx(far_background, bg2Pos, 0.0f, bgScale, WHITE);

        // back_background related
        Vector2 b_bg1Pos{b_bgX, 0.0f};
        DrawTextureEx(back_background, b_bg1Pos, 0.0f, bgScale, WHITE);
        Vector2 b_bg2Pos{b_bgX + back_background.width * 2, 0.0f};
        DrawTextureEx(back_background, b_bg2Pos, 0.0f, bgScale, WHITE);

        // fore background
        Vector2 f_bg1Pos{f_bgX, 0.0f};
        DrawTextureEx(fore_background, f_bg1Pos, 0.0f, bgScale, WHITE);
        Vector2 f_bg2Pos{f_bgX + fore_background.width * 2, 0.0f};
        DrawTextureEx(fore_background, f_bg2Pos, 0.0f, bgScale, WHITE);

        // Nebulas Animation
        for (int i = 0; i < NebulaSize; i++)
        {
            UpdateAnimation(Nebulas[i], dT);
        }

        // Scarfy Animation
        UpdateAnimation(Scarfy, dT);

        // Check player air state
        if (isOnGround(Scarfy, windowDimensions[1]))
        {
            // On Ground
            scarfy_vel = 0;
            isInAir = false;
        }
        else
        {
            // In Air
            scarfy_vel += gravity * dT;
            Scarfy.RunningTime = 0.0f;
            isInAir = true;
        }

        // Jump
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            scarfy_vel += jump_vel;
        }

        // Continues Y-Axis Movement Check
        Scarfy.pos.y += scarfy_vel * dT;
        gameOver = !collision && Scarfy.pos.x >= winLine;

        if (collision)
        {
            DrawText("Game Over!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        }
        else if (gameOver)
        {
            DrawText("You Won!", windowDimensions[0]/4, windowDimensions[1]/2, 40, RED);
        }
        else
        {
            if (!gameOver)
            {
                DrawTextureRec(Scarfy.texture, Scarfy.source, Scarfy.pos, WHITE);
                for (int i = 0; i < NebulaSize; i++)
                {
                    Nebulas[i].pos.x += nebula_vel * dT;
                    DrawTextureRec(Nebulas[i].texture, Nebulas[i].source, Nebulas[i].pos, WHITE);
                }
            }  
        }

        EndDrawing();
    }

    UnloadTexture(far_background);
    UnloadTexture(back_background);
    UnloadTexture(fore_background);
    UnloadTexture(Scarfy.texture);

    for (int i = 0; i < NebulaSize; i++)
    {
        UnloadTexture(Nebulas[i].texture);
    }
    CloseWindow();

    return 0;
}