#version 450 core
precision highp float;
		
struct Light {
	vec3 La, Le;
	vec3 dir;
};

struct Material {
	vec3 kd, ks, ka;
	float shininess;
};

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
    float _pad3;
    float _pad4;
};

layout(std140, binding = 7) uniform ColorPalette {
    vec4 terrainColors[5];
    vec4 grassColor;
    vec4 waterColor;
    vec4 fogColor;
    vec4 angleThresholds;
    float fogDensity;
};

uniform float u_time;
uniform Material material;
uniform Light light;

in float wDist;	// distance from camera
in vec3 wView;		
in vec3 vertexPos;

out vec4 fragmentColor;
	
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

float cubeVal(vec3 pos) {
   
    // Bedrock
    float noise = 0.0;
    float frequency = u_bedrockFrequency;
    float amplitude = u_bedrockAmplitude;

    for(int i = 0; i < 6; ++i){
        noise += simplex3d(pos * frequency) * amplitude;
        frequency *= u_frequencyMultiplier;
        amplitude *= u_amplitudeMultiplier;
    }
    
    return  noise + u_floorLevel;
}

void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);

	vec3 texColor = waterColor.xyz;

	float alpha = 0.7;
	vec3 foamColor = vec3(1.0);
	float waterDepth = cubeVal(vertexPos);
	float epsilon = 1.0;
	if(abs(waterDepth) < epsilon) {
		float foamFactor = abs(waterDepth) / epsilon;
		texColor = mix(foamColor, texColor, foamFactor);
		alpha = mix(1.0, alpha, foamFactor);
	}

	vec3 ka = material.ka * texColor;
	vec3 kd = material.kd * texColor;
	vec3 ks = material.ks;

    // Directional light: constant direction
    vec3 L = normalize(light.dir);      // direction TOWARD surface
    vec3 H = normalize(L + V);
    float cost = max(dot(N, L), 0.0);
    float cosd = max(dot(N, H), 0.0);

    vec3 radiance = ka * light.La + (kd * cost + ks * pow(cosd, material.shininess)) * light.Le;


    float fogFactor = exp(-fogDensity * wDist * wDist);
    vec3 finalColor = mix(fogColor.xyz, radiance, fogFactor);
    fragmentColor = vec4(finalColor, 1.0);
}