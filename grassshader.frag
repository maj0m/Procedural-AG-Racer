#version 450 core

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

in float vShade;
in vec3 wView;						// interpolated world sp view
in float wDist;						// distance from camera
in vec3 vtxPos;

out vec4 fragmentColor;

void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);
	
	// Green with variation from vShade
    vec3 texColor = grassColor.xyz + vShade * 0.25;

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