#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//floor don't need to go through matrix
//uniform int isFloor;
 
void main()
{
    mat4 newModel=transform*model;
    gl_Position = projection * view *newModel * vec4(aPos, 1.0);
}