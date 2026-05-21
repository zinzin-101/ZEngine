#version 330 core
out vec4 FragColor;
out vec4 BlurColor;
out vec4 DepthColor;

in vec2 TexCoords;

uniform sampler2D albedoMap1;
uniform bool useDepthOfField;
uniform float farPlane;
uniform float nearPlane;

float LinearizeDepth(float depth) {
    float far_plane = farPlane;
    float near_plane = nearPlane;

    float z = depth * 2.0 - 1.0; // Back to NDC 
    float linearDepth = (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
    return linearDepth / far_plane;	
}

void main()
{    
    //FragColor = vec4(TexCoords, 0.0, 1.0); // red=U, green=V
    FragColor = texture(albedoMap1, TexCoords);

    if (!useDepthOfField) return;

    float depth = LinearizeDepth(gl_FragCoord.z);
    BlurColor = FragColor;
    DepthColor = vec4(vec3(depth), 1.0);

    //FragColor = vec4(1.0f);
}
