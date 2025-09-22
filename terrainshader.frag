#version 450 core
precision highp float;

struct Light {
	vec3 La, Le;
	vec4 wLightPos;
};

struct Material {
	vec3 kd, ks, ka;
	float shininess;
};

uniform Material material;
uniform Light[8] lights;
uniform int   nLights;

in vec3 wView;						// interpolated world sp view
in vec3 wLight[8];					// interpolated world sp illum dir		
in float wDist;						// distance from camera
in vec3 vtxPos;

out vec4 fragmentColor;				// output goes to frame buffer



void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);

	// Calculate the angle between the normal and the Y-axis (up direction)
	float angle = acos(dot(N, vec3(0.0, 1.0, 0.0))) * (180.0 / 3.14159265);


	vec3 texColor;
	float angleTreshold1 = 15.0;
	float angleTreshold2 = 30.0;
	float angleTreshold3 = 45.0;
	float angleTreshold4 = 180.0;

	// Define the colors for each range
	vec3 colorA = vec3(228, 213, 211) / 255.0;
	vec3 colorB = vec3(170, 183, 203) / 255.0;
	vec3 colorC = vec3(122, 108, 147) / 255.0;
	vec3 colorD = vec3(90, 77, 122) / 255.0;
	vec3 colorE = vec3(61, 43, 74) / 255.0;

	// Color based on tri angle
	if (angle < angleTreshold1)			texColor = mix(colorA, colorB, angle / angleTreshold1);
	else if (angle < angleTreshold2)	texColor = mix(colorB, colorC, (angle - angleTreshold1) / (angleTreshold2 - angleTreshold1));
	else if (angle < angleTreshold3)	texColor = mix(colorC, colorD, (angle - angleTreshold2) / (angleTreshold3 - angleTreshold2));
	else if (angle < angleTreshold4)	texColor = mix(colorD, colorE, (angle - angleTreshold3) / (angleTreshold4 - angleTreshold3));
	else								texColor = colorE;


	vec3 ka = material.ka * texColor;
	vec3 kd = material.kd * texColor;
	vec3 ks = material.ks;

	vec3 radiance = vec3(0, 0, 0);
	for(int i = 0; i < nLights; i++) {
		vec3 L = normalize(wLight[i]);
		vec3 H = normalize(L + V);
		float cost = max(dot(N,L), 0);
		float cosd = max(dot(N,H), 0);
		radiance += ka * lights[i].La + (kd * cost + ks * lights[i].Le * pow(cosd, material.shininess)) * lights[i].Le;
	}

	float fogDensity = 0.000002;
	vec3 fogColor = vec3(0.6, 0.55, 0.45);
    float fogFactor = exp(-fogDensity * wDist * wDist);

    vec3 finalColor = mix(fogColor, radiance, fogFactor);
    fragmentColor = vec4(finalColor, 1.0);
}


