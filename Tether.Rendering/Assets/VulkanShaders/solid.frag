#version 450

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Rectangle
{
    vec2 position;
    vec2 scale;
    vec4 color;
} rect;

void main() 
{
    outColor = rect.color;
}