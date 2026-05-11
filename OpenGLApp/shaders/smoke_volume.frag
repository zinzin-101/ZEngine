#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 ForegroundBlur;
layout(location = 2) out vec4 BackgroundBlur;
layout(location = 3) out vec4 DepthColor;

in vec3 uvw;

uniform sampler3D volume;

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

void main() {
	float density = texture(volume, uvw).r;
	vec3 smokeColor = vec3(1.0, 1.0, 1.0);
	FragColor = vec4(smokeColor, density);
	
	if (!useDepthOfField) return;
	
	//float depth = LinearizeDepth(gl_FragCoord.z);
	//if (depth > depthPercentage){
	//	BlurColor = vec4(smokeColor, density);
	//}

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