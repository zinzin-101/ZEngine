#version 330 core
out vec4 BackgroundColor;
out vec4 ForegroundColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform sampler2D backgroundBlur;
uniform sampler2D foregroundBlur;


void main()
{             
    //BackgroundColor = vec4(texture(image, TexCoords).rgb * texture(backgroundBlur, TexCoords).rgb, 1.0);
    //ForegroundColor = vec4(texture(image, TexCoords).rgb * texture(foregroundBlur, TexCoords).rgb, 1.0);

    BackgroundColor = vec4((texture(backgroundBlur, TexCoords).r), 0.0, 0.0, 1.0);
    ForegroundColor = vec4(0.0, (texture(foregroundBlur, TexCoords).r), 0.0, 1.0);

}