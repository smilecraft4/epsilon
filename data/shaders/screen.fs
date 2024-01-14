#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D canvas;

void main() {
    FragColor = texture(canvas, uv);
}