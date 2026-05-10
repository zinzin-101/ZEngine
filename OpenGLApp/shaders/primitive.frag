#version 430 core

out vec4 FragColor;
out vec4 BlurColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;
uniform vec3 camPos;

vec3 lightDir = normalize(vec3(-0.0, -1.0, -0.0));

float shininess = 48;
vec3 ambient = vec3(0.3);
vec3 diffuse = vec3(0.9);
vec3 specular = vec3(0.6);

uniform bool useDepthOfField;

vec3 CalcDirLight(vec3 lightDir, vec3 normal, vec3 viewDir);
float LinearizeDepth(float depth);

void main() {
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 result = CalcDirLight(lightDir, normalize(Normal), viewDir);

    FragColor = vec4(result, 1.0);
    if (!useDepthOfField) return;

    float depth = LinearizeDepth(gl_FragCoord.z);
    BlurColor = vec4(result, 1.0);
    if (depth <= 0.5){
        BlurColor = vec4(vec3(0.0), 1.0);
    }
    else{
        FragColor = vec4(vec3(0.0), 1.0);
    }

    //FragColor = vec4(color, 1.0);
}

float LinearizeDepth(float depth) {
    float far_plane = 1.0;
    float near_plane = 0.01;

    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

vec3 CalcDirLight(vec3 lightDirection, vec3 normal, vec3 viewDirection) {
    vec3 L = normalize(lightDirection);
    // diffuse shading
    float diff = max(dot(normal, L), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-L, normal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), shininess);
    // combine results
    vec3 _ambient = ambient * color;
    vec3 _diffuse = diffuse * diff * color;
    vec3 _specular = specular * spec * color;
    return (_ambient + _diffuse + _specular);
}