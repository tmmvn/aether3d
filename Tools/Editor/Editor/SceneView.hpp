#pragma once
#include "FileSystem.hpp"
#include "GameObject.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "Texture2D.hpp"
#include "Vec3.hpp"

class SceneView
{
public:
    void Init( int width, int height );
    void Render();
    void RotateCamera( float xDegrees, float yDegrees );
    
private:
    ae3d::GameObject camera;
    ae3d::Scene scene;
    ae3d::Shader unlitShader;
    ae3d::Vec3 moveDir;
    
    // TODO: Test content, remove when stuff works.
    ae3d::GameObject cube;
    ae3d::Texture2D gliderTex;
    ae3d::Material material;
    ae3d::Mesh cubeMesh;
};
