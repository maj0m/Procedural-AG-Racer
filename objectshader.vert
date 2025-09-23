#version 450 core
precision highp float;

struct Light {
	vec3 La, Le;
	vec4 wLightPos;
};

layout(location = 0) in vec3 vtxPos;   // Originally in modeling space, now directly using NDC
layout(location = 1) in vec2 vtxUV;

uniform mat4 MVP, M;					// MVP, Model
uniform Light[8] lights;				// Light sources 
uniform int nLights;
uniform vec3 wEye;						// Eye position

out vec3 wView;
out vec3 wLight[8];	

void main() {
	gl_Position = vec4(vtxPos, 1.0) * MVP;

	vec4 wPos = vec4(vtxPos, 1) * M;
	for(int i = 0; i < nLights; i++) {
		wLight[i] = lights[i].wLightPos.xyz - wPos.xyz * lights[i].wLightPos.w;
	}
	wView  = wEye - wPos.xyz;
}