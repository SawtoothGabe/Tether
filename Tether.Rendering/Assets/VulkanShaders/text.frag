#version 450

layout(location = 0) in vec2 fragCoord;
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform TextInfo
{
    vec2 position;
    vec2 scale;
    vec4 color;
} text;

layout(binding = 0) uniform sampler2D image;

void main() 
{
    outColor = text.color * texture(image, fragCoord).r;
}