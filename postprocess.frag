#version 450 core
precision highp float;

uniform sampler2D uScene;

in vec2 vUV;
out vec4 fragmentColor;

void main() {
    vec3 color = texture(uScene, vUV).rgb;
    fragmentColor = vec4(color, 1.0);
}
