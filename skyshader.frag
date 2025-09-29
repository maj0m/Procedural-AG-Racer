#version 450 core
precision highp float;

layout(std140, binding = 7) uniform ColorPalette {
    vec4 terrainColors[5];
	vec4 angleThresholds;
    vec4 grassColor;
    vec4 waterColor;
    vec4 skyColor;
    vec4 atmosphereColor;
    float fogDensity;
};

// Uniforms
//uniform float u_time;
uniform vec3  u_sunDir;
uniform float u_sunDiscSize;
uniform float u_sunGlow;

in vec3 wView;
out vec4 fragmentColor;


void main(){
    vec3 dir = normalize(-wView);

    // Base sky gradient
    float t = clamp(dir.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

    // Sun disc + halo
    vec3 sunDir = normalize(u_sunDir);
    float cosAng = clamp(dot(dir, sunDir), -1.0, 1.0);
    float ang    = acos(cosAng);

    // Inside disc -> 1, soft edge
    float disc = smoothstep(u_sunDiscSize, u_sunDiscSize*0.7, ang);

    // Wide halo falloff
    float halo = smoothstep(0.5, 0.0, ang / max(u_sunGlow, 1e-5));
    vec4 sunCol = vec4(1.0, 0.96, 0.85, 1.0);
    skyCol += sunCol * (disc * 8.0 + halo * 0.5);

    fragmentColor = vec4(skyCol.xyz, 1.0);
}
