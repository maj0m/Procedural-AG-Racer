#version 450 core
precision highp float;

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

#define MAT_WATER 2
#define material (materials[MAT_WATER])

layout(std140, binding = 7) uniform ColorPalette {
    vec4 u_terrainColors[5];
	vec4 u_angleThresholds;
    vec4 u_grassColor;
    vec4 u_waterColor;
    vec4 u_skyColor;
    vec4 u_atmosphereColor;
    float u_fogDensity;
};

layout(binding = 2) uniform sampler2D u_shadowMap;
layout(binding = 3) uniform sampler2D u_sceneColor;
layout(binding = 4) uniform sampler2D u_sceneDepth;
uniform float     u_shadowBias;
uniform vec2      u_shadowTexel;
uniform mat4      u_V, u_P;
uniform mat4      u_invP;

in float viewDist_WS;
in vec3 vtxPos_VS;
in vec3 viewDir_WS;
in vec4 lightPos_CS;

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

// ---------- SSR ----------
vec3 viewFromDepth(vec2 uv, float depth01) {
    // NDC
    float z = depth01 * 2.0 - 1.0;
    vec4 ndc = vec4(uv * 2.0 - 1.0, z, 1.0);
    vec4 vs = u_invP * ndc;
    return vs.xyz / vs.w;
}

// Returns uv of hit and weight (hit? 1:0)
vec3 ssrTrace(vec3 originVS, vec3 dirVS) {
    const int   STEPS = 100;
    const float STEP  = 5.0;
    const float THICK = 10.0;   // thickness tolerance
    vec3 p = originVS;

    for (int i=0; i<STEPS; ++i) {
        p += dirVS * STEP;

        // project to screen
        vec4 clip = u_P * vec4(p, 1.0);
        vec3 ndc  = clip.xyz / clip.w;
        vec2 uv   = ndc.xy * 0.5 + 0.5;

        // out of screen
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;

        float sceneDepth01 = texture(u_sceneDepth, uv).r;
        vec3  sceneVS      = viewFromDepth(uv, sceneDepth01);

        // ray vs depth plane test (both are in VS; z is negative forward)
        if (p.z >= sceneVS.z - THICK && p.z <= sceneVS.z + THICK) {
            return vec3(uv, 1.0);
        }
    }

    return vec3(0.0, 0.0, 0.0);
}

// ---------- Water Depth ----------
float getWaterDepth() {
    vec2 screenUV = gl_FragCoord.xy / vec2(textureSize(u_sceneDepth, 0));
    float terrainDepth01 = texture(u_sceneDepth, screenUV).r;
    vec3 terrainPos_VS = viewFromDepth(screenUV, terrainDepth01);

    float depth = vtxPos_VS.z - terrainPos_VS.z;
    return max(0.0, depth);
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
	float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
	float spec = pow(NdotH, material.shininess_pad.x) * NdotL;

	vec3 texColor = u_waterColor.xyz;
    float waterDepth = getWaterDepth();

	// Foam
	vec3 foamColor = vec3(1.0);
	if(waterDepth < 0.5) {
		texColor = foamColor;
	}

    vec3 ambient = material.ka.xyz * texColor * u_lightLa.xyz;
	vec3 diffuse = material.kd.xyz * texColor * NdotL * u_lightLe.xyz;
	vec3 specular = material.ks.xyz * spec * u_lightLe.xyz;

    float shadow = shadowMask(lightPos_CS);
    vec3 radiance = ambient + (diffuse + specular) * shadow;

    // Base sky gradient
    float t = clamp(V.y * 0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(u_skyColor, u_atmosphereColor, t);

    // Build reflection ray in view space
    vec3 N_VS = normalize((u_V * vec4(N, 0.0)).xyz);
    vec3 viewDir_VS = normalize(-vtxPos_VS);              // camera at origin in VS
    vec3 reflection_VS  = reflect(-viewDir_VS, N_VS);

    // start just above the surface to avoid self-hits
    vec3 start = vtxPos_VS + N_VS * 0.05;
    vec3 hit = ssrTrace(start, reflection_VS);

    vec3 reflColor = skyCol.xyz;                 // fallback
    if (hit.z > 0.5) {
        reflColor = texture(u_sceneColor, hit.xy).rgb;
    }

    // Schlick's approximation
    float R0 = 0.02;
    float fresnel = R0 + (1.0 - R0) * pow(1.0 - NdotV, 5.0);
    radiance = mix(radiance, reflColor, fresnel); 

	// Fog
    float fogFactor = exp(-u_fogDensity * viewDist_WS * viewDist_WS);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    // Alpha
    const float MAX_DEPTH = 12.0;
    float depthFactor = smoothstep(0.0, MAX_DEPTH, waterDepth); // 0..1
    float alpha = mix(0.5, 0.8, depthFactor);

    fragmentColor = vec4(finalColor, alpha);
}