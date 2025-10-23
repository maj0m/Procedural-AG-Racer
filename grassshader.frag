#version 450 core

struct Material {
    vec4 kd;
    vec4 ks;
    vec4 ka;
    vec4 shininess_pad;
};

layout(std140, binding = 2) uniform Lighting {
    vec4 u_lightDir;
    vec4 u_lightLa;
    vec4 u_lightLe;
};

layout(std140, binding = 6) uniform Materials {
    Material materials[16];
};

#define MAT_GRASS 1
#define material (materials[MAT_GRASS])

layout(std140, binding = 7) uniform ColorPalette {
    vec4 terrainColors[5];
	vec4 angleThresholds;
    vec4 grassColor;
    vec4 waterColor;
    vec4 skyColor;
    vec4 atmosphereColor;
    float fogDensity;
};

// Uniforms
uniform sampler2D u_shadowMap;
uniform vec2      u_shadowTexel;  // (1/width, 1/height)
uniform float     u_shadowBias;

in float vShade;
in vec3 wView;						// interpolated world sp view
in float wDist;						// distance from camera
in vec3 vtxPos;
in vec4 lightClip;

out vec4 fragmentColor;

// ---------- Shadow ----------
float shadowMask(vec4 lightClip) {
    // Clip to NDC
    vec3 proj = lightClip.xyz / lightClip.w;
    vec2 uv = proj.xy * 0.5 + 0.5;
    float depth = proj.z * 0.5 + 0.5;

    // Outside map
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) return 1.0;

    // 3x3 PCF
    float vis = 0.0;
    int kernel = 1;
    for (int dx = -kernel; dx <= kernel; ++dx) {
        for (int dy = -kernel; dy <= kernel; ++dy) {
            vec2 offset = vec2(dx, dy) * u_shadowTexel;
            float closest = texture(u_shadowMap, uv + offset).r;
            float current = depth - u_shadowBias;
            vis += (current <= closest) ? 1.0 : 0.0;
        }
    }

    return vis / pow(kernel * 2 + 1, 2);
}

// ---------- Main ----------
void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent));
	vec3 V = normalize(wView);
	vec3 L = normalize(u_lightDir.xyz);
	vec3 H = normalize(L + V);
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
	float spec = pow(NdotH, material.shininess_pad.x) * NdotL;
	float diff = (NdotL * 0.5 + 0.5);

	// Green with variation from vShade
    vec3 texColor = grassColor.xyz + vShade * 0.25;

    vec3 ambient = material.ka.xyz * texColor * u_lightLa.xyz;
	vec3 diffuse = material.kd.xyz * texColor * diff * u_lightLe.xyz;
	vec3 specular = material.ks.xyz * spec * u_lightLe.xyz;

    float shadow = shadowMask(lightClip);
    vec3 radiance = ambient + (diffuse + specular) * shadow;

    // Base sky gradient
    float t = clamp(V.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

	// Fog
    float fogFactor = exp(-fogDensity * wDist * wDist);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}