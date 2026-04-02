#version 450

layout(location = 0) in vec2 position;
layout(location = 0) out vec2 fragCoord;

layout(push_constant) uniform TextInfo
{
    vec2 position;
    vec2 scale;
    vec4 color;
} text;

void main() 
{
    vec2 vertexPosition = position.xy;
    vertexPosition *= text.scale.xy;
    vertexPosition += text.position.xy;

    vertexPosition *= 2;
    vertexPosition -= 1;

    fragCoord = position.xy;
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}