#version 450 core
precision highp float;

struct Light {
	vec3 La, Le;
	vec3 dir;
};

layout(std430, binding = 0) buffer VertexBuffer {
    uint vertexCount;   // counts vertices written (multiple of 3)
    uint _pad0;         // pad to 16 bytes for std430 alignment
    uint _pad1;
    uint _pad2;
    vec4 vertices[];    // payload starts at offset 16
};
		
uniform mat4 MVP, M;					// MVP, Model
uniform vec3 wEye;						// Eye position

out vec3 wView;							// view in world space
out float wDist;						// distance from camera
out vec3 vtxPos;

// ---------- Main ----------
void main() {
	vec3 vertexPos = vertices[gl_VertexID].xyz;
	gl_Position = vec4(vertexPos, 1) * MVP;			// to NDC

	vec4 wPos = vec4(vertexPos, 1) * M;
	wView  = wEye - wPos.xyz;
	wDist = length(wView);         // Distance from eye to vertex
	vtxPos = vertexPos;
}
