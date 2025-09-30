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

layout(std140, binding = 7) uniform ColorPalette {
    vec4 terrainColors[5];
	vec4 angleThresholds;
    vec4 grassColor;
    vec4 waterColor;
    vec4 skyColor;
    vec4 atmosphereColor;
    float fogDensity;
};

uniform Material material;
uniform Light light;

in vec3 wView;
in float wDist;
in vec3 vtxPos;
out vec4 fragmentColor;

// ---------- Color helper ---------- 
vec3 normalToColor(vec3 N) {
	// Calculate the angle between the normal and the Y-axis
	float angle = degrees(acos(dot(N, vec3(0.0, 1.0, 0.0))));

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
	vec3 L = normalize(light.dir);
	vec3 H = normalize(L + V);
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
	float spec = (NdotL > 0.0 && NdotV > 0.0) ? pow(NdotH, material.shininess) : 0.0;

	vec3 texColor = normalToColor(N);

    vec3 radiance = material.ka * texColor * light.La +
		(material.kd * texColor * NdotL + material.ks * spec) * light.Le;

    // Sky gradient
    float t = clamp(V.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

	// Fog
    float fogFactor = exp(-fogDensity * wDist * wDist);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    fragmentColor = vec4(finalColor, 1.0);
}


