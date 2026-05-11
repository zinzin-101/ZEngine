#version 330 core
out vec4 FragColor;
out vec4 ForegroundBlur;
out vec4 BackgroundBlur;
out vec4 DepthColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform bool useDepthOfField;
uniform float farPlane;
uniform float nearPlane;
uniform float depthPercentage;

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

    // float depth = LinearizeDepth(gl_FragCoord.z);
    // BlurColor = vec4(envColor, 1.0);
    // if (depth <= depthPercentage){
    //     BlurColor = vec4(vec3(0.0), 1.0);
    // }
    // else{
    //     FragColor = vec4(vec3(0.0), 1.0);
    // }
    float depth = LinearizeDepth(gl_FragCoord.z);
    DepthColor = vec4(vec3(depth), 1.0);
    if (depth <= depthPercentage){
        float blur = ((depth - 0.9) / (0.9 - depthPercentage)) + 1.0;
        ForegroundBlur = vec4(vec3(blur), 1.0);
        BackgroundBlur = vec4(vec3(0.0), 1.0);
    }
    else if (depth > depthPercentage){
        float blur = ((-depth + 0.1) / (depthPercentage - 0.1)) + 1.0;
        ForegroundBlur = vec4(vec3(0.0), 1.0);
        BackgroundBlur = vec4(vec3(blur), 1.0);
    }
}
