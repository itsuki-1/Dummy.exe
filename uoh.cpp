#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "raylib.h"
#include "arrayNotMT.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cstdlib>

int main()
{
    const int screenWidth = 1000;

    const int screenHeight = 450;

    // SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Make it draggable

    InitWindow(screenWidth, screenHeight, "Totally Not Malware");

    SetTargetFPS(120);

    int animFrames{ 0 };

    const char* appDir{ GetApplicationDirectory() };

    if (appDir == nullptr)
    {
        std::cerr << "Failed to get application directory." << std::endl;

        CloseWindow();

        return -1;
    }

    std::string gifPath = std::string(appDir) + "\\chisato.gif";

    if (!FileExists(gifPath.c_str()))
    {
        std::string url{ "https://i.imgur.com/ZAOxRUL.gif" };

        std::string downloadPath{ std::string(appDir) + "\\chisato.gif" };

        std::string command = "powershell -Command \""
            "$Response = Invoke-WebRequest -Uri '" + url + "' -UseBasicParsing -Method GET;"
            "$Response.Content | Set-Content -Path '" + downloadPath + "' -Encoding Byte\"";

        int result{ system(command.c_str()) };

        if (result != 0)
        {
            std::cerr << "Failed to execute PowerShell command" << '\n';

            return -1;
        }
        else
        {
            std::cout << "File downloaded successfully to " << downloadPath << '\n';
        }
    }

    Image chisatoGIF{ LoadImageAnim(gifPath.c_str(), &animFrames) };

    Texture2D chisatoGIFAnim{ LoadTextureFromImage(chisatoGIF) };

    unsigned int nextFrameDataOffset{ 0 };

    int currentAnimFrame{ 0 };

    int frameDelay{ 8 };

    int frameCounter{ 0 };

    char cardNumber[20] = "\0";
    char cvc[5] = "\0";
    char date[11] = "\0";

    bool cardNumberActive{ false };
    bool cvcActive{ false };
    bool dateActive{ false };
    bool buttonActive{ false };
    bool dataSaved{ false };

    Rectangle cardNumberRec = { 547, 180, 400, 40 };
    Rectangle cvcRec = { 547, 245, 400, 40 };
    Rectangle dateRec = { 547, 310, 400, 40 };
    Rectangle buttonRec = { 547, 365, 120, 40 };

    // std::this_thread::sleep_for(std::chrono::seconds(5));

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        if (CheckCollisionPointRec(GetMousePosition(), cardNumberRec))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                cardNumberActive = true;
                cvcActive = false;
                dateActive = false;
                buttonActive = false;
            }
        }
        else if (CheckCollisionPointRec(GetMousePosition(), cvcRec))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                cvcActive = true;
                cardNumberActive = false;
                dateActive = false;
                buttonActive = false;
            }
        }
        else if (CheckCollisionPointRec(GetMousePosition(), dateRec))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                dateActive = true;
                cardNumberActive = false;
                cvcActive = false;
                buttonActive = false;
            }
        }
        else if (CheckCollisionPointRec(GetMousePosition(), buttonRec))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                dateActive = false;
                cardNumberActive = false;
                cvcActive = false;
                buttonActive = true;
            }
        }
        else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            buttonActive = cardNumberActive = cvcActive = dateActive = false;
        }

        int key = GetCharPressed();

        while (key > 0)
        {
            if (key >= 32 && key <= 125)
            {
                if (cardNumberActive && strlen(cardNumber) < 19)
                {
                    int len = strlen(cardNumber);
                    cardNumber[len] = (char)key;
                    cardNumber[len + 1] = '\0';
                }
                else if (cvcActive && strlen(cvc) < 3)
                {
                    int len = strlen(cvc);
                    cvc[len] = (char)key;
                    cvc[len + 1] = '\0';
                }
                else if (dateActive && strlen(date) < 5)
                {
                    int len = strlen(date);
                    date[len] = (char)key;
                    date[len + 1] = '\0';
                }
            }

            key = GetCharPressed(); // Check next character
        }

        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (cardNumberActive && strlen(cardNumber) > 0) cardNumber[strlen(cardNumber) - 1] = '\0';
            if (cvcActive && strlen(cvc) > 0) cvc[strlen(cvc) - 1] = '\0';
            if (dateActive && strlen(date) > 0) date[strlen(date) - 1] = '\0';
        }

        if (CheckCollisionPointRec(GetMousePosition(), buttonRec) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            // Save to file using raylib's function
            std::string content = "Card Number: " + std::string(cardNumber) + "\n"
                "CVC: " + std::string(cvc) + "\n"
                "Date: " + std::string(date) + "\n";
            if (SaveFileText("card_info.txt", &content[0]))
            {
                TraceLog(LOG_INFO, "Data saved successfully!");

                dataSaved = true;

                buttonActive = false;
            }
            else
            {
                TraceLog(LOG_WARNING, "Failed to save data!");

                DrawText("Failed to save data!", 670, 375, 20, BLACK);

                buttonActive = false;
            }

            // Clear inputs
            cardNumber[0] = cvc[0] = date[0] = '\0';
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        frameCounter++;

        if (frameCounter >= frameDelay)
        {
            currentAnimFrame++;

            if (currentAnimFrame >= animFrames)
                currentAnimFrame = 0;

            nextFrameDataOffset = chisatoGIFAnim.width * chisatoGIFAnim.height * 4 * currentAnimFrame;

            UpdateTexture(chisatoGIFAnim, ((unsigned char *)chisatoGIF.data) + nextFrameDataOffset);

            frameCounter = 0;
        }

        DrawTexture(chisatoGIFAnim, 0, 0, WHITE);

        DrawText("H-Hi...\n\nDo you want to give your credit card\n\nto this cute anime girl name Chisato?\n\nP-pwease...?", 527 + 20, 10, 20, BLACK);

        DrawRectangleRec(cardNumberRec, cardNumberActive ? LIGHTGRAY : WHITE);
        DrawRectangleRec(cvcRec, cvcActive ? LIGHTGRAY : WHITE);
        DrawRectangleRec(dateRec, dateActive ? LIGHTGRAY : WHITE);
        DrawRectangleRec(buttonRec, buttonActive ? LIME : LIGHTGRAY);

        DrawRectangleLinesEx(cardNumberRec, 1.0, cardNumberActive ? LIME : BLACK);
        DrawRectangleLinesEx(cvcRec, 1.0, cvcActive ? LIME : BLACK);
        DrawRectangleLinesEx(dateRec, 1.0, dateActive ? LIME : BLACK);

        DrawText("Credit Card Number", 547, 160, 15, DARKGRAY);

        if (strlen(cardNumber) == 0)
            DrawText("1234 5678 9012 3456", 550, 190, 20, LIGHTGRAY);

        DrawText(cardNumber, 550, 190, 20, BLACK);

        DrawText("CVC", 547, 225, 15, DARKGRAY);

        if (strlen(cvc) == 0)
            DrawText("123", 550, 255, 20, LIGHTGRAY);

        DrawText(cvc, 550, 255, 20, BLACK);

        DrawText("Date", 547, 290, 15, DARKGRAY);

        if (strlen(date) == 0)
            DrawText("12/23", 550, 320, 20, LIGHTGRAY);

        DrawText(date, 550, 320, 20, BLACK);

        DrawText("T-Thanks", 555, 375, 20, DARKGRAY);

        if (dataSaved)
            DrawText(" Data saved successfully!", 670, 375, 20, LIME);

        EndDrawing();
    }

    UnloadTexture(chisatoGIFAnim);

    UnloadImage(chisatoGIF);

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
