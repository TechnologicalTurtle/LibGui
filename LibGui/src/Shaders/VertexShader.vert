#version 330 core

uniform vec2 u_ParentPos;
uniform vec2 u_Scale;
uniform vec2 u_UpVector;
uniform vec2 u_RightVector;
uniform vec4 u_Color;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec4 aColor;

out vec4 fColor;
out vec2 fTexCoord;

void main()
{
    fColor = aColor * u_Color;
    fTexCoord = aTexCoord;

    // position transformations
    vec2 pos = aPos;
    
    // scale
    pos *= u_Scale;

    // direction
    pos = (pos.x*u_RightVector) + (pos.y*u_UpVector);
    
    // position
    pos += vec2(u_ParentPos.x, -u_ParentPos.y);
    
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}