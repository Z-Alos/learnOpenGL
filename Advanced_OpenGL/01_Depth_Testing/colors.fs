#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular; 
    float shininess;
};

struct Light {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

// NDC
float near = 0.1;
float far = 4000.0;

float LinearizeDepth(float depth){
    float z = depth * 2.0 - 1.0; // back to normalized depth coords
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(){
    // ambient
    // we are setting diffenrent light intesity for different vars
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position-FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    // NDC
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    FragColor=vec4(vec3(depth), 1.0);

    vec3 result=ambient+diffuse+specular;
    //FragColor=vec4(result, 1.0);
    //FragColor=vec4(vec3(gl_FragCoord.z), 1.0);
};



