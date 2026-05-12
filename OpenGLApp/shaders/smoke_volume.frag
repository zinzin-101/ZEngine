#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BlurColor;
layout(location = 2) out vec4 DepthColor;

in vec3 uvw;

uniform sampler3D volume;

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

void main() {
	float density = texture(volume, uvw).r;
	vec3 smokeColor = vec3(1.0, 1.0, 1.0);
	FragColor = vec4(smokeColor, density);
	
	if (!useDepthOfField) return;
	
    float depth = LinearizeDepth(gl_FragCoord.z);
    BlurColor = FragColor;
    //DepthColor = vec4(vec3(depth), 1.0);

	//FragColor = vec4(1.0f);
    //FragColor = vec4(vec3(depth), 1.0);
    //BlurColor = FragColor;

}