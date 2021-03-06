#version 330 core

layout(location = 0) in vec2 in_position;

out vec2 vs_rayCoordinates;

void main()
{
    vs_rayCoordinates = in_position;
    gl_Position = vec4(in_position, 0.0, 1.0);    
}