#include "Model.h"
#include <QDebug>

Model::Model(QString path)
{
    loadModel(path);
    centre = {(maxX + minX) / 2.f, (maxY + minY) / 2.f, (maxZ + minZ) / 2.f};
}

void Model::Draw()
{
    SRendererDevice::GetInstance().textureList = textureList;
    for(int i = 0; i < meshes.size(); i++)
        meshes[i].Draw();
}

void Model::loadModel(QString path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path.toStdString(), aiProcess_Triangulate | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        loadSuccess = false;
        return;
    }
    QString modelName = path.mid(path.lastIndexOf('/') + 1);
    directory = path.mid(0, path.lastIndexOf('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vertexCount += mesh->mNumVertices;
    triangleCount += mesh->mNumFaces;
    Mesh res;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    Vertex vertex;
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        minX = std::min(minX, mesh->mVertices[i].x);
        minY = std::min(minY, mesh->mVertices[i].y);
        minZ = std::min(minZ, mesh->mVertices[i].z);
        maxX = std::max(maxX, mesh->mVertices[i].x);
        maxY = std::max(maxY, mesh->mVertices[i].y);
        maxZ = std::max(maxZ, mesh->mVertices[i].z);
        vertex.worldSpacePos = Coord3D(mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z);
        vertex.normal = Coord3D(mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z);
        if(mesh->mTextureCoords[0])//only process one texture
            vertex.texCoord = Coord2D(mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y);
        else
            vertex.texCoord = Coord2D(0.0f,0.0f);
        vertices.push_back(vertex);
    }
    res.vertices = vertices;
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    res.indices = indices;
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        res.diffuseTextureIndex = loadMaterialTextures(res, material, aiTextureType_DIFFUSE);
        res.specularTextureIndex = loadMaterialTextures(res, material, aiTextureType_SPECULAR);
    }
    return res;
}

int Model::loadMaterialTextures(Mesh &mesh, aiMaterial *mat, aiTextureType type)
{
    if(mat->GetTextureCount(type) > 0)
    {
        aiString str;
        mat->GetTexture(type, 0, &str);
        QString path = directory + '/' + str.C_Str();
        for(int i = 0; i < textureList.size(); i++)
        {
            if(textureList[i].path == path)
                return i;
        }
        Texture texture;
        if(texture.LoadFromImage(path))
        {
            qDebug() << path;
            textureList.push_back(texture);
            return (int)textureList.size() - 1;
        }
    }
    return -1;
}


