#version 450 core
precision highp float;

layout(location = 0) in vec3 vertexPos;

// UBO set in ChunkManager
layout(std140, binding = 3) uniform TerrainParams {
    float u_bedrockFrequency;
    float u_bedrockAmplitude;
    float u_frequency;
    float u_frequencyMultiplier;
    float u_amplitude;
    float u_amplitudeMultiplier;
    float u_floorLevel;
    float u_blendFactor;
    float u_warpFreq;
    float u_warpAmp;
    float u_warpStrength;
    float u_warpFreqMult;
    float u_warpAmpMult; 
    int u_warpOctaves;
    int u_seed;
    float u_waterLevel;
};

uniform float u_time;
uniform vec2 u_planeOriginXZ;
uniform vec3 u_camPos_WS;
uniform mat4 u_V, u_P;
uniform mat4 u_lightVP;

out float viewDist_WS;
out vec3 viewDir_WS;
out vec3 vtxPos_WS;
out vec3 vtxPos_VS;
out vec4 lightPos_CS;

// ---------- Seed ----------
vec3 seedOffset(int s) {
    return vec3(
        float(s) * 127.1 + 311.7,
        float(s) * 269.5 + 183.3,
        float(s) * 419.2 + 247.0
    );
}

// ---------- Noise ----------
vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

// skew constants for 3d simplex functions
const float F3 =  0.3333333;
const float G3 =  0.1666667;
float simplex3d(vec3 p) {
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 vec4 w, d;
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 w = max(0.6 - w, 0.0);
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 w *= w;
	 w *= w;
	 d *= w;

	 return dot(d, vec4(52.0));
}

float fbmSimplex3D(vec3 p, float freq, float amp, float fMul, float aMul, int octs) {
    p += seedOffset(u_seed);

    float acc = 0.0;
    for (int i = 0; i < octs; ++i) {
        acc += simplex3d(p * freq) * amp;
        freq *= fMul;
        amp  *= aMul;
    }
    return acc;
}

// ---------- Main ----------
void main() {
	vtxPos_WS = vertexPos + vec3(u_planeOriginXZ.x, u_waterLevel, u_planeOriginXZ.y);
	vtxPos_WS.y += fbmSimplex3D(vec3(vtxPos_WS.x, u_time * 10.0, vtxPos_WS.z), 0.02, 1.0, 2.0, 0.5, 2);

	vec4 vtxPos_CS = u_P * u_V * vec4(vtxPos_WS, 1.0);
	gl_Position = vtxPos_CS;

	viewDir_WS  = u_camPos_WS - vtxPos_WS.xyz;
	viewDist_WS = length(viewDir_WS);         // Distance from eye to vertex
	lightPos_CS = u_lightVP * vec4(vtxPos_WS, 1.0);
	vtxPos_VS = (u_V * vec4(vtxPos_WS, 1.0)).xyz;
}