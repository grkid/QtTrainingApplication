#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight dLight;

// textures
// struct myTexture{
//     sampler2D diffuse;
//     sampler2D specular;
//     sampler2D ambient;
//     sampler2D height;
// };
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_ambient1;
uniform sampler2D texture_height1;

uniform sampler2D depthMap;
//uniform myTexture textures;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}
 
void main()
{
    // ambient
    vec3 ambient=dLight.ambient*vec3(texture(texture_ambient1,TexCoords).rgb);

    // diffuse
    vec3 diffuseColor=texture(texture_diffuse1,TexCoords).rgb;

    // vec3 normal=texture(texture_height1,TexCoords).rgb;
    // normal=normalize(normal*2.0-1.0);
    vec3 normal=normalize(Normal);

    vec3 lightDir=normalize(dLight.direction);
    float diff=max(dot(lightDir,normal),0.0);
    vec3 diffuse=diff*diffuseColor*dLight.diffuse;

    //specular
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular=dLight.specular*spec*vec3(texture(texture_specular1, TexCoords));

    //result
    float shadow=shadowCalculation(FragPosLightSpace);
    vec3 result=(ambient+diffuse+specular);
    FragColor=vec4(result,1.0);

}