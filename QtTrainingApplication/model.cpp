#include "model.h"

Model::Model(QString path, QOpenGLContext* context)
    : context(context)
    , directory(path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(directory.absolutePath().toLocal8Bit(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    modelName = QFileInfo(path).baseName();
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        qDebug() << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    qDebug() << directory.absolutePath() << "load success";
    qDebug() << "mesh:" << scene->mNumMeshes;
    qDebug() << "material:" << scene->mNumMaterials;
    qDebug() << "texture:" << scene->mNumTextures;
    qDebug() << "animation:" << scene->mNumAnimations;
    directory.cdUp();
    processNode(scene->mRootNode, scene);
}

Model::~Model() //���ٶ���
{
    for (auto& it : textures_loaded) {
        it->texture.destroy();
        delete it;
    }
    for (auto& it : meshes) {
        delete it;
    }
}

void Model::draw(QOpenGLShaderProgram* shader) {
    //shaderProgram->bind();
    shader->bind();
    shader->setUniformValue("transform", transform);

    if (modelName.split(" ")[0] == "floor")
    {
        shader->setUniformValue("haveFloor", 1);
    }
    else
    {
        shader->setUniformValue("haveFloor", 0);
    }
    for (Mesh* mesh : meshes) {
        mesh->draw(shader);
    }
}

void Model::destroy()
{
    delete this;
    context->doneCurrent();
}

Model* Model::createModel(QString path, QOpenGLContext* context)
{
    return new Model(path, context);
}

void Model::processCentre(aiVector3D* v)
{
    if (v->x <= min_x) min_x = v->x;
    if (v->x >= max_x) max_x = v->x;
    if (v->y <= min_y) min_y = v->y;
    if (v->y >= max_y)max_y = v->y;
    if (v->z <= min_z) min_z = v->z;
    if (v->z >= max_z) max_z = v->z;
}


void Model::processNode(aiNode* node, const aiScene* scene, aiMatrix4x4 mat4)
{

    // ����ڵ����е���������еĻ���
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, mat4));

    }
    // �������������ӽڵ��ظ���һ����
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, mat4 * node->mChildren[i]->mTransformation);
    }
}
Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4 model)
{
    //  ��ʼ������
    Mesh* m_mesh = new Mesh(context->functions(), model);
    // ���������ÿ������
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        QVector3D vector; //��assimp������ת��ΪQtOpenGL֧�ֵ�����

        // λ��
        processCentre(&mesh->mVertices[i]);
        vector.setX(mesh->mVertices[i].x);
        vector.setY(mesh->mVertices[i].y);
        vector.setZ(mesh->mVertices[i].z);
        vertex.Position = vector;
        // ������
        if (mesh->mNormals) {
            vector.setX(mesh->mNormals[i].x);
            vector.setY(mesh->mNormals[i].y);
            vector.setZ(mesh->mNormals[i].z);
            vertex.Normal = vector;
        }
        // ��������
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            QVector2D vec;
            //һ�����������԰���8����ͬ���������ꡣ������Ǽ������ǲ���
            //ʹ��һ����������ж�����������ģ�ͣ�������������ȡ��һ������(0)��
            vec.setX(mesh->mTextureCoords[0][i].x);
            vec.setY(mesh->mTextureCoords[0][i].y);
            vertex.TexCoords = vec;
        }
        else {
            vertex.TexCoords = QVector2D(0, 0);
        }
        if (mesh->mTangents) {
            // tangent
            vector.setX(mesh->mTangents[i].x);
            vector.setY(mesh->mTangents[i].y);
            vector.setZ(mesh->mTangents[i].z);
            vertex.Tangent = vector;
        }
        if (mesh->mBitangents) {
            vector.setX(mesh->mBitangents[i].x);
            vector.setY(mesh->mBitangents[i].y);
            vector.setZ(mesh->mBitangents[i].z);
            vertex.Bitangent = vector;
        }
        // bitangent
        m_mesh->vertices.push_back(vertex);
    }

    //�õ�ģ��������Ϣ
    x_centre = (max_x + min_x) / 2;
    y_centre = (max_y + min_y) / 2;
    z_centre = (max_z + min_z) / 2;
    centre = QVector4D(x_centre, y_centre, z_centre, 1);

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // �������������������ӵ�����������
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            m_mesh->indices.push_back(face.mIndices[j]);
        }
    }

    // �������
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. ������ͼ
    QVector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    for (auto& it : diffuseMaps)
        m_mesh->textures.push_back(it);

    // 2. ������ͼ
    QVector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    for (auto& it : specularMaps)
        m_mesh->textures.push_back(it);

    // 3. ������ͼ
    QVector<Texture*> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    for (auto& it : heightMaps)
        m_mesh->textures.push_back(it);

    // 4. ������ͼ
    QVector<Texture*> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
    for (auto& it : ambientMaps)
        m_mesh->textures.push_back(it);

    m_mesh->setupMesh();
    return m_mesh;
}

QVector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, QString typeName)
{
    QVector<Texture*> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // ��������Ƿ���֮ǰ���ع�������ǣ����������һ������:��������������
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            //const char* str1 = textures_loaded[j]->path.toStdString().c_str();
            std::string stdstr = textures_loaded[j]->path.toLocal8Bit().data();
           if (std::strcmp(stdstr.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; //���Ż��� ������ͬfilepath�������Ѿ����أ���������һ��
                break;
            }
        }
        if (!skip)
        {   // ������ʻ�û�м��أ�������
            Texture* texture = new Texture;
            QImage data(directory.filePath(str.C_Str()));
            if (!data.isNull()) {
                texture->texture.setData(data);
                texture->type = typeName;
                texture->path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
            else {
                qDebug() << "failed to loda texture:" << directory.filePath(str.C_Str());
            }
        }
    }
    return textures;
}

QString Model::getName()
{
    return modelName;
}

void Model::modelTransform(modelOperation op, modelDirection dir)
{
    if (op == modelTranslate)
    {
        if (dir == tLeft) {
            transform.translate(-sensitivity, 0, 0);
            //modelCentre += QVector3D(-sensitivity, 0, 0);
        }
        else if (dir == tRight) {
            transform.translate(sensitivity, 0, 0);
            //modelCentre += QVector3D(sensitivity, 0, 0);
        }
        else if (dir == tUp) {
            transform.translate(0, sensitivity, 0);
            //modelCentre += QVector3D(0, sensitivity, 0);
        }
        else if (dir == tDown) {
            transform.translate(0, -sensitivity, 0);
            //modelCentre += QVector3D(0, -sensitivity, 0);
        }
        else if (dir == tFront) {
            transform.translate(0, 0, +sensitivity);
            //modelCentre += QVector3D(0, 0,sensitivity);
        }
        else if (dir == tBack) {
            transform.translate(0, 0, -sensitivity);
            //modelCentre += QVector3D(0, 0, -sensitivity);
        }
    }
    else if (op == modelRotate)
    {
        if (dir == rXa)
            transform.rotate(sensitivity*10, 1, 0, 0);
        else if (dir == rXc)
            transform.rotate(-sensitivity*10, 1, 0, 0);
        else if (dir == rYa)
            transform.rotate(sensitivity*10, 0, 1, 0);
        else if (dir == rYc)
            transform.rotate(-sensitivity*10, 0, 1, 0);
        else if (dir == rZa)
            transform.rotate(sensitivity*10, 0, 0, 1);
        else if (dir == rZc)
            transform.rotate(-sensitivity*10, 0, 0, 1);


    }
    else// if (op == modelScale)
    {
        if (dir == sUp)
        {
            transform.scale(1 + sensitivity*0.1);
        }
        else
        {
            transform.scale(1 - sensitivity*0.1);
        }
    }
}

QVector3D Model::getModelCentre()
{
    //return modelCentre;
    QMatrix4x4 model;
    QMatrix4x4 t = transform;
    t *= model;
   /* QVector4D temp=QVector4D(x_centre, y_centre, z_centre, 1);*/
    QVector4D result = t * centre;
    return result.toVector3D();
}

QMatrix4x4 Model::getTransform()
{
    return transform;
}

void Model::setTransform(QMatrix4x4 t)
{
    transform = t;
}

