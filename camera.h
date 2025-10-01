#pragma once
#include "framework.h"
#include <iostream>
#include "globals.h"


class Camera {
private:

    vec3 wEye, wFront, wUp;
    float fov, asp, fp, bp;
    float walkSpeed = 200.0f;
    float fastSpeed = 400.0f;
    float sensitivity = 0.25f;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    bool firstMouse = false;

public:
    Camera() {
        wEye = vec3(0.0f, 0.0f, 0.0f);
        wFront = vec3(0.0f, 0.0f, -1.0f);
        wUp = vec3(0.0f, 1.0f, 0.0f);
        asp = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
        fov = 75.0f;
        fp = 0.1f;
        bp = 2000.0f;
    }


    std::vector<vec4> getFrustumPlanes() {
        std::vector<vec4> planes(6);
        mat4 VP = TransposeMatrix(P() * V());

        planes[0] = VP[3] + VP[0];          // Left 
        planes[1] = VP[3] - VP[0];          // Right 
        planes[2] = VP[3] + VP[1];          // Bottom 
        planes[3] = VP[3] - VP[1];          // Top 
        planes[4] = VP[3] + VP[2];          // Near 
        planes[5] = VP[3] - VP[2];          // Far 

        // Normalize
        for (auto& p : planes) {
            vec3 n = vec3(p.x, p.y, p.z);
            float invLen = 1.0f / length(n);
            p.x *= invLen; p.y *= invLen; p.z *= invLen; p.w *= invLen;
        }


        return planes;
    }

    void followPlayer(const vec3& playerPos, const vec3& playerDirection, float distanceBehind, float heightAbove, float followSpeed, float dt) {
        vec3 targetPosition = playerPos + vec3(-playerDirection.x * distanceBehind, heightAbove, -playerDirection.z * distanceBehind);
        vec3 targetDirection = normalize(playerPos + playerDirection * 50.0 - targetPosition);
        wEye = lerp(wEye, targetPosition, dt * followSpeed);
        wFront = normalize(lerp(wFront, targetDirection, dt * followSpeed / 2));
    }


    void rotate(int pX, int pY) {
        if (firstMouse) {
            lastX = pX;
            lastY = pY;
            firstMouse = false;
        }

        float xoffset = pX - lastX;
        float yoffset = lastY - pY;
        lastX = pX;
        lastY = pY;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // Clamp camera pitch
        clamp(pitch, -89.0, 89.0);

        vec3 direction;
        direction.x = cos(radians(yaw)) * cos(radians(pitch));
        direction.y = sin(radians(pitch));
        direction.z = sin(radians(yaw)) * cos(radians(pitch));
        wFront = normalize(direction);
    }

    void move(float deltaTime) {
        float currentSpeed = (KEYDOWN_CTRL ? fastSpeed : walkSpeed);
        float step = currentSpeed * deltaTime;

        vec3 front = normalize(wFront);
        vec3 right = normalize(cross(front, wUp));
        vec3 up = wUp;

        if (KEYDOWN_W)      wEye += step * front;
        if (KEYDOWN_S)      wEye -= step * front;
        if (KEYDOWN_A)      wEye -= step * right;
        if (KEYDOWN_D)      wEye += step * right;
        if (KEYDOWN_SPACE)  wEye += step * up;
        if (KEYDOWN_SHIFT)  wEye -= step * up;
    }

    vec3 getEyePos() {
        return wEye;
    }

    void setEyePos(vec3 pos) {
        wEye = pos;
    }

    vec3 getEyeDir() {
        return wFront;
    }

    void setEyeDir(vec3 dir) {
        wFront = dir;
    }

    vec3 getEyeUp() {
        return wUp;
    }

    void setFirstMouse() {
        firstMouse = true;
    }



    mat4 V() {
        vec3 w = normalize(-wFront);            // backward
        vec3 u = normalize(cross(wUp, w));      // right
        vec3 v = cross(w, u);                   // up

        return mat4(
            vec4(u.x, v.x, w.x, 0.0),
            vec4(u.y, v.y, w.y, 0.0),
            vec4(u.z, v.z, w.z, 0.0),
            vec4(-dot(u, wEye), -dot(v, wEye), -dot(w, wEye), 1.0)
        );
    }


    mat4 P() {
        float f = 1.0f / tanf(radians(fov) * 0.5f); // = sy

        return mat4(
            vec4(f / asp, 0.0f, 0.0f, 0.0f),
            vec4(0.0f, f, 0.0f, 0.0f),
            vec4(0.0f, 0.0f, -(bp + fp) / (bp - fp), -1.0f),
            vec4(0.0f, 0.0f, -(2.0f * bp * fp) / (bp - fp), 0.0f)
        );
    }

};


