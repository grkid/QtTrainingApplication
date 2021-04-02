#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// layout (location = 3) in vec3 aTangent;
// layout (locaiton = 4) in vec3 aBitangent;
 
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;
 
void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view *transform* model * vec4(aPos, 1.0);
    FragPos=vec3(model*vec4(aPos,1.0));

    mat3 normalMat=transpose(inverse(mat3(transform*model)));
    Normal=normalMat*aNormal;

    FragPosLightSpace=lightSpaceMatrix*vec4(FragPos,1.0);
}