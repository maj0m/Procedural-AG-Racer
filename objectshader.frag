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

uniform Material material;
uniform Light light;

in vec3 wView;				

out vec4 fragmentColor;
	
void main() {
	vec3 xTangent = dFdx(wView);
	vec3 yTangent = dFdy(wView);
	vec3 N = normalize(cross(xTangent, yTangent)); // Surface normal
	vec3 V = normalize(wView);

	vec3 texColor = vec3(78, 162, 177) / 255.0;

	vec3 ka = material.ka * texColor;
	vec3 kd = material.kd * texColor;
	vec3 ks = material.ks;

    // Directional light: constant direction
    vec3 L = normalize(light.dir);      // direction TOWARD surface
    vec3 H = normalize(L + V);
    float cost = max(dot(N, L), 0.0);
    float cosd = max(dot(N, H), 0.0);
    vec3 radiance = ka * light.La + (kd * cost + ks * pow(cosd, material.shininess)) * light.Le;

	fragmentColor = vec4(radiance, 1.0);
}