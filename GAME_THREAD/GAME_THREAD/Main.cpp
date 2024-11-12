#include "icb_gui.h"
#include <windows.h>
#include <stdlib.h>

ICBYTES m; // For GUI rendering
int keypressed; // For key presses
int FRM1;

// Function prototypes
void WhenKeyPressed(int k);
void* SlidingBox(void* params);
void* TargetBox(void* params);
void* BulletMovement(void* params);
void Shoot(int* variables); // Shooting function
void butonfonk(); // Start/Stop button function

// Initialize GUI
void ICGUI_Create() {
    ICG_MWTitle("Falling Box Shooter");
    ICG_MWSize(450, 500);
}

// Configure and display the main game frame
void ConfigureMainFrame() {
    FRM1 = ICG_FrameMedium(5, 40, 400, 400);
    ICG_SetOnKeyPressed(WhenKeyPressed);
    CreateImage(m, 400, 400, ICB_UINT);
}

// Player-controlled box movement
void* SlidingBox(void* params) {
    int* variables = (int*)params; // Receive pointer for variables
    while (1) {
        FillRect(m, variables[0], variables[2], 20, 20, 0x000000); // Erase previous position
        if (keypressed == 37) variables[0] -= 2; // Move left and update boxX
        if (keypressed == 39) variables[0] += 2; // Move right and update boxX
        FillRect(m, variables[0], variables[2], 20, 20, 0xff0000); // Draw new position
        DisplayImage(FRM1, m);
        Sleep(30);
    }
    return NULL;
}

// Target box movement
void* TargetBox(void* params) {
    int* variables = (int*)params; // Receive pointer for variables
    int box_count = 5; // Number of boxes
    int box_widths[5] = { 10, 20, 30, 40, 50 }; // Box widths
    int box_positions[5][2]; // Array for box x, y positions

    for (int i = 0; i < box_count; i++) {
        box_positions[i][0] = rand() % 350; // Random x position
        box_positions[i][1] = 10; // Initial y position
    }

    while (1) {
        for (int i = 0; i < box_count; i++) {
            int x = box_positions[i][0];
            int y = box_positions[i][1];
            int width = box_widths[i];

            FillRect(m, x, y, width, 10, 0x000000); // Clear previous box

            // Update box position (falling down)
            y += 2;
            if (y > 400) y = 10; // Reset to top if it reaches the bottom
            box_positions[i][1] = y;

            FillRect(m, x, y, width, 10, 0x00ff00); // Draw new box position

            // Collision detection for each box with the bullet
            if (variables[5] != -1 && // Bullet is active
                variables[4] >= x && variables[4] <= x + width && // Bullet within box x range
                variables[5] >= y && variables[5] <= y + 10) { // Bullet within box y range

                // Handle collision
                variables[5] = -1; // Reset bullet after hit
                FillRect(m, variables[4], variables[5], 4, 8, 0x000000); // Clear bullet trace
            }
        }
        DisplayImage(FRM1, m);
        Sleep(50);
    }
    return NULL;
}

// Bullet movement (updated)
void* BulletMovement(void* params) {
    int* variables = (int*)params; // Receive pointer for variables

    while (variables[5] != -1) { // Bullet is active
        FillRect(m, variables[4], variables[5], 4, 8, 0x000000); // Erase previous bullet position
        variables[5] -= 5; // Move bullet upwards

        if (variables[5] < 0) { // Bullet goes off the screen
            FillRect(m, variables[4], variables[5], 4, 8, 0x000000); // Clear final bullet trace
            variables[5] = -1; // Reset bullet position to inactive state
            break;
        }

        if (variables[5] != -1) { // If bullet is still on the screen
            FillRect(m, variables[4], variables[5], 4, 8, 0xffffff); // Draw bullet
        }
        DisplayImage(FRM1, m);
        Sleep(30);
    }
    return NULL;
}

// Shoot function (updated)
void Shoot(int* variables) {
    if (variables[5] == -1) { // Only shoot if bullet is inactive
        // Center bullet horizontally relative to the red box
        variables[4] = variables[0] + 8; // Center bullet horizontally relative to red box
        variables[5] = variables[2];     // Start bullet at the red box's y position

        DWORD dw;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BulletMovement, variables, 0, &dw);
    }
}

// Start/Stop button function
void butonfonk() {
    static bool threads_started = false;
    static int variables[6] = { 200, 200, 380, 10, -1, -1 }; // {boxX, targetX, boxY, targetY, bulletX, bulletY}

    if (!threads_started) {
        threads_started = true;

        DWORD dw;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SlidingBox, variables, 0, &dw);
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TargetBox, variables, 0, &dw);

        SetFocus(ICG_GetMainWindow());
    }
    else {
        threads_started = false;
    }
}

// Key press event handler
void WhenKeyPressed(int k) {
    keypressed = k;
    static int variables[6] = { 200, 200, 380, 10, -1, -1 }; // {boxX, targetX, boxY, targetY, bulletX, bulletY}

    // Update boxX in response to left or right key press
    if (k == 37) {
        variables[0] -= 2; // Move left (update boxX)
    }
    if (k == 39) {
        variables[0] += 2; // Move right (update boxX)
    }

    if (k == 32) { // Space key press for shooting
        Shoot(variables);
    }
}

// Main GUI function
void ICGUI_main() {
    ICG_Button(5, 5, 120, 25, "START / STOP", butonfonk);
    FRM1 = ICG_FrameMedium(5, 40, 400, 400);
    ICG_SetOnKeyPressed(WhenKeyPressed);

    CreateImage(m, 400, 400, ICB_UINT);
}