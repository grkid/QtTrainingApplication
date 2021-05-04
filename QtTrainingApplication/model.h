#pragma once
#include "mesh.h"
#include <QDir>
#include <qfileinfo.h>
#include <QMatrix4x4>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <QVector4D>
#include <QVector3D>

enum modelOperation { modelTranslate, modelRotate, modelScale };
enum modelDirection { tLeft, tRight, tUp, tDown, tFront, tBack, rXa, rXc, rYa, rYc, rZa, rZc, sUp, sDown };

class Model
{
public:
    void draw(QOpenGLShaderProgram* shader);
    void destroy();
    static Model* createModel(QString path, QOpenGLContext* context);
    ~Model();

    //可重入函数
    //void readModel(QString path);

    //获取模型名称
    QString getName();

    //修改模型变换矩阵
    void modelTransform(modelOperation op, modelDirection dir);

    //获取模型中心
    QVector3D getModelCentre();

    //模型变换矩阵
    QMatrix4x4 getTransform();
    void setTransform(QMatrix4x4 t);

private:
    Model(QString path, QOpenGLContext* context);
    QOpenGLContext* context;          //opengl函数入口

    /*  模型数据  */
    QString modelName;  //模型名称
    QVector<Texture*>textures_loaded;       //纹理
    QVector<Mesh*> meshes;                  //网格
    QDir directory;                         //模型所在路径
    QMatrix4x4 transform;
    //QVector3D modelCentre = QVector3D(0, 5, 0);
    float sensitivity = 0.01;

    /*求取模型中心*/
    const float _max = 8192.0;
    const float _min = -8192.0;
    float max_x=_min, min_x=_max;
    float max_y = _min, min_y = _max;
    float max_z = _min, min_z = _max;
    float x_centre = 0, y_centre = 0, z_centre = 0;
    QVector4D centre;
    void processCentre(aiVector3D* v);

    //递归遍历结点
    void processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 mat4 = aiMatrix4x4());

    //加载网格
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 model);

    //加载材质纹理
    QVector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName);

};

/*

COMMON INTERCHANGE FORMATS
    Autodesk(.fbx)
    Collada(.dae)
    glTF(.gltf, .glb)
    Blender 3D(.blend)
    3ds Max 3DS(.3ds)
    3ds Max ASE(.ase)
    Wavefront Object(.obj)
    Industry Foundation Classes(IFC / Step) (.ifc)
    XGL(.xgl, .zgl)
    Stanford Polygon Library(.ply)
    * AutoCAD DXF(.dxf)
    LightWave(.lwo)
    LightWave Scene(.lws)
    Modo(.lxo)
    Stereolithography(.stl)
    DirectX X(.x)
    AC3D(.ac)
    Milkshape 3D(.ms3d)
    * TrueSpace(.cob, .scn)
MOTION CAPTURE FORMATS
    Biovision BVH(.bvh)
    * CharacterStudio Motion(.csm)
    GRAPHICS ENGINE FORMATS
    Ogre XML(.xml)
    Irrlicht Mesh(.irrmesh)
    * Irrlicht Scene(.irr)
    GAME FILE FORMATS
    Quake I(.mdl)
    Quake II(.md2)
    Quake III Mesh(.md3)
    Quake III Map / BSP(.pk3)
    * Return to Castle Wolfenstein(.mdc)
    Doom 3 (.md5*)
    * Valve Model(.smd, .vta)
    * Open Game Engine Exchange(.ogex)
    * Unreal(.3d)
OTHER FILE FORMATS
    BlitzBasic 3D(.b3d)
    Quick3D(.q3d, .q3s)
    Neutral File Format(.nff)
    Sense8 WorldToolKit(.nff)
    Object File Format(.off)
    PovRAY Raw(.raw)
    Terragen Terrain(.ter)
    3D GameStudio(3DGS) (.mdl)
    3D GameStudio(3DGS) Terrain(.hmp)
    Izware Nendo(.ndo)

*/