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

in float waterDepth;
in float wDist;
in vec3 wView;		
in vec3 vtxPos;

out vec4 fragmentColor;


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
	float spec = (NdotL > 0.0 && NdotV > 0.0) ? pow(NdotH, material.shininess) : 0.0;

	vec3 texColor = waterColor.xyz;

	// Foam
	vec3 foamColor = vec3(1.0);
	if(waterDepth > -0.5) {
		texColor = foamColor;
	}

    vec3 direct  = (material.kd * texColor * NdotL + material.ks * spec) * u_lightLe.xyz;
    vec3 ambient = material.ka * texColor * u_lightLa.xyz;

    vec3 radiance = ambient + direct;

    // Base sky gradient
    float t = clamp(V.y*0.5 + 0.5, 0.0, 1.0);
    vec4 skyCol = mix(skyColor, atmosphereColor, t);

	// Fog
    float fogFactor = exp(-fogDensity * wDist * wDist);
    vec3 finalColor = mix(skyCol.xyz, radiance, fogFactor);

    fragmentColor = vec4(finalColor, 0.7);
}