#version 450 core

struct Particle {
    vec4 pos;
    vec4 vel;
    vec4 col;
    vec4 ext; // x=age, y=lifetime, z=size, w=type
};

layout(std430, binding = 0) buffer Particles {
    Particle particles[];
};

out VS_OUT {
    vec3 pos_WS;
    vec4 color;
    float age;
    float life;
    float size;
    float type;
} vout;

void main() {
    uint id = uint(gl_VertexID);
    Particle p = particles[id];

    vout.pos_WS = p.pos.xyz;
    vout.color = p.col;
    vout.age = p.ext.x;
    vout.life = p.ext.y;
    vout.size = p.ext.z;
    vout.type = p.ext.w;
}
