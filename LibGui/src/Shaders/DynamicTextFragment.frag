#version 330 core
out vec4 FragColor;

uniform sampler2D myTexture;
uniform vec2 u_EdgeMin;
uniform vec2 u_EdgeMax;

in vec4 fTextColor;
in vec4 fBackColor;
in vec2 fTexCoord;

void main()
{
    if (
        gl_FragCoord.x >= u_EdgeMin.x &&
        gl_FragCoord.y >= u_EdgeMin.y &&
        gl_FragCoord.x <= u_EdgeMax.x &&
        gl_FragCoord.y <= u_EdgeMax.y
    )
    {
        float alpha = texture(myTexture, fTexCoord).a;
        FragColor = (fTextColor * alpha) + (fBackColor * (1 - alpha));
    }
}