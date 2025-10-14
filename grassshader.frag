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
vec3 u_zenithColor = vec3(0.06f, 0.22f, 0.60f);
vec3 u_horizonColor = vec3(0.65f, 0.78f, 0.90f);

in float vShade;
in vec3 wView;						// interpolated world sp view
in float wDist;						// distance from camera
in vec3 vtxPos;
in float vShadow;

out vec4 fragmentColor;

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

    float shadowTerm = 1.0 - vShadow * 0.7; // 1 in light, 0 in shadow
    vec3 radiance = ambient + (diffuse + specular) * shadowTerm;

    // Base sky gradient
    float t = clamp(V.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

	// Fog
    float fogFactor = exp(-fogDensity * wDist * wDist);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}