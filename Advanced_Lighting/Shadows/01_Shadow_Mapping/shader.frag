#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;    

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool blinn;

float ShadowCalculation(vec4 fragPosLightSpace){
}

void main(){
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    // some research work: about relative specular mapping (phong/blinn phong)
    // https://www.rorydriscoll.com/2009/01/25/energy-conservation-in-games/
    float spec;
    vec3 viewDir=normalize(viewPos - fs_in.FragPos);
    if(blinn){
        vec3 halfway = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfway), 0.0), 64.0);
    }else{
        vec3 reflectDir=reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 specular = spec * lightColor;
    // attenuation
    // float max_distance = 1.5;
    // float distance = length(light.position - FragPos);
    // float attenuation = 1.0 / (distance);
    //
    // diffuse *= attenuation;
    // specular *= attenuation;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor=vec4(lighting, 1.0);
};



