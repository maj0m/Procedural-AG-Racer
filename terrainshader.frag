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
    vec4 grassColor;
    vec4 waterColor;
    vec4 fogColor;
    vec4 angleThresholds;
    float fogDensity;
};

uniform Material material;
uniform Light light;

in vec3 wView;						// interpolated world sp view
in float wDist;						// distance from camera
in vec3 vtxPos;

out vec4 fragmentColor;				// output goes to frame buffer

void main() {
	vec3 xTangent = dFdx(vtxPos);
	vec3 yTangent = dFdy(vtxPos);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);

	// Calculate the angle between the normal and the Y-axis (up direction)
	float angle = acos(dot(N, vec3(0.0, 1.0, 0.0))) * (180.0 / 3.14159265);


	vec3 texColor;
	float angleTreshold1 = angleThresholds.x;
	float angleTreshold2 = angleThresholds.y;
	float angleTreshold3 = angleThresholds.z;
	float angleTreshold4 = angleThresholds.w;

	// Define the colors for each range
	vec3 colorA = terrainColors[0].xyz;
	vec3 colorB = terrainColors[1].xyz;
	vec3 colorC = terrainColors[2].xyz;
	vec3 colorD = terrainColors[3].xyz;
	vec3 colorE = terrainColors[4].xyz;

	// Color based on tri angle
	if (angle < angleTreshold1)			texColor = mix(colorA, colorB, angle / angleTreshold1);
	else if (angle < angleTreshold2)	texColor = mix(colorB, colorC, (angle - angleTreshold1) / (angleTreshold2 - angleTreshold1));
	else if (angle < angleTreshold3)	texColor = mix(colorC, colorD, (angle - angleTreshold2) / (angleTreshold3 - angleTreshold2));
	else if (angle < angleTreshold4)	texColor = mix(colorD, colorE, (angle - angleTreshold3) / (angleTreshold4 - angleTreshold3));
	else								texColor = colorE;


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


