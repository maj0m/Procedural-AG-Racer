#version 450 core

// Passed from grass_scatter.comp
layout (location=0) in vec3 vtxPos_OS;
layout (location=1) in vec3 instPos_WS;
layout (location=2) in float instYaw;
layout (location=3) in float instHeight;
layout (location=4) in float instWidth;
layout (location=5) in float instPhase;

// Uniforms
uniform float u_time;
uniform vec3 u_camPos_WS;
uniform mat4 u_V, u_P;
uniform mat4 u_lightVP;

out float colorOffset;
out float viewDist_WS;
out vec3 viewDir_WS;
out vec4 lightPos_CS;

float u_windStrength = 1.0;
vec2  u_windDir = vec2(1.0, 0.3); // XZ direction

mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

// ---------- Main ----------
void main() {
    // Scale
    vec3 p = vtxPos_OS;
    p.x *= instWidth;
    p.y *= instHeight;

    // Wind sway
    float topWeight = smoothstep(0.0, 1.0, vtxPos_OS.y);
    float sway = sin(u_time * 0.8 + instPhase) * u_windStrength * topWeight;
    vec2 windDir = normalize(u_windDir);
    p.xz += windDir * sway;

    // Rotate
    p.xz = rot(instYaw) * p.xz;

    vec3 vtxPos_WS = instPos_WS + p;
    gl_Position = u_P * u_V * vec4(vtxPos_WS, 1.0);

    viewDir_WS  = u_camPos_WS - vtxPos_WS;
	viewDist_WS = length(viewDir_WS);
    lightPos_CS = u_lightVP * vec4(vtxPos_WS, 1.0);
    colorOffset = fract(sin(dot(instPos_WS.xz, vec2(12.9898,78.233))) * 43758.5453) * 0.25; // per-blade color variation
}