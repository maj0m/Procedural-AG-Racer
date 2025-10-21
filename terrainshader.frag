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

#define MAT_TERRAIN 0
#define material (materials[MAT_TERRAIN])

layout(std140, binding = 7) uniform ColorPalette {
    vec4 terrainColors[5];
	vec4 angleThresholds;
    vec4 grassColor;
    vec4 waterColor;
    vec4 skyColor;
    vec4 atmosphereColor;
    float fogDensity;
};

flat in float vShadow;  // 0=lit, 1=shadowed
in vec3 wView;
in float wDist;
in vec3 vtxPos;
out vec4 fragmentColor;

// ---------- Color helper ---------- 
vec3 normalToColor(vec3 N) {
	// Calculate the angle between the normal and the Y-axis
	float angle =  N.y < 0.9999 ? degrees(acos(dot(N, vec3(0.0, 1.0, 0.0)))) : 0.0;

	// Color based on tri angle
	vec3 col = vec3(0.0);
	if		(angle < angleThresholds.x)	col = mix(terrainColors[0].xyz, terrainColors[1].xyz, angle / angleThresholds.x);
	else if (angle < angleThresholds.y)	col = mix(terrainColors[1].xyz, terrainColors[2].xyz, (angle - angleThresholds.x) / (angleThresholds.y - angleThresholds.x));
	else if (angle < angleThresholds.z)	col = mix(terrainColors[2].xyz, terrainColors[3].xyz, (angle - angleThresholds.y) / (angleThresholds.z - angleThresholds.y));
	else if (angle < angleThresholds.w)	col = mix(terrainColors[3].xyz, terrainColors[4].xyz, (angle - angleThresholds.z) / (angleThresholds.w - angleThresholds.z));
	else								col = terrainColors[4].xyz;
	return col;
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

	vec3 texColor = normalToColor(N);

    vec3 ambient = material.ka.xyz * texColor * u_lightLa.xyz;
	vec3 diffuse = material.kd.xyz * texColor * NdotL * u_lightLe.xyz;
	vec3 specular = material.ks.xyz * spec * u_lightLe.xyz;

    float shadowTerm = 1.0 - vShadow * 0.7; // 1 in light, 0 in shadow
    vec3 radiance = ambient + (diffuse + specular) * shadowTerm;

    // Sky gradient
    float t = clamp(V.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

	// Fog
    float fogFactor = exp(-fogDensity * wDist * wDist);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}



