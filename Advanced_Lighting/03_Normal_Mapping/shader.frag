#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;    

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool blinn;

void main(){
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;
    // vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * color;
    // diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    // some research work: about relative specular mapping (phong/blinn phong)
    // https://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/
    float spec=0;
    if(blinn){
        vec3 halfway = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfway), 0.0), 64.0);
    }else{
        vec3 reflectDir=reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    }

    vec3 specular = spec * color;
    // attenuation
    // float max_distance = 1.5;
    // float distance = length(light.position - FragPos);
    // float attenuation = 1.0 / (distance);
    //
    // diffuse *= attenuation;
    // specular *= attenuation;

    vec3 lighting = ambient + diffuse + specular;

    FragColor=vec4(lighting, 1.0);
};



