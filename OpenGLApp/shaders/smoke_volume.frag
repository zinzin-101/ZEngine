#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BlurColor;

in vec3 uvw;

uniform sampler3D volume;

uniform bool useDepthOfField;

float LinearizeDepth(float depth) {
    float far_plane = 1.0;
    float near_plane = 0.01;

    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main() {
	float density = texture(volume, uvw).r;
	vec3 smokeColor = vec3(1.0, 1.0, 1.0);
	FragColor = vec4(smokeColor, density);
	
	if (!useDepthOfField) return;
	
	float depth = LinearizeDepth(gl_FragCoord.z);
	if (depth > 0.5){
		BlurColor = vec4(smokeColor, density);
	}

	//FragColor = vec4(1.0f);
}