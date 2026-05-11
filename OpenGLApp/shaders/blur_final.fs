#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D background;
uniform sampler2D foreground;

void main()
{             
    vec3 backgroundColor = texture(background, TexCoords).rgb;
    vec3 foregroundColor = texture(foreground, TexCoords).rgb;      
    vec3 final = foregroundColor;
    FragColor = vec4(final, 1.0);
}