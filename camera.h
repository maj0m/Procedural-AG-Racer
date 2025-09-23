#pragma once
#include "framework.h"
#include <iostream>
#include "globals.h"


class Camera {
private:

    vec3 wEye, wFront, wUp;
    float fov, asp, fp, bp;
    float speed = (float)20 / fps;
    float sensitivity = 0.25;
    float lastX = WINDOW_WIDTH / 2;
    float lastY = WINDOW_HEIGHT / 2;
    float yaw = 0.0;
    float pitch = 0.0;
    bool firstMouse = false;

public:
    Camera() {
        wEye = vec3(0.0, 30.0, 0.0);
        wFront = vec3(0.0, 0.0, -1.0);
        wUp = vec3(0.0, 1.0, 0.0);
        asp = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
        fov = 75;
        fp = 0.1;
        bp = 1000.0;
    }


    std::vector<vec4> getFrustumPlanes() {
        std::vector<vec4> planes(6);
        mat4 VP = TransposeMatrix(V1() * P());

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

    void move() {
        vec3 front = normalize(wFront);
        vec3 right = normalize(cross(front, wUp));

        if (KEYDOWN_W)      wEye += speed * wFront;
        if (KEYDOWN_S)      wEye -= speed * wFront;
        if (KEYDOWN_A)      wEye -= speed * normalize(cross(wFront, wUp));
        if (KEYDOWN_D)      wEye += speed * normalize(cross(wFront, wUp));
        if (KEYDOWN_SPACE)  wEye += speed * wUp;
        if (KEYDOWN_SHIFT)  wEye -= speed * wUp;
        if (KEYDOWN_CTRL)   speed = min(200.0 / fps, 0.5);
        else                speed = min(100.0 / fps, 0.2);
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
        vec3 w = normalize(-wFront);
        vec3 u = normalize(cross(wUp, w));
        vec3 v = cross(w, u);

        return TranslateMatrix(-wEye) * mat4(u.x, v.x, w.x, 0.0,
            u.y, v.y, w.y, 0.0,
            u.z, v.z, w.z, 0.0,
            0.0, 0.0, 0.0, 1.0);
    }

    mat4 P() {
        float sy = 1.0 / tan(radians(fov / 2.0));

        return mat4(sy / asp, 0.0, 0.0, 0.0,
            0.0, sy, 0.0, 0.0,
            0.0, 0.0, -(fp + bp) / (bp - fp), -1.0,
            0.0, 0.0, -2.0 * fp * bp / (bp - fp), 0.0);
    }

    // Alternate view matrix with wEye set back for frustum culling (quick & dirty)
    mat4 V1() {
        vec3 w = normalize(-wFront);
        vec3 u = normalize(cross(wUp, w));
        vec3 v = cross(w, u);

        return TranslateMatrix(-wEye + wFront * 128) * mat4(u.x, v.x, w.x, 0.0,
            u.y, v.y, w.y, 0.0,
            u.z, v.z, w.z, 0.0,
            0.0, 0.0, 0.0, 1.0);
    }
};


