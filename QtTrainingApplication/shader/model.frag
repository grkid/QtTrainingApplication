#version 330 core
#extension GL_NV_shadow_samplers_cube:enable
out vec4 FragColor;
//layout(location = 0) out vec3 color;

in vec2 TexCoords;
in vec3 FragPos;
in vec2 screenPos; //屏幕坐标
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

uniform samplerCube texture_background;
uniform sampler2D texture_background_flat;
in vec2 vN;

uniform float nearPlane;
uniform float farPlane;

uniform sampler2D depthMap;

//flags
uniform int haveTexture;    //是否有纹理
uniform int haveHeight; //是否有法线贴图
uniform int haveSpecular; //是否有亮面贴图
uniform int haveDemo;  //是否开启演示模式
uniform int haveBackground; //是否有背景，决定是否开启环境映射
uniform int haveFloor;
uniform int haveFloorTransparent;
//在演示模式下，floor会变得完全透明并显示出黑色的阴影，在非演示模式下，floor会是白色

uniform float shadowIntensity;

vec2 stride=1.0/textureSize(depthMap, 0);
//float stride=1.0/2000.0;
const float environmentMappingRatio=0.25;

mat3 shadowFilter=mat3(
    0.0947416,0.118318,0.0947416,
    0.118318,0.147761,0.118318,
    0.0947416,0.118318,0.0947416
);

//使用简单滤波器+偏移的shadow map，也就是PCF
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
    float bias = 0.005;
    return (currentDepth-bias>closestDepth)?(1.0):(0.0);
        
}

float linearizeDepth(float depth){
    float z = (2.0*nearPlane*farPlane)/(farPlane+nearPlane-depth*(farPlane-nearPlane));
    return (z-nearPlane)/(farPlane-nearPlane);
}

//VSM
float varianceShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords=projCoords*0.5+0.5;//归一化到 [0,1]
    float depth=projCoords.z;

    vec2 uv=projCoords.xy;
    vec2 varianceData=texture(depthMap,uv).rg;

    float var=varianceData.g - varianceData.r*varianceData.r;
    if(depth <= varianceData.r){
        return 0.0;
    }
    else{
        return 1-var/(var+pow(depth-varianceData.r, 2.0));
    }
}

vec3 blinnPhongDiffuse(vec3 lightDir,vec3 normal,vec3 diffuseColor)
{
    float diff=max(dot(lightDir,normal),0.0);
    vec3 diffuse=diff*diffuseColor*dLight.diffuse;
    return diffuse;
}

vec3 phongSpecular(vec3 viewPos,vec3 FragPos,vec3 lightDir,vec3 normal,vec3 tex)
{
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular=dLight.specular*spec*tex;
    return specular;
}

vec3 blinnPhongSpecular(vec3 viewPos,vec3 FragPos,vec3 lightDir,vec3 normal,vec3 tex)
{
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 reflectDir=reflect(-lightDir,normal);
    vec3 halfwayDir=normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
    vec3 specular=dLight.specular*spec*tex;
    return specular;
}

vec3 PBRSpecular(vec3 viewPos,vec3 FragPos,vec3 lightDir,vec3 normal,vec3 tex)
{
    vec3 viewDir=normalize(viewPos-FragPos);
    vec3 halfwayDir=normalize(lightDir+viewDir);
    vec3 reflectDir=reflect(-lightDir,normal);

    float spec;
    vec3 specular;
        
    float alpha=dLight.specular.r;    //固定，可以加调整 TODO
    alpha=alpha*alpha;
    float pi=acos(-1.0);
    float nh=dot(normal, halfwayDir);
    float D=alpha/(pi*(nh*nh*(alpha-1)+1)*(nh*nh*(alpha-1)+1));
    spec=D;
    specular=spec*tex;
    return specular;
}

vec3 myClamp(vec3 a)
{
    vec3 b;
    for(int i=0;i<3;i++)
    {
        b[i]=a[i];
        if(a[i]<0)  b[i]=0;
        if(a[i]>1) b[i]=1;
    }
    return b;
}

 
void main()
{
    if(haveTexture==1){
        vec3 ambient=dLight.ambient*vec3(texture(texture_ambient1,TexCoords).rgb);

        // diffuse
        vec3 diffuseColor=texture(texture_diffuse1,TexCoords).rgb;

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
        vec3 diffuse=blinnPhongDiffuse(lightDir,normal,diffuseColor);

        //specular
        vec3 specular;
        if(haveSpecular==1)
            specular=PBRSpecular(viewPos,FragPos,lightDir,normal,texture(texture_specular1,TexCoords).rgb);
        else
            specular=PBRSpecular(viewPos,FragPos,lightDir,normal,texture(texture_diffuse1,TexCoords).rgb);

        //环境映射
        vec3 environementBase=textureCube(texture_background,reflect(viewPos,normal)).rgb;

        //result
        float shadow;
        shadow=shadowIntensity*varianceShadowCalculation(FragPosLightSpace);
        vec3 result=ambient+(1-shadow)*(diffuse+specular);
        if(haveBackground==1){
            //result+=environementBase;
        }
        FragColor=vec4(result,1.0);
    }
    else //if (haveTexture==0)
    {
        vec3 whiteColor=vec3(1.0,1.0,1.0);
        if(haveFloor==1 && haveFloorTransparent==1)
            whiteColor=vec3(0.6,0.6,0.6);
        vec3 ambient=dLight.ambient*whiteColor;

        // diffuse
        vec3 diffuseColor=whiteColor;

        // vec3 normal=texture(texture_height1,TexCoords).rgb;
        // normal=normalize(normal*2.0-1.0);
        vec3 normal=normalize(Normal);

        vec3 lightDir=normalize(dLight.direction);
        float diff=max(dot(lightDir,normal),0.0);
        vec3 diffuse=blinnPhongDiffuse(lightDir,normal,diffuseColor);

        //PBR高光
        //vec3 viewPos,vec3 FragPos,vec3 lightDir,vec3 normal,vec3 tex

        vec3 specular=PBRSpecular(viewPos,FragPos,lightDir,normal,whiteColor);
        if(haveDemo==1)
            specular=blinnPhongSpecular(viewPos,FragPos,lightDir,normal,whiteColor);
        else if(haveDemo==0)
            specular=phongSpecular(viewPos,FragPos,lightDir,normal,whiteColor);

        //environment mapping
        vec3 environementBase=textureCube(texture_background,reflect(viewPos,normal)).rgb;


        //result
        float shadow;
//        if(haveDemo==1)
//            shadow=directShadowCalculation(FragPosLightSpace);
//        else
//            shadow=filterShadowCalculation(FragPosLightSpace);
        shadow=shadowIntensity*varianceShadowCalculation(FragPosLightSpace);
//        if(haveDemo==1 && haveFloor==0){
////            if(dot(normal,lightDir)<0)
////                shadow=0.0;
//            float sigmoid=1.0/(1+exp(-50.0*dot(normal,lightDir)));
//            shadow*=sigmoid;
//        }
        vec3 result=ambient+(1-shadow)*(diffuse+specular);
        if(haveBackground==1){
            //result+=environmentMappingRatio*environementBase;
            if(haveDemo==1 && haveFloor==0)
                result=environementBase+specular;
        }
        float fragAlpha=0.00;
        fragAlpha=shadow;
        if(haveFloor==1 && haveFloorTransparent==1)
        {
            if(haveBackground==0)
                FragColor=vec4(result,fragAlpha);
            else
            {
                FragColor=vec4(0.0,0.0,0.0,fragAlpha);
            }
        }
        else
        {
            FragColor=vec4(result,1.0);
        }
    }

}

/*
如果不透明度为1：1*上+0*下
否则：上*下

GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
在不透明度为1时 全是上
在不透明度不为1时 上=0 0*alpha+下*(1-alpha)
*/