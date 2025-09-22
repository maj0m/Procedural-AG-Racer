#version 450 core
precision highp float;

struct Light {
	vec3 La, Le;
	vec4 wLightPos;
};
		
uniform mat4 MVP, M;					// MVP, Model
uniform Light[8] lights;				// Light sources 
uniform int nLights;
uniform vec3 wEye;						// Eye position

layout(std430, binding = 0) buffer VertexBuffer {
    uint vertexCount;   // counts vertices written (multiple of 3)
    uint _pad0;         // pad to 16 bytes for std430 alignment
    uint _pad1;
    uint _pad2;
    vec4 vertices[];    // payload starts at offset 16
};

out vec3 wView;							// view in world space
out vec3 wLight[8];						// light dir in world space
out float wDist;						// distance from camera
out vec3 vtxPos;


void main() {
	vec3 vertexPos = vertices[gl_VertexID].xyz;
	gl_Position = vec4(vertexPos, 1) * MVP;			// to NDC

	vec4 wPos = vec4(vertexPos, 1) * M;
	for(int i = 0; i < nLights; i++) {
		wLight[i] = lights[i].wLightPos.xyz - wPos.xyz * lights[i].wLightPos.w;
	}
	wView  = wEye - wPos.xyz;
	wDist = length(wView);         // Distance from eye to vertex
	vtxPos = vertexPos;
}
