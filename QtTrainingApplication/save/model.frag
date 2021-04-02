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
    // ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // �任��[0,1]�ķ�Χ
    projCoords = projCoords * 0.5 + 0.5;
    // ȡ�����������(ʹ��[0,1]��Χ�µ�fragPosLight������)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // ȡ�õ�ǰƬ���ڹ�Դ�ӽ��µ����
    float currentDepth = projCoords.z;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
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