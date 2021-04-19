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

uniform sampler2D texture_background;
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
//在演示模式下，floor会变得完全透明并显示出黑色的阴影，在非演示模式下，floor会是白色

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
    return (currentDepth>closestDepth)?(1.0):(0.0);
        
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

    //减少light bleeding部分
//    int filterRadius=10;
//    int inShadowNum=0;
//    const int inShadow=6;
//    vec2 tex_offset = 1.0 / textureSize(depthMap, 0);
//    vec2 nearbyMax=vec2(0.0,0.0);
//    for(int i=-1;i<=1;i++)
//    {
//        for(int j=-1;j<=1;j++)
//        {
//            if(i==0 && j==0)
//                break;
//            vec2 uv2=uv+vec2(i*filterRadius*tex_offset.x,j*filterRadius*tex_offset.y);
//            vec2 temp=texture(depthMap,uv2).rg;
//            if(depth<=temp.r){
//                inShadowNum++;
//                if(temp.r>nearbyMax.r)
//                    nearbyMax=temp;
//            }
//        }
//    }
//
//    bool IS=inShadowNum >= inShadow;

    float var=varianceData.g - varianceData.r*varianceData.r;
    if(depth <= varianceData.r){
        return 0.0;
    }
    else{
//        if(IS)
//        {
//            if(haveDemo==0){
//                varianceData=nearbyMax;
//                float var=varianceData.g - varianceData.r*varianceData.r;
//            }
//        }
        return 1-var/(var+pow(depth-varianceData.r, 2.0));
    }
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
            if(haveDemo==1)
            {
                normal=normalize(Normal);
            }
        }

        vec3 lightDir=normalize(dLight.direction);
        float diff=max(dot(lightDir,normal),0.0);
        vec3 diffuse=diff*diffuseColor*dLight.diffuse;

        //specular
        vec3 viewDir=normalize(viewPos-FragPos);
        vec3 halfwayDir=normalize(lightDir+viewDir);
        //Blinn-Phong
        float spec;
        vec3 specular;
//        if(haveDemo==1){
//            spec=pow(max(dot(normal, halfwayDir), 0.0), 64);
//            specular=dLight.specular*spec*vec3(texture(texture_specular1, TexCoords));
//        }
//        else{
//            float alpha=texture(texture_specular1, TexCoords).r;
//            alpha=alpha*alpha;
//            float pi=acos(-1.0);
//            float nh=dot(normal, halfwayDir);
//            float D=alpha/(pi*(nh*nh*(alpha-1)+1)*(nh*nh*(alpha-1)+1));
//            spec=D;
//            specular=dLight.specular*spec*vec3(texture(texture_specular1, TexCoords));
//
//        }


        //基于PBR的高光算法
        
        float alpha=dLight.specular.r;    //固定，可以加调整 TODO
        alpha=alpha*alpha;
        float pi=acos(-1.0);
        float nh=dot(normal, halfwayDir);
        float D=alpha/(pi*(nh*nh*(alpha-1)+1)*(nh*nh*(alpha-1)+1));
        spec=D;
        if(haveSpecular==1)
            specular=spec*vec3(texture(texture_specular1, TexCoords));
        else
            specular=spec*vec3(texture(texture_diffuse1, TexCoords));

        //环境映射
        vec3 environementBase=texture2D(texture_background,vN).rgb;

        //result
        float shadow=varianceShadowCalculation(FragPosLightSpace);
        vec3 result=ambient+(1-shadow)*(diffuse+specular);
        if(haveBackground==1){
            result+=environmentMappingRatio*environementBase;
        }
        FragColor=vec4(result,1.0);
    }
    else //if (haveTexture==0)
    {
        vec3 whiteColor=vec3(1.0,1.0,1.0);
        if(haveDemo==1) {
            whiteColor=dLight.ambient;
        }
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

        //environment mapping
        vec3 environementBase=texture2D(texture_background,vN).rgb;


        //result
        float shadow=varianceShadowCalculation(FragPosLightSpace);
        vec3 result=ambient+(1-shadow)*(diffuse+specular);
        if(haveBackground==1){
            result+=environmentMappingRatio*environementBase;
        }
        float alpha=0.00;
        alpha=shadow;
        if(haveDemo==1)
            FragColor=vec4(result,alpha);
        else
            FragColor=vec4(result,1.0);
    }

}