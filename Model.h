#ifndef MODEL_H
#define MODEL_H
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cfloat>
#include "BasicDataStructure.hpp"
#include "Mesh.h"

class Model
{
    public:
        Model(QString path);
        void Draw();
        Coord3D centre;
        int triangleCount{0};
        int vertexCount{0};
        float GetYRange(){return maxY - minY;}
        bool loadSuccess{true};
    private:
        float minX{FLT_MAX};
        float minY{FLT_MAX};
        float minZ{FLT_MAX};
        float maxX{FLT_MIN};
        float maxY{FLT_MIN};
        float maxZ{FLT_MIN};
        std::vector<Mesh> meshes;
        std::vector<Texture> textureList;
        QString directory;
        void loadModel(QString path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        int loadMaterialTextures(Mesh & mesh,aiMaterial *mat, aiTextureType type);
};

#endif // MODEL_H
