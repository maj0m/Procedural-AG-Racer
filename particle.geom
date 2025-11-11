#version 450 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 u_V;
uniform mat4 u_P;

in VS_OUT {
    vec3 pos_WS;
    vec4 color;
    float age;
    float life;
    float size;
    float type;
} vin[];

out vec2 fragUV;
out vec4 fragColor;
out float normalizedAge;

void main() {
    vec4 camPos_VS = u_V * vec4(vin[0].pos_WS, 1.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = vec3(1.0, 0.0, 0.0);
    float halfSize = 0.5 * vin[0].size;
    
    vec3 corners[4];
    corners[0] = camPos_VS.xyz + (-right - up) * halfSize; 
    corners[1] = camPos_VS.xyz + ( +right - up) * halfSize; 
    corners[2] = camPos_VS.xyz + (-right + up) * halfSize; 
    corners[3] = camPos_VS.xyz + ( +right + up) * halfSize;

    vec2 uvs[4];
    uvs[0] = vec2(0.0, 0.0);
    uvs[1] = vec2(1.0, 0.0);
    uvs[2] = vec2(0.0, 1.0);
    uvs[3] = vec2(1.0, 1.0);

    // Emit particle
    normalizedAge = clamp(vin[0].age / vin[0].life, 0.0, 1.0);
    for (int i = 0; i < 4; ++i) {
        gl_Position = u_P * vec4(corners[i], 1.0);
        fragUV = uvs[i];
        fragColor = vin[0].color;
        EmitVertex();
    }
    EndPrimitive();
}
