#include "pf_raylib_helper_3d.h"

#include <float.h>
#include <raylib.h>
#include <raymath.h>

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

int main(void)
{
    // Init raylib window and set target FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "PixelForge - Animated Model");


    // Create a rendering buffer in RAM as well as in VRAM (see pf_raylib_helper.h)
    PF_Renderer renderer = PF_LoadRenderer(800, 600);

    // Load a 3D model with raylib
    Model model = LoadModel(RESOURCES_PATH "models/robot.glb");

    // Load the animations of the 3D model with raylib
    int animsCount = 0;
    unsigned int animIndex = 0;
    unsigned int animCurrentFrame = 0;
    ModelAnimation *modelAnimations = LoadModelAnimations(RESOURCES_PATH "models/robot.glb", &animsCount);

    // Creer des structures avec les tampons de sommets du maillage
    PF_Model pfModel = PF_LoadModel(model); 

    // Start the main loop
    while (!WindowShouldClose())
    {
        // Select current animation
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) animIndex = (animIndex + 1)%animsCount;
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) animIndex = (animIndex + animsCount - 1)%animsCount;

        // Update model animation
        ModelAnimation anim = modelAnimations[animIndex];
        animCurrentFrame = (animCurrentFrame + 1)%anim.frameCount;
        UpdateModelAnimation(model, anim, animCurrentFrame);

        // Update camera position/diraction
        pf_mat4_look_at(renderer.rn3D.mat_view,
            (float[3]) { 10.0f*cosf(GetTime()), 5, 10.0f*sinf(GetTime()) },
            (float[3]) { 0, 2.5f, 0 }, (float[3]) { 0, 1, 0 });

        // Clear the destination buffer (RAM)
        PF_Clear(renderer, BLACK);

        // Rendu des tampon de sommets
        PF_DrawModel(renderer, pfModel);
        PF_UpdateRenderer(renderer);

        // Texture rendering via raylib
        BeginDrawing();
            ClearBackground(BLACK);
            PF_DrawRenderer(renderer);
            DrawFPS(10, 10);
        EndDrawing();
    }

    // Unload assets
    UnloadModel(model);
    UnloadModelAnimations(modelAnimations, animsCount);

    // Unload the renderer and associated data
    PF_UnloadRenderer(renderer);

    // Close raylib window
    CloseWindow();

    return 0;
}