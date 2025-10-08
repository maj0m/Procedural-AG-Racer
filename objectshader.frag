#version 450 core
precision highp float;

struct Material {
	vec3 kd, ks, ka;
	float shininess;
};

layout(std140, binding = 2) uniform Lighting {
    vec4 u_lightDir;
    vec4 u_lightLa;
    vec4 u_lightLe;
};

uniform Material material;

in vec3 wView;				

out vec4 fragmentColor;
	
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
	float spec = pow(NdotH, material.shininess);

	vec3 texColor = vec3(78, 162, 177) / 255.0;

	vec3 direct  = (material.kd * texColor * NdotL + material.ks * spec) * u_lightLe.xyz;
    vec3 ambient = material.ka * texColor * u_lightLa.xyz;
    vec3 radiance = ambient + direct;

	fragmentColor = vec4(radiance, 1.0);
}