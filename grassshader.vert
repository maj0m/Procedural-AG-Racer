#version 450 core

struct Light {
	vec3 La, Le;
	vec4 wLightPos;
};

layout (location=0) in vec3 aPos;     // (0,0,0), (1,0,0), (0,1,0)
layout (location=1) in vec3 iPos;     // world position per blade
layout (location=2) in float iYaw;    // yaw (radians)
layout (location=3) in float iHeight; // height scale
layout (location=4) in float iWidth;  // width scale
layout (location=5) in float iPhase;  // sway phase

uniform float u_time;
uniform mat4 MVP, M;					// MVP, Model
uniform Light[8] lights;				// Light sources 
uniform int nLights;
uniform vec3 wEye;						// Eye position

out float vShade;                       // tiny variation for fragment	
out vec3 wView;							// view in world space
out vec3 wLight[8];						// light dir in world space
out float wDist;						// distance from camera

float u_windStrength = 1.0;  // meters of lateral tip deflection
vec2  u_windDir = vec2(1.0, 0.3); // XZ direction (will be normalized)

mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, -s, s, c);
}

void main() {
    // Normalize wind dir on XZ plane
    vec2 wdir = normalize(u_windDir);

    // Local space -> anisotropic scale (width on X, height on Y)
    vec3 p = aPos;
    p.x *= iWidth;
    p.y *= iHeight;

    // Sway
    float topWeight = smoothstep(0.0, 1.0, aPos.y);
    float sway = sin(u_time * 0.8 + iPhase) * u_windStrength * topWeight;

    // Apply lateral deflection in local XZ (use wind direction)
    p.xz += wdir * sway;

    // Rotate around Y by yaw (in world)
    // Build a Y-rotation applied to XZ
    p.xz = rot(iYaw) * p.xz;

    // Translate to world position
    vec3 worldPos = iPos + p;

    for(int i = 0; i < nLights; i++) {
	    wLight[i] = lights[i].wLightPos.xyz - worldPos * lights[i].wLightPos.w;
	}
    wView  = wEye - worldPos;
	wDist = length(wView);

    // Small per-blade color variation
    vShade = fract(sin(dot(iPos.xz, vec2(12.9898,78.233))) * 43758.5453);

    gl_Position = vec4(worldPos, 1.0) * MVP;
}