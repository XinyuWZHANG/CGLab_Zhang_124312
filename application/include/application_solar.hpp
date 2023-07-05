#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "SceneGraph.h"
#include "GeometryNode.h"
#include "CameraNode.h"
#include "PointLightNode.h"


// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;
  // public scenegraph for initializing
  SceneGraph sceneGraph_;
  void renderStars() const;
  void renderPlanets() const;
  void renderSun() const;

  //Assignment 4
  unsigned int m_texture;
  unsigned int m_skyboxTexture;
  //Assignment 5
  unsigned m_width;
  unsigned m_height;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void initializeSceneGraph();//TODO
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();
  //making stars at random position
  void generateStars();

  //Assignment 4
  void initializeTextures();
  void initializeSkybox();
  void renderSkybox();
  //Assignment 5
  void initializeFramebuffer(unsigned width, unsigned height);

  // cpu representation of model
  model_object planet_object;
  model_object star_model;
  //Assignment 4
  model_object skybox_object;
  //Assignment 5
  framebuffer_object framebuffer_obj;
  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif