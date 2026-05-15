#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D blur;
uniform sampler2D depth;
uniform float depthPercentage;

void main()
{             
    vec3 sceneColor = texture(scene, TexCoords).rgb;      
    vec3 blurColor = texture(blur, TexCoords).rgb;

    float lower = clamp(depthPercentage - 0.02, 0.0, 1.0);
    float upper = clamp(depthPercentage + 0.02, 0.0, 1.0);

    float z = texture(depth, TexCoords).r;

    if (z >= upper){
        FragColor = vec4(blurColor, 1.0);
        return;
    }

    if (z <= lower){
        FragColor = vec4(sceneColor, 1.0);
        return;
    }

    float t = (z - (lower)) / 0.04;
    vec3 color = mix(sceneColor, blurColor, t);
    
    FragColor = vec4(color, 1.0);
}