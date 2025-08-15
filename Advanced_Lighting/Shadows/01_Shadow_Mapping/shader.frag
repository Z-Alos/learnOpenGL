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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir){
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform NDC to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(fs_in.Normal, lightDir)), 0.005);
    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    // PCF
    // if(projCoords.z > 1.0)
    //     shadow = 0.0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x<=1; ++x){
        for(int y = -1; y<=1; ++y){
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main(){
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
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
    float spec=0;
    vec3 viewDir=normalize(viewPos - fs_in.FragPos);
    if(blinn){
        vec3 halfway = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfway), 0.0), 64.0);
    }else{
        vec3 reflectDir=reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
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
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, lightDir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor=vec4(lighting, 1.0);
};



