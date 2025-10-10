#pragma once
#include "framework.h"
#include "camera.h"
#include "object.h"
#include "objectshader.h"
#include "collisioncomputeshader.h"
#include "ship.h"
#include "chunkmanager.h"

class Player {
    Camera* camera;
    ChunkManager* chunkManager;
    Object* playerObject;
    CollisionComputeShader* collisionComputeShader;

    vec3 pos;
    vec3 vel;
    vec3 acc;

    Quaternion rotation;
    vec3 forward, up, right;

    float rudder = 0.0f;       // Between -1 and 1
    float groundDist = 0.0f;   // Distance from ground (vertical)

    vec3 gravity = vec3(0.0f, -20.0f, 0.0f);
    float speed = 200.0f;
    float turnSpeed = 1.0f;
    float hoverHeight = 10.0f;
    float springStrength = 320.0f;
    float drag = 0.95f;
    float rollAngle = 0.36f;
    float sidewaysFriction = 6.0f;

    // Pitch
    float pitch = 0.0f;             // smoothed visual pitch (radians)
    float pitchMax = 0.35f;         // clamp value
    float pitchResponse = 10.0f;    // how fast visual pitch follows target
    float kPitch_vUp = 0.006f;      // vertical velocity
    float kPitch_aUp = 0.0006f;     // vertical accel
    float kPitch_aFwd = -0.0002f;   // fwd accel
    float kPitch_speed = -0.0002f;  // overall speed

    // For finite-difference acceleration
    float prevFwdSpeed = 0.0f;
    float prevUpSpeed = 0.0f;

    bool insideTerrain = false;

public:
    Player(Camera* camera, ChunkManager* chunkManager) : camera(camera), chunkManager(chunkManager) {       
        playerObject = new Object(new ObjectShader(), new ShipGeometry(4.0f, 8));
        collisionComputeShader = new CollisionComputeShader();

        pos = chunkManager->getSpawnPoint();
        vel = vec3(0, 0, 0);
        acc = vec3(0, 0, 0);
        rotation = Quaternion(1, 0, 0, 0);

        playerObject->pos = pos;
    }


    void Update(float dt) {
        // Camera
        camera->followPlayer(pos, forward, 5.0, 3.0, 32.0, dt);

        // Update Position
        playerObject->pos = pos;

        // Rotation
        if (KEYDOWN_A) Rotate(rotation, -turnSpeed * dt, up);
        if (KEYDOWN_D) Rotate(rotation, turnSpeed * dt, up);
        UpdateRotation();
        UpdateRudder(dt);
        
        GLuint segSSBO = 0, segCount = 0;
        chunkManager->getSegIndexForPos(pos, segSSBO, segCount);
        collisionComputeShader->Dispatch(playerObject->pos, hoverHeight, groundDist, segSSBO, segCount); // Calculate distance to ground

        // Gravity
        acc += gravity;

        // Acceleration
        if (KEYDOWN_W) acc += forward * speed;
        if (KEYDOWN_S) acc -= forward * speed;
        if (KEYDOWN_SPACE) acc += up * 50.0;

        // Sideway friction
        vec3 lateralVel = dot(vel, right) * right;
        vel -= lateralVel * sidewaysFriction * dt;

        // Suspension
        if (groundDist < hoverHeight && groundDist > 0.0f) {
            float springFactor = (hoverHeight - groundDist) / hoverHeight;
            acc += up * (springFactor * springStrength);
        }

        // Inside terrain - Collision
        if(abs(groundDist) < 0.0001f) {
            if (!insideTerrain) vel *= -0.5f;
            insideTerrain = true;
        }
        else {
            insideTerrain = false;
        }

        // Integration
        acc -= drag * vel;
        vel += acc * dt;
        pos += vel * dt;
        acc = vec3(0.0f, 0.0f, 0.0f);

        AlignVisual(dt);
    }

    // Cosmetic rotations
    void AlignVisual(float dt) {
        Quaternion bodyRotation = rotation;

        // Banking
        float bankAngle = rollAngle * rudder;
        Rotate(bodyRotation, bankAngle, bodyRotation.forward());

        // Speeds along current local axes
        float vFwd = dot(vel, forward);
        float vUp = dot(vel, up);

        // Finite-difference accelerations along those axes
        float invDt = (dt > 1e-4f) ? 1.0f / dt : 0.0f;
        float aFwd = (vFwd - prevFwdSpeed) * invDt;
        float aUp = (vUp - prevUpSpeed) * invDt;

        // Speed magnitude for "air load"
        float spd = length(vel);

        // Compose target pitch (positive = nose up)
        float desiredPitch =
            kPitch_vUp * vUp
            + kPitch_aUp * aUp
            + kPitch_aFwd * aFwd
            + kPitch_speed * spd;

        // Clamp and smooth (exp decay, frame-rate independent)
        clamp(desiredPitch, -pitchMax, pitchMax);
        float alpha = 1.0f - expf(-pitchResponse * dt);
        pitch += (desiredPitch - pitch) * alpha;

        // Apply pitch
        Rotate(bodyRotation, pitch, bodyRotation.right());
        playerObject->SetRotation(bodyRotation);

        // Update history for next frame
        prevFwdSpeed = vFwd;
        prevUpSpeed = vUp;
    }

    void UpdateRotation() {
        forward = rotation.forward();
        up = rotation.up();
        right = rotation.right();
    }

    void UpdateRudder(float dt) {
        const float rudderSpeed = 4.0;
        const float rudderReturnSpeed = 8.0;

        if (KEYDOWN_A) rudder += rudderSpeed * dt;
        if (KEYDOWN_D) rudder -= rudderSpeed * dt;

        // Move rudder towards 0 when no input is given
        if (!KEYDOWN_A && !KEYDOWN_D) rudder -= rudder * rudderReturnSpeed * dt;

        clamp(rudder, -1, 1);
    }

    void Rotate(Quaternion& rotation, float angle, vec3 axis) {
        Quaternion q = Quaternion::fromAxisAngle(axis, angle);
        rotation = rotation * q;
        rotation.normalize();
    }

    void Draw(RenderState& state) {
      playerObject->Draw(state);
    }

    void Respawn() {
        pos = chunkManager->getSpawnPoint();
        vel = vec3(0, 0, 0);
    }

    vec3 getPos() const {
        return pos;
    }
};
