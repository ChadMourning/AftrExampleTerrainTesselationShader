#include "GLViewExampleTerrainTesselationShader.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "GLSLShaderDefaultGL32.h"
#include "MGLGridTesselated.h"
#include "GLSLUniform.h"

using namespace Aftr;

GLSLUniform* tess = nullptr;
int level = 4;

GLViewExampleTerrainTesselationShader* GLViewExampleTerrainTesselationShader::New( const std::vector< std::string >& args )
{
   GLViewExampleTerrainTesselationShader* glv = new GLViewExampleTerrainTesselationShader( args );
   glv->init( Aftr::GRAVITY, Vector( 0, 0, -1.0f ), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE );
   glv->onCreate();
   return glv;
}


GLViewExampleTerrainTesselationShader::GLViewExampleTerrainTesselationShader( const std::vector< std::string >& args ) : GLView( args )
{
   //Initialize any member variables that need to be used inside of LoadMap() here.
   //Note: At this point, the Managers are not yet initialized. The Engine initialization
   //occurs immediately after this method returns (see GLViewExampleTerrainTesselationShader::New() for
   //reference). Then the engine invoke's GLView::loadMap() for this module.
   //After loadMap() returns, GLView::onCreate is finally invoked.

   //The order of execution of a module startup:
   //GLView::New() is invoked:
   //    calls GLView::init()
   //       calls GLView::loadMap() (as well as initializing the engine's Managers)
   //    calls GLView::onCreate()

   //GLViewExampleTerrainTesselationShader::onCreate() is invoked after this module's LoadMap() is completed.
}


void GLViewExampleTerrainTesselationShader::onCreate()
{
   //GLViewExampleTerrainTesselationShader::onCreate() is invoked after this module's LoadMap() is completed.
   //At this point, all the managers are initialized. That is, the engine is fully initialized.

   if( this->pe != NULL )
   {
      //optionally, change gravity direction and magnitude here
      //The user could load these values from the module's aftr.conf
      this->pe->setGravityNormalizedVector( Vector( 0,0,-1.0f ) );
      this->pe->setGravityScalar( Aftr::GRAVITY );
   }
   this->setActorChaseType( STANDARDEZNAV ); //Default is STANDARDEZNAV mode
   //this->setNumPhysicsStepsPerRender( 0 ); //pause physics engine on start up; will remain paused till set to 1
}


GLViewExampleTerrainTesselationShader::~GLViewExampleTerrainTesselationShader()
{
   //Implicitly calls GLView::~GLView()
}


void GLViewExampleTerrainTesselationShader::updateWorld()
{
   GLView::updateWorld(); //Just call the parent's update world first.
                          //If you want to add additional functionality, do it after
                          //this call.
}


void GLViewExampleTerrainTesselationShader::onResizeWindow( GLsizei width, GLsizei height )
{
   GLView::onResizeWindow( width, height ); //call parent's resize method.
}


void GLViewExampleTerrainTesselationShader::onMouseDown( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseDown( e );
}


void GLViewExampleTerrainTesselationShader::onMouseUp( const SDL_MouseButtonEvent& e )
{
   GLView::onMouseUp( e );
}


void GLViewExampleTerrainTesselationShader::onMouseMove( const SDL_MouseMotionEvent& e )
{
   GLView::onMouseMove( e );
}


void GLViewExampleTerrainTesselationShader::onKeyDown( const SDL_KeyboardEvent& key )
{
   GLView::onKeyDown( key );
   if( key.keysym.sym == SDLK_0 )
      this->setNumPhysicsStepsPerRender( 1 );

   if( key.keysym.sym == SDLK_1 )
   {

   }
}


void GLViewExampleTerrainTesselationShader::onKeyUp( const SDL_KeyboardEvent& key )
{
   GLView::onKeyUp( key );
}


void Aftr::GLViewExampleTerrainTesselationShader::loadMap()
{
   this->worldLst = new WorldList(); //WorldList is a 'smart' vector that is used to store WO*'s
   this->actorLst = new WorldList();
   this->netLst = new WorldList();

   ManagerOpenGLState::GL_CLIPPING_PLANE = 10000.0;
   ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
   ManagerOpenGLState::enableFrustumCulling = false;
   Axes::isVisible = true;
   this->glRenderer->isUsingShadowMapping( false ); //set to TRUE to enable shadow mapping, must be using GL 3.2+

   this->cam->setPosition( 15,15,10 );

   std::string shinyRedPlasticCube( ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl" );
   std::string wheeledCar( ManagerEnvironmentConfiguration::getSMM() + "/models/rcx_treads.wrl" );
   std::string grass( ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl" );
   std::string human( ManagerEnvironmentConfiguration::getSMM() + "/models/human_chest.wrl" );
   
   //SkyBox Textures readily available
   std::vector< std::string > skyBoxImageNames; //vector to store texture paths
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_water+6.jpg" );
   //skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_dust+6.jpg" );
   skyBoxImageNames.push_back( ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg" );


   {
      //Create a light
      float ga = 0.1f; //Global Ambient Light level for this module
      ManagerLight::setGlobalAmbientLight( aftrColor4f( ga, ga, ga, 1.0f ) );
      WOLight* light = WOLight::New();
      light->isDirectionalLight( true );
      light->setPosition( Vector( 0, 0, 100 ) );
      //Set the light's display matrix such that it casts light in a direction parallel to the -z axis (ie, downwards as though it was "high noon")
      //for shadow mapping to work, this->glRenderer->isUsingShadowMapping( true ), must be invoked.
      light->getModel()->setDisplayMatrix( Mat4::rotateIdentityMat( { 0, 1, 0 }, 90.0f * Aftr::DEGtoRAD ) );
      light->setLabel( "Light" );
      worldLst->push_back( light );
   }

   {
      //Create the SkyBox
      WO* wo = WOSkyBox::New( skyBoxImageNames.at( 0 ), this->getCameraPtrPtr() );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->setLabel( "Sky Box" );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      worldLst->push_back( wo );
   }

   { 
      ////Create the infinite grass plane (the floor)
      WO* wo = WO::New( grass, Vector( 1, 1, 1 ), MESH_SHADING_TYPE::mstFLAT );
      wo->setPosition( Vector( 0, 0, 0 ) );
      wo->renderOrderType = RENDER_ORDER_TYPE::roOPAQUE;
      wo->upon_async_model_loaded( [wo]()
         {
            ModelMeshSkin& grassSkin = wo->getModel()->getModelDataShared()->getModelMeshes().at( 0 )->getSkins().at( 0 );
            grassSkin.getMultiTextureSet().at( 0 ).setTexRepeats( 5.0f );
            grassSkin.setAmbient( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Color of object when it is not in any light
            grassSkin.setDiffuse( aftrColor4f( 1.0f, 1.0f, 1.0f, 1.0f ) ); //Diffuse color components (ie, matte shading color of this object)
            grassSkin.setSpecular( aftrColor4f( 0.4f, 0.4f, 0.4f, 1.0f ) ); //Specular color component (ie, how "shiney" it is)
            grassSkin.setSpecularCoefficient( 10 ); // How "sharp" are the specular highlights (bigger is sharper, 1000 is very sharp, 10 is very dull)
         } );
      wo->setLabel( "Grass" );
      //worldLst->push_back( wo );
   }

   class GLSLShaderTerrainTessellation : public GLSLShaderDefaultGL32
   {
   protected:
      GLSLShaderTerrainTessellation(GLSLShaderDataShared* s) : GLSLShaderDefaultGL32(s)
      {

      }
   public:
      static GLSLShaderTerrainTessellation* New()
      {
         //std::string vert = ManagerEnvironmentConfiguration::getLMM() + "/shaders/terrain.vert";
         //std::string frag = ManagerEnvironmentConfiguration::getLMM() + "/shaders/terrain.frag";
         //std::string tcs = ManagerEnvironmentConfiguration::getLMM() + "/shaders/terrainlod.tcs";
         //std::string tes = ManagerEnvironmentConfiguration::getLMM() + "/shaders/terrain.tes";

         std::string vert = ManagerEnvironmentConfiguration::getLMM() + "/shaders/pass.vert";
         std::string frag = ManagerEnvironmentConfiguration::getLMM() + "/shaders/pass.frag";
         std::string tcs = ManagerEnvironmentConfiguration::getLMM() + "/shaders/pass.tcs";
         std::string tes = ManagerEnvironmentConfiguration::getLMM() + "/shaders/pass.tes";

         GLSLShaderDataShared* data = ManagerShader::loadShaderDataShared(vert, frag,tcs, tes);
        // GLuint program = data->getShaderHandle();
        
         //std::cout << c << std::endl;
         if (data == nullptr)
            return nullptr;

         GLSLShaderTerrainTessellation* ptr = new GLSLShaderTerrainTessellation(data);
         tess = new GLSLUniform("level", GLSLUniformType::utINT, ptr->getHandle());
         ptr->addUniform(tess);
         tess->set(4);
         return ptr;
      }
   };

   ModelMeshSkin skin(ManagerTex::loadTexAsync(ManagerEnvironmentConfiguration::getLMM() + "/images/iceland_heightmap.png").value(), GLSLShaderTerrainTessellation::New());
   std::vector< std::vector< VectorD > > points;
   std::vector< std::vector< aftrColor4ub > > colors;

   skin.setGLPrimType(GL_TRIANGLES);
   skin.setMeshShadingType(MESH_SHADING_TYPE::mstFLAT);

   points.resize(2);
   points[0].resize(2);
   points[1].resize(2);

   colors.resize(2);
   colors[0].resize(2);
   colors[1].resize(2);



   points[0][0] = (Vector(0, 0, 0));
   points[0][1] = (Vector(100, 0, 0));
   points[1][0] = (Vector(0, 100, 0));
   points[1][1] = (Vector(100, 100, 0));
   colors[0][0] = colors[0][1] = colors[1][0] = colors[1][1] = aftrColor4ub(255, 255, 255, 255);

   
   int size = 4;
   points.resize(size);
   colors.resize(size);
   for (int i = 0; i < size; i++)
   {
      points[i].resize(size);
      colors[i].resize(size);
      for (int j = 0; j < size; j++)
      {
         points[i][j] = VectorD(i * 10, j * 10, 0);
         colors[i][j] = aftrColor4ub(255, 255, 255, 255);
      }
   }
   

   WO* wo = WO::New();
   MGLGridTessellated* mgl = MGLGridTessellated::New(wo, points, colors);
 
   mgl->defineVertices();
   mgl->defineIndices();
   mgl->defineTextureCoords();
   mgl->defineNormals();
   mgl->defineColors();
   mgl->makeVBO(0);
   mgl->getSkins().at(0) = (std::move(skin));
   wo->setModel(mgl);
   worldLst->push_back(wo);

   //Make a Dear Im Gui instance via the WOImGui in the engine... This calls
   //the default Dear ImGui demo that shows all the features... To create your own,
   //inherit from WOImGui and override WOImGui::drawImGui_for_this_frame(...) (among any others you need).
   WOImGui* gui = WOImGui::New( nullptr );
   gui->setLabel( "My Gui" );
   gui->subscribe_drawImGuiWidget(
      [this, gui]() //this is a lambda, the capture clause is in [], the input argument list is in (), and the body is in {}
      {
         ImGui::Begin("Tesselation Control");
         if (ImGui::SliderInt("Tesselation Level", &level, 1, 64))
         {
            tess->set(level);
         }
         ImGui::Checkbox("Wireframe", &ManagerGLView::getGLViewT<GLViewExampleTerrainTesselationShader>()->wireframe);
         ImGui::End();
      } );
   this->worldLst->push_back( gui );

   //createExampleTerrainTesselationShaderWayPoints();
}


void GLViewExampleTerrainTesselationShader::createExampleTerrainTesselationShaderWayPoints()
{
   // Create a waypoint with a radius of 3, a frequency of 5 seconds, activated by GLView's camera, and is visible.
   WayPointParametersBase params(this);
   params.frequency = 5000;
   params.useCamera = true;
   params.visible = true;
   WOWayPointSpherical* wayPt = WOWayPointSpherical::New( params, 3 );
   wayPt->setPosition( Vector( 50, 0, 3 ) );
   worldLst->push_back( wayPt );
}
