#version 330 core
out vec4 FragColor;

uniform sampler2D myTexture;
uniform bool u_HasTexture;

in vec4 fColor;
in vec2 fTexCoord;

void main()
{
    if (u_HasTexture)
        FragColor = texture(myTexture, fTexCoord) * fColor;
    else
        FragColor = fColor;
} 