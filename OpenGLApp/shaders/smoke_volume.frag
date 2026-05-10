#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BlurColor;

in vec3 uvw;

uniform sampler3D volume;

uniform bool useDepthOfField;

void main() {
	float density = texture(volume, uvw).r;
	vec3 smokeColor = vec3(1.0, 1.0, 1.0);
	FragColor = vec4(smokeColor, density);
	
	if (!useDepthOfField) return;
	
	BlurColor = vec4(smokeColor, density);

	float depth = LinearizeDepth(gl_FragCoord.z);
	if (depth <= 0.5){
        BlurColor = vec4(vec3(0.0), 1.0)
    }
    else{
        FragColor = vec4(vec3(0.0), 1.0)
    }

	//FragColor = vec4(1.0f);
}