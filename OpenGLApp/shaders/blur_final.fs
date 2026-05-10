#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D blur;

void main()
{             
    const float gamma = 2.2;
    vec3 sceneColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(blur, TexCoords).rgb;
    sceneColor += bloomColor; // additive blending
    FragColor = vec4(sceneColor, 1.0);
}