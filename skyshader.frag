#version 450 core
precision highp float;

in vec3 wView;
out vec4 fragmentColor;

// Uniforms
uniform float u_time;
uniform vec3  u_zenithColor;
uniform vec3  u_horizonColor;
uniform vec3  u_sunDir;
uniform float u_sunDiscSize;
uniform float u_sunGlow;


void main(){
    vec3 dir = normalize(-wView);

    // Base sky gradient
    float t = clamp(dir.y*0.5 + 0.5, 0.0, 1.0);
    vec3 skyCol = mix(u_horizonColor, u_zenithColor, t);

    // Sun disc + halo
    vec3 sunDir = normalize(u_sunDir);
    float cosAng = clamp(dot(dir, sunDir), -1.0, 1.0);
    float ang    = acos(cosAng);

    // Inside disc -> 1, soft edge
    float disc = smoothstep(u_sunDiscSize, u_sunDiscSize*0.7, ang);

    // Wide halo falloff
    float halo = smoothstep(0.5, 0.0, ang / max(u_sunGlow, 1e-5));
    vec3  sunCol = vec3(1.0, 0.96, 0.85);
    skyCol += sunCol * (disc * 8.0 + halo * 0.5);

    fragmentColor = vec4(skyCol, 1.0);
}
