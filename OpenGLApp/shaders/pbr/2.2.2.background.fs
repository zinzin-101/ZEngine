#version 330 core
out vec4 FragColor;
out vec4 BlurColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
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
    vec3 envColor = textureLod(environmentMap, WorldPos, 0.0).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
    
    FragColor = vec4(envColor, 1.0);

    if (!useDepthOfField) return;

    float depth = LinearizeDepth(gl_FragCoord.z);
    BlurColor = vec4(envColor, 1.0);
    if (depth <= 0.5){
        BlurColor = vec4(vec3(0.0), 1.0);
    }
    else{
        FragColor = vec4(vec3(0.0), 1.0);
    }

}
