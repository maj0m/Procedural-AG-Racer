#version 450 core
precision highp float;

layout(std140, binding = 2) uniform Lighting {
    vec4 u_lightDir;
    vec4 u_lightLa;
    vec4 u_lightLe;
};

layout(std140, binding = 7) uniform ColorPalette {
    vec4 terrainColors[5];
	vec4 angleThresholds;
    vec4 grassColor;
    vec4 waterColor;
    vec4 skyColor;
    vec4 atmosphereColor;
    float fogDensity;
};

in vec3 wView;
out vec4 fragmentColor;

const float sunDiscSize = 0.08;
const float sunGlow = 0.3;

void main(){
    vec3 dir = normalize(-wView);

    // Base sky gradient
    float t = clamp(dir.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

    // Sun disc + halo
    vec3 sunDir = normalize(u_lightDir.xyz);
    float cosAng = clamp(dot(dir, sunDir), -1.0, 1.0);
    float ang = acos(cosAng);

    // Inside disc -> 1, soft edge
    float disc = smoothstep(sunDiscSize, sunDiscSize*0.7, ang);

    // Wide halo falloff
    float halo = smoothstep(0.5, 0.0, ang / max(sunGlow, 1e-5));
    vec4 sunCol = vec4(1.0, 0.96, 0.85, 1.0);
    skyCol += sunCol * (disc * 8.0 + halo * 0.5);

    fragmentColor = vec4(skyCol.xyz, 1.0);
}
