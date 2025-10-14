#pragma once
#include <GLFW/glfw3.h>

class FPSCounter {
private:
    // FPS
    int frameCount = 0;
    double previousFrameTime;
    int currentFPS = 0;

    // Average FPS
    double totalTimeElapsed = 0.0;
    long long totalFrames = 0;
    float currentAvgFPS = 0.0f;

    // Delta Time
    double lastFrameTime;
    float deltaTime = 0.0f;

public:
    FPSCounter() : previousFrameTime(glfwGetTime()), lastFrameTime(glfwGetTime()) {}

    void update() {
        double currentTime = glfwGetTime();

        // Delta time
        if (lastFrameTime == 0.0) lastFrameTime = currentTime;
        deltaTime = (float)(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        // FPS
        frameCount++;
        if (currentTime - previousFrameTime >= 1.0) {
            currentFPS = frameCount;
            frameCount = 0;
            previousFrameTime = currentTime;
        }

        // Average FPS
        totalTimeElapsed = currentTime;
        totalFrames++;
        currentAvgFPS = (float)totalFrames / (float)totalTimeElapsed;
    }

    // Getters
    float getDeltaTime() const { return deltaTime; }
    int getFPS() const { return currentFPS; }
    float getAverageFPS() const { return currentAvgFPS; }
};