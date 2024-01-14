#version 330 core

const vec2 triangle_vertices[3] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 3.0, -1.0),
    vec2(-1.0,  3.0)
);

const vec2 triangle_uvs[3] = vec2[](
    vec2(0.0, 0.0),
    vec2(2.0, 0.0),
    vec2(0.0, 2.0)
);

out vec2 uv;

void main() {
    gl_Position = vec4(triangle_vertices[gl_VertexID], 0.0, 1.0);
    uv = triangle_uvs[gl_VertexID];
}