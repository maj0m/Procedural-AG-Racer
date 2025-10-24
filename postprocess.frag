#version 450 core
precision highp float;

uniform sampler2D u_sceneColor;

in vec2 uv;

out vec4 fragmentColor;

void main() {
    vec3 color = texture(u_sceneColor, uv).rgb;
    fragmentColor = vec4(color, 1.0);
}
