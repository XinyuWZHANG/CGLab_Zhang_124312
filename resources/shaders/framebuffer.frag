#version 150

out vec4 out_Color;

uniform sampler2D framebufferTexture;
void main() {
    out_Color = texture(framebufferTexture, gl_FragCoord.xy / vec2(800.0, 600.0));
}