#version 450 core
precision highp float;

layout(std430, binding = 0) buffer VertexBuffer {
    uint vertexCount;
    uint _pad0;
    uint _pad1;
    uint _pad2;
    vec4 vertices[];
};
		
uniform vec3 u_camPos_WS;
uniform mat4 u_V, u_P;
uniform mat4 u_lightVP;

out float viewDist_WS;
out vec3 viewDir_WS;
out vec4 lightPos_CS;

// ---------- Main ----------
void main() {
	vec3 vtxPos_WS = vertices[gl_VertexID].xyz;
	gl_Position = u_P * u_V * vec4(vtxPos_WS, 1);

	viewDir_WS  = u_camPos_WS - vtxPos_WS.xyz;
	viewDist_WS = length(viewDir_WS);
    lightPos_CS = u_lightVP * vec4(vtxPos_WS, 1.0);
}
