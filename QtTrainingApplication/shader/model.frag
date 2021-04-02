#version 330 core
out vec4 FragColor;
//layout(location = 0) out vec3 color;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;
in mat3 TBN;

uniform vec3 viewPos;

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight dLight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_ambient1;
uniform sampler2D texture_height1;

uniform float nearPlane;
uniform float farPlane;

uniform sampler2D depthMap;

//flags
uniform int haveTexture;    //是否有纹理
uniform int haveHeight; //是否有法线贴图

vec2 stride=1.0/textureSize(depthMap, 0);
//float stride=1.0/2000.0;

mat3 shadowFilter=mat3(
    0.0947416,0.118318,0.0947416,
    0.118318,0.147761,0.118318,
    0.0947416,0.118318,0.0947416
);

//使用简单滤波器+偏移的shadow map
float filterShadowCalculation(vec4 fragPosLightSpace)
{
    //add a filter to avoid Aliasing
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float shadow=0;
    float bias=max(0.05 * (1.0 - dot(normalize(Normal), dLight.direction)), 0.005);
    //float bias=0.0;
    //filter
    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            vec2 textureCoord=projCoords.xy;
            textureCoord.x+=i*stride.x;
            textureCoord.y+=j*stride.y;
            float closestDepth = texture(depthMap, textureCoord).x; 
            shadow += (currentDepth-bias > closestDepth  ? 1.0 : 0.0)*shadowFilter[1+i][1+j];
        }
    }
    return shadow;
}

//直接采样的shadow map
float directShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float closestDepth=texture(depthMap,projCoords.xy).x;
    return (currentDepth>closestDepth)?(1.0):(0.0);
        
}


float computeMsmShadowIntensity(vec4 b, float fragment_depth) {
    
    // OpenGL 4 only - fma has higher precision:
    // float l32_d22 = fma(-b.x, b.y, b.z); // a * b + c
    // float d22 = fma(-b.x, b.x, b.y);     // a * b + c
    // float squared_depth_variance = fma(-b.x, b.y, b.z); // a * b + c
    
    float l32_d22 = -b.x * b.y + b.z;
    float d22 = -b.x *  b.x + b.y;
    float squared_depth_variance = -b.x * b.y + b.z;
    
    float d33_d22 = dot(vec2(squared_depth_variance, -l32_d22), vec2(d22, l32_d22));
    float inv_d22 = 1.0 - d22;
    float l32 = l32_d22 * inv_d22;

    float z_zero = fragment_depth;
    vec3 c = vec3(1.0, z_zero - b.x, z_zero * z_zero);
    c.z -= b.y + l32 * c.y;
    c.y *= inv_d22;
    c.z *= d22 / d33_d22;
    c.y -= l32 * c.z;
    c.x -= dot(c.yz, b.xy);
    
    float inv_c2 = 1.0 / c.z;
    float p = c.y * inv_c2;
    float q = c.x * inv_c2;
    float r = sqrt((p * p * 0.25) - q);

    float z_one = -p * 0.5 - r;
    float z_two = -p * 0.5 + r;
    
    vec4 switch_msm;
    if (z_two < z_zero) {
        switch_msm = vec4(z_one, z_zero, 1.0, 1.0);
    } else {
        if (z_one < z_zero) {
            switch_msm = vec4(z_zero, z_one, 0.0, 1.0);
        } else {
            switch_msm = vec4(0.0);
        }
    }
    
    float quotient = (switch_msm.x * z_two - b.x * (switch_msm.x + z_two + b.y)) / ((z_two - switch_msm.y) * (z_zero - z_one));
    return clamp(switch_msm.y + switch_msm.z * quotient ,0.0,1.0);
}

float momentShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;//归一化到 [0,1]
    float currentDepth = projCoords.z;
    float b=texture(depthMap,projCoords.xy).x;
    return computeMsmShadowIntensity(vec4(b,b*b,b*b*b,b*b*b*b),currentDepth);
}
float linearizeDepth(float depth){
    float z = (2.0*nearPlane*farPlane)/(farPlane+nearPlane-depth*(farPlane-nearPlane));
    return (z-nearPlane)/(farPlane-nearPlane);
}

float varianceShadowCalculation(vec4 fragPosLightSpace)
{
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    projCoords=projCoords*0.5+0.5;//归一化到 [0,1]
//    float depth=projCoords.z;
//
//    vec2 uv=projCoords.xy;
//    vec2 varianceData=texture(depthMap,uv).rg;
//    float var=varianceData.g - varianceData.r*varianceData.r;
//    if(depth - 0.001 <= varianceData.r){
//        return 0.0;
//    }
//    else{
//        return 1-var/(var+pow(depth-varianceData.r, 2.0));
//    }

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float closestDepth=texture(depthMap,projCoords.xy).x;
    return (currentDepth>closestDepth)?(1.0):(0.0);
}

 
void main()
{
    // ambient
    if(haveTexture==1){
        vec3 ambient=dLight.ambient*vec3(texture(texture_ambient1,TexCoords).rgb);

        // diffuse
        vec3 diffuseColor=texture(texture_diffuse1,TexCoords).rgb;

        // vec3 normal=texture(texture_height1,TexCoords).rgb;
        // normal=normalize(normal*2.0-1.0);
        vec3 normal;
        if(haveHeight==0)
        {
            normal=normalize(Normal);
        }
        else    //if (haveHeight==1)
        {
            normal=texture(texture_height1,TexCoords).rgb;
            normal=normalize(normal*2.0-1.0);
            normal=normalize(TBN*normal);
        }

        vec3 lightDir=normalize(dLight.direction);
        float diff=max(dot(lightDir,normal),0.0);
        vec3 diffuse=diff*diffuseColor*dLight.diffuse;

        //specular
        vec3 viewDir=normalize(viewPos-FragPos);
        vec3 halfwayDir=normalize(lightDir+viewDir);
        //vec3 reflectDir=reflect(-lightDir,normal);
        //Blinn-Phong
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
        vec3 specular=dLight.specular*spec*vec3(texture(texture_specular1, TexCoords));

        //result
        float shadow=varianceShadowCalculation(FragPosLightSpace);
        //vec3 result=ambient+(1-shadow)*(diffuse+specular);
        //vec3 result=ambient;
        vec3 result=(1-shadow)*(ambient+diffuse+specular);
        FragColor=vec4(result,1.0);
        if(shadow>1.0 || shadow<0.0)
        {
            //debug
            FragColor=vec4(1.0,0.0,0.0,1.0);
        }
        //color=result;
    }
    else //if (haveTexture==0)
    {
        vec3 whiteColor=vec3(0.0,0.0,0.0);
        vec3 ambient=dLight.ambient*whiteColor;

        // diffuse
        vec3 diffuseColor=whiteColor;

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
        vec3 specular=dLight.specular*spec*whiteColor;

        //result
        float shadow=momentShadowCalculation(FragPosLightSpace);
        vec3 result=ambient+(1-shadow)*(diffuse+specular);
        float alpha=0.05;
        alpha=shadow;
        FragColor=vec4(result,alpha);
    }

}