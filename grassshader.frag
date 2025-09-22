#version 450 core

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

in float vShade;
in vec3 wView;						// interpolated world sp view
in vec3 wLight[8];					// interpolated world sp illum dir		
in float wDist;						// distance from camera
in vec3 vtxPos;

out vec4 fragmentColor;

void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);
	
	// Green with variation from vShade
    vec3 texColor = vec3(0.18, 0.45, 0.16) +  vShade * 0.25;

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