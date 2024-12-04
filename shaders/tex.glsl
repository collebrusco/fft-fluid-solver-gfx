#version 410 core

uniform sampler2D uTexslot;
out vec4 outColor;
in vec2 iUV;

void main(){
    vec4 c = texture(uTexslot, iUV);
    // if (c.r != 0.0) {c.r = 1.f;}
    c.z = c.y;
    c.y = 0;
    outColor = c;
}
