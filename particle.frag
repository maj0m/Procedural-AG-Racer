#version 450 core
precision highp float;

in vec2 fragUV;
in vec4 fragColor;
in float normalizedAge;

out vec4 fragmentColor;

void main() {
    vec2 dist = fragUV - vec2(0.5);
    float r2 = dot(dist, dist);

    float radialAlpha = step(r2, 0.25);
    float lifeAlpha = 1.0 - smoothstep(0.7, 1.0, normalizedAge);
    float finalAlpha = fragColor.a * lifeAlpha * radialAlpha;
    if (finalAlpha < 0.01) discard;

    fragmentColor = vec4(fragColor.rgb, finalAlpha);
}