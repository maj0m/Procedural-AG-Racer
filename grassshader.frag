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
    vec4 u_terrainColors[5];
	vec4 u_angleThresholds;
    vec4 u_grassColor;
    vec4 u_waterColor;
    vec4 u_skyColor;
    vec4 u_atmosphereColor;
    float u_fogDensity;
};

uniform sampler2D u_shadowMap;
uniform float     u_shadowBias;
uniform vec2      u_shadowTexel;

in float colorOffset;
in float viewDist_WS;
in vec3 viewDir_WS;
in vec4 lightPos_CS;

out vec4 fragmentColor;

// ---------- Shadow ----------
float shadowMask(vec4 lightPos_CS) {
    // Clip to NDC
    vec3 proj = lightPos_CS.xyz / lightPos_CS.w;
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
	vec3 xTangent = dFdx(viewDir_WS);
	vec3 yTangent = dFdy(viewDir_WS);
	vec3 N = normalize(cross(xTangent, yTangent));
	vec3 V = normalize(viewDir_WS);
	vec3 L = normalize(u_lightDir.xyz);
	vec3 H = normalize(L + V);
	float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
	float spec = pow(NdotH, material.shininess_pad.x) * NdotL;
	float diff = (NdotL * 0.5 + 0.5);

	// Green with variation from vShade
    vec3 texColor = u_grassColor.xyz + colorOffset;

    vec3 ambient = material.ka.xyz * texColor * u_lightLa.xyz;
	vec3 diffuse = material.kd.xyz * texColor * diff * u_lightLe.xyz;
	vec3 specular = material.ks.xyz * spec * u_lightLe.xyz;

    float shadow = shadowMask(lightPos_CS);
    vec3 radiance = ambient + (diffuse + specular) * shadow;

    // Base sky gradient
    float t = clamp(V.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(u_skyColor, u_atmosphereColor, t);

	// Fog
    float fogFactor = exp(-u_fogDensity * viewDist_WS * viewDist_WS);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}