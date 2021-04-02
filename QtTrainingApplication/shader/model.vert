#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
 
out vec2 TexCoords; //纹理坐标
out vec3 FragPos;   //世界坐标
out vec3 Normal;    //法向量
out vec4 FragPosLightSpace; //光空间世界坐标
out mat3 TBN;   //TBN矩阵，用于法线贴图

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

//floor don't need to go through matrix
//uniform int isFloor;
 
void main()
{
    mat4 newModel=transform*model;
    TexCoords = aTexCoords;
    gl_Position = projection * view *newModel * vec4(aPos, 1.0);
    FragPos=vec3(newModel*vec4(aPos,1.0));

    mat3 normalMat=transpose(inverse(mat3(newModel)));
    Normal=normalMat*aNormal;

    FragPosLightSpace=lightSpaceMatrix * transform*model * vec4(aPos, 1.0f);

    vec3 T=normalize(vec3(newModel*vec4(aTangent,0.0)));
    vec3 B=normalize(vec3(newModel*vec4(aBitangent,0.0)));
    vec3 N=normalize(vec3(newModel*vec4(aNormal,0.0)));
    TBN=mat3(T,B,N);
}