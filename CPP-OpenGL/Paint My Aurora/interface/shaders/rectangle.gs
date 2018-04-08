#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

uniform float ratio;

void main() {
    vec4 fragPos = gl_in[0].gl_Position;
    vec2 fragCoord = fragPos.xy / fragPos.w;
    vec2 offsets[] = vec2[] (
        vec2( 1, 1), vec2(-1,  1), vec2(-1, -1),
        vec2( 1, 1), vec2(-1, -1), vec2( 1, -1)
    );
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            vec2 offset = offsets[3 * i + j] * 0.01;
            offset.y *= ratio;
            gl_Position = vec4(fragCoord + offset, 0.1, 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
