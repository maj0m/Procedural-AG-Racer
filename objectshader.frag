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

in vec3 wView;				
in vec3 wLight[8];

out vec4 fragmentColor;
	
void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);

	vec3 texColor = vec3(78, 162, 177) / 255.0;
	vec3 radiance = vec3(0, 0, 0);

	vec3 ka = material.ka * texColor;
	vec3 kd = material.kd * texColor;
	vec3 ks = material.ks;

	for(int i = 0; i < nLights; i++) {
		vec3 L = normalize(wLight[i]);
		vec3 H = normalize(L + V);
		float cost = max(dot(N,L), 0);
		float cosd = max(dot(N,H), 0);
		radiance += ka * lights[i].La + (kd * cost + ks * lights[i].Le * pow(cosd, material.shininess)) * lights[i].Le;
	}

	fragmentColor = vec4(radiance, 1.0);
}