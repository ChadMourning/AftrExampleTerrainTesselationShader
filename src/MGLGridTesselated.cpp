#include "MGLGridTesselated.h"
#include "GLViewExampleTerrainTesselationShader.h"
#include "Camera.h"
using namespace Aftr;

MGLGridTessellated* MGLGridTessellated::New(WO* parentWO, const std::vector< std::vector< VectorD > >& points,
   const std::vector< std::vector< aftrColor4ub > >& colors)
{
   MGLGridTessellated* ptr = new MGLGridTessellated(parentWO, points, colors);
   ptr->onCreate();
   return ptr;
}

MGLGridTessellated::MGLGridTessellated(WO* parentWO, const std::vector< std::vector< VectorD > >& points,
   const std::vector< std::vector< aftrColor4ub > >& colors) : MGLGrid(parentWO, points, VectorD(1,1,1), colors)
{

}

void MGLGridTessellated::onCreate()
{
   //generate data
   // vertex generation
   std::vector<float> vertices;

   unsigned rez = 20;
   float width = 1000;
   float height = 1000;
   for (unsigned i = 0; i <= rez - 1; i++)
   {
      for (unsigned j = 0; j <= rez - 1; j++)
      {
         vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
         
         vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
         vertices.push_back(0.0f); // v.y
         vertices.push_back(i / (float)rez); // u
         vertices.push_back(j / (float)rez); // v

         vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
         
         vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
         vertices.push_back(0.0f); // v.y
         vertices.push_back((i + 1) / (float)rez); // u
         vertices.push_back(j / (float)rez); // v

         vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
         
         vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
         vertices.push_back(0.0f); // v.y
         vertices.push_back(i / (float)rez); // u
        vertices.push_back((j + 1) / (float)rez); // v

         vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
         
         vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
         vertices.push_back(0.0f); // v.y
         vertices.push_back((i + 1) / (float)rez); // u
         vertices.push_back((j + 1) / (float)rez); // v
      }
   }

   numPatches = 4 * rez * rez;

   GLsizei stride = sizeof(GLfloat) * 3 +sizeof(GLfloat) * 2;

   
   glGenBuffers(1, &vertexBuffer);

   //send interleaved data for patches
   glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
   glBufferData(GL_ARRAY_BUFFER, numPatches * stride, vertices.data(), GL_STATIC_DRAW);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   //no index buffer
   GLuint vertexLocation = ManagerShader::getDefaultAttributeLocationVertexPosition();
   GLuint texCoordLocation = ManagerShader::getDefaultAttributeLocationVertexTexCoord();

  
   glGenVertexArrays(1, &vaoHandle);
   glBindVertexArray(vaoHandle);
   glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

   glEnableVertexAttribArray(vertexLocation);
   glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, stride, 0);

   glEnableVertexAttribArray(texCoordLocation);
   glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(static_cast<uintptr_t>(sizeof(GLfloat) * 3)));

   glBindVertexArray(0);

}

void MGLGridTessellated::render(const Camera& cam)
{
   //bind skin
   Mat4 modelMatrix = this->getModelMatrix(); ///< Returns the full 4x4 model matrix used by a shader, including orientation, position, scale.
   Mat4 normalMatrix = cam.getCameraViewMatrix() * this->dispMatrix; //for transforming normals in the shader

   std::tuple< const Mat4&, const Mat4&, const Camera& > shaderParams(modelMatrix, normalMatrix, cam);
   ModelMeshSkin& skin = this->getSkin();
   skin.bind(&shaderParams);

   if (this->modelData != nullptr)
   {

      //glBegin(GL_PATCHES);
      //glVertex3f(0, 0, 0);
      //glVertex3f(100, 0, 0);
      //glVertex3f(0, 100, 0);
      //glEnd();

      GLuint handle = vaoHandle;
      GLuint patchSize = 4;
      //GLuint handle = this->modelData->getModelMeshes().at(0)->getMeshDataShared()->getModelMeshRenderData(MESH_SHADING_TYPE::mstFLAT,GL_TRIANGLES)->getVAOHandle();
      //GLuint patchSize = 3;
      if(ManagerGLView::getGLViewT<GLViewExampleTerrainTesselationShader>()->wireframe)
         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glBindVertexArray(handle);
      glPatchParameteri(GL_PATCH_VERTICES, patchSize);
      glDrawArrays(GL_PATCHES, 0, numPatches); // VBO
      glBindVertexArray(0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }

   skin.unbind();
}
/*
void MGLGridTessellated::defineVertices()
{
   //buffer your own data
}
*/