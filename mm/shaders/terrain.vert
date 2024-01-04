#version 410
//Written by Scott Nykl. Default Shader for a Core 3.2+ (really 4.3+).
//Performs per fragment ambient/diffuse/specular shading with material
//properties, textures, and lights.

layout ( location = 0 ) in vec3 VertexPosition;
layout ( location = 1 ) in vec3 VertexNormal;
layout ( location = 2 ) in vec2 VertexTexCoord;
layout ( location = 3 ) in vec4 VertexColor;

uniform mat4 ModelMat;
uniform mat4 NormalMat;
uniform mat4 TexMat0; //Texture Matrix for TexUnit0
uniform mat4 MVPMat;
uniform int level;

uniform sampler2D TexUnit0;


layout ( std140 ) uniform CameraTransforms
{
   mat4 View;
   mat4 Projection;
   mat4 Shadow; //for shadow mapping
   // A Value of 0 = Render w/ No shadows
   // A Value of 1 = Generate depth map only
   // A Value of 2 = Render w/ Shadow mapping
   int ShadowMapShadingState;
} Cam;

//out vec4 Color;
//out vec3 VertexES; //eye space Vertex
//out vec3 NormalES; //eye space Vertex Normal Vector
out vec2 TexCoord;
//out vec4 ShadowCoord;
//flat out int ShadowMapShadingState; //Set to non-zero value when generating a shadow map (only filling the depth buffer)

void main()
{
    // convert XYZ vertex to XYZW homogeneous coordinate
    gl_Position = vec4(VertexPosition, 1.0);
    // pass texture coordinate though
    TexCoord = VertexTexCoord;
}