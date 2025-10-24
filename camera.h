#pragma once
#include "framework.h"
#include "globals.h"

class Camera {
private:

    vec3 pos;
    vec3 forward, right, up;
    float fov, aspectRatio;
    float nearPlane, farPlane;
    float focalLength;
    float sensitivity = 0.25f;
    float speedNormal = 200.0f;
    float speedSprint = 400.0f;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;
    float yaw = 0.0f;
    float pitch = 0.0f;
    bool firstMouse = false;

public:
    Camera() {
        pos = vec3(0.0f, 0.0f, 0.0f);
        forward = vec3(0.0f, 0.0f, -1.0f);
        up = vec3(0.0f, 1.0f, 0.0f);
        fov = 75.0f;
        aspectRatio = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
        focalLength = 1.0f / tanf(radians(fov) * 0.5f);
        farPlane = 0.1f;
        nearPlane = 2000.0f;
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
        pos = lerp(pos, targetPosition, dt * followSpeed);
        forward = normalize(lerp(forward, targetDirection, dt * followSpeed / 2));
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
        forward = normalize(direction);
    }

    void move(float deltaTime) {
        float speed = KEYDOWN_CTRL ? speedSprint : speedNormal;
        float step = speed * deltaTime;

        vec3 front = normalize(forward);
        vec3 right = normalize(cross(front, up));

        if (KEYDOWN_W)      pos += step * front;
        if (KEYDOWN_S)      pos -= step * front;
        if (KEYDOWN_A)      pos -= step * right;
        if (KEYDOWN_D)      pos += step * right;
        if (KEYDOWN_SPACE)  pos += step * up;
        if (KEYDOWN_SHIFT)  pos -= step * up;
    }

    vec3 getPos() {
        return pos;
    }

    void setPos(vec3 cameraPos) {
        pos = cameraPos;
    }

    vec3 getDir() {
        return forward;
    }

    void setDir(vec3 cameraDir) {
        forward = cameraDir;
    }

    void setFirstMouse() {
        firstMouse = true;
    }

    mat4 V() {
        vec3 w = normalize(-forward);
        vec3 u = normalize(cross(up, w));
        vec3 v = cross(w, u);

        return mat4(
            vec4(u.x, v.x, w.x, 0.0),
            vec4(u.y, v.y, w.y, 0.0),
            vec4(u.z, v.z, w.z, 0.0),
            vec4(-dot(u, pos), -dot(v, pos), -dot(w, pos), 1.0)
        );
    }

    mat4 P() {
        return mat4(
            vec4(focalLength / aspectRatio, 0.0f, 0.0f, 0.0f),
            vec4(0.0f, focalLength, 0.0f, 0.0f),
            vec4(0.0f, 0.0f, -(nearPlane + farPlane) / (nearPlane - farPlane), -1.0f),
            vec4(0.0f, 0.0f, -(2.0f * nearPlane * farPlane) / (nearPlane - farPlane), 0.0f)
        );
    }
};