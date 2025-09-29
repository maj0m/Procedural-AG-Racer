#version 450 core
precision highp float;

struct Light {
	vec3 La, Le;
	vec3 dir;
};

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

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
    float _pad4;
};

uniform vec3 u_chunkId;
uniform float u_chunkSize;
uniform float u_time;
uniform mat4 MVP, M;					// MVP, Model
uniform vec3 wEye;						// Eye position

out float waterDepth;
out float wDist;
out vec3 wView;
out vec3 vtxPos;

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

// ---------- Terrain density ----------
float bedrockDensityAt(vec3 pos) {
    // Bedrock
    float bedrockNoise = fbmSimplex3D(pos, u_bedrockFrequency, u_bedrockAmplitude, u_frequencyMultiplier, u_amplitudeMultiplier, 6); // Accumulator
    float bedrockDensity = -pos.y + bedrockNoise + u_floorLevel;

    return bedrockDensity;
}

// ---------- Main ----------
void main() {
	
	vtxPos = vertexPos + u_chunkId * u_chunkSize;
	waterDepth = bedrockDensityAt(vtxPos);
	vtxPos.y += fbmSimplex3D(vec3(vtxPos.x, u_time * 8.0, vtxPos.z), 0.02, 4.0, 2.0, 0.5, 2);
	
	gl_Position = vec4(vtxPos, 1.0) * MVP;

	vec4 wPos = vec4(vtxPos, 1) * M;
	wView  = wEye - wPos.xyz;
	wDist = length(wView);         // Distance from eye to vertex
}