#version 330 core
out vec2 FragColor;

uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float depth){
    float z = depth*2.0-1.0;
    return (2.0*nearPlane*farPlane)/(farPlane+nearPlane-z*(farPlane-nearPlane));
}

void main()
{
    float depth = gl_FragCoord.z;
    //depth�ķֲ���
    //�����[-1,1] ֮��ķֲ��� [0,1]
    //�����[0,1] ֮��ķֲ��� [0.5,1]
    depth=depth*0.5+0.5;

    if(depth>0.5)
    {
        FragColor.x = 0;
        FragColor.y = 0;
    }
    FragColor.x = depth;
    FragColor.y = depth*depth;
}