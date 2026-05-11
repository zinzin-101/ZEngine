#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform samepler2D blur;


void main()
{             
    FragColor = vec4(texture(image, TexCoords).rgb * texture(blur, TexCoords).rgb, 1.0)   
}