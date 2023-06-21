#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//don't load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,sceneGraph_{}
 ,planet_object{}
 ,star_model{}
 //Assignment 4
 ,skybox_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}, m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeSceneGraph();
  initializeGeometry();
  generateStars(); //Assignment 2
  initializeShaderPrograms();

  //Assignment 4
  initializeTextures();
  initializeSkybox();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
  // //Assignment 2: delete buffers for stars
  glDeleteBuffers(1, &star_model.vertex_BO);
  glDeleteBuffers(1, &star_model.element_BO);
  glDeleteVertexArrays(1, &star_model.vertex_AO);

}


///////////////////////////////// UPLOAD FUNCTIONS ///////////////////////////
void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  //Assignment 2
  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ModelViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
    glUseProgram(m_shaders.at("light").handle);
    glUniformMatrix4fv(m_shaders.at("light").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
    // bind shader to which to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
    //sun
    glUseProgram(m_shaders.at("light").handle);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("light").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));

    //the same for stars
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
    // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

/////////////////// Assignment 2 // creating Geometry for Stars
void ApplicationSolar::generateStars()
{
    //vector for saving stars in float
    std::vector<GLfloat> stars;
    //maximal distance that stars may have
    unsigned int distance = 1000;
    //number of stars
    unsigned int numStars = 400000;
    //for scattering
    float middle_value = 200.0f;

    //for each star define attributes
    for (unsigned int i = 0; i < numStars; i++)
    {
        // x, y, z position of one star
        GLfloat x_pos = (static_cast<float>(static_cast<unsigned int>(std::rand()) % distance)) - middle_value;
        GLfloat y_pos = (static_cast<float>(static_cast<unsigned int>(std::rand()) % distance)) - middle_value;
        GLfloat z_pos = (static_cast<float>(static_cast<unsigned int>(std::rand()) % distance)) - middle_value;
        stars.push_back(x_pos); //adding to vector
        stars.push_back(y_pos);
        stars.push_back(z_pos);

        //colours
        GLfloat r_colour = static_cast<float>(std::rand() % 255)/255;  // Generate a random red component between 0.0 and 255.0
        GLfloat g_colour = static_cast<float>(std::rand() % 255)/255; // Generate a random green component between 0.0 and 255.0
        GLfloat b_colour = static_cast<float>(std::rand() % 255)/255; // Generate a random blue component between 0.0 and 255.0
        stars.push_back(r_colour);
        stars.push_back(g_colour);
        stars.push_back(b_colour);


        // Debug output
       /* std::cout << "Generated star " << i + 1 << ":"
                  << " Position=(" << x_pos << ", " << y_pos << ", " << z_pos << ")"
                  << " Color=(" << r_colour << ", " << g_colour << ", " << b_colour << ")"
                  << std::endl;*/

    }
    //creating vertex array
    glGenVertexArrays(1, &star_model.vertex_AO);
    //bind vertex array to attach buffers
    glBindVertexArray(star_model.vertex_AO);

    //creating buffer and loading data
    glGenBuffers(1, &star_model.vertex_BO); //generic buffer
    glBindBuffer(GL_ARRAY_BUFFER, star_model.vertex_BO); //binding generic buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(stars.size()), stars.data(), GL_STATIC_DRAW); //configure bound array buffer

    //position array
    glEnableVertexArrayAttrib(star_model.vertex_AO, 0);
    glVertexAttribPointer(GLuint(0), GLuint(3), GL_FLOAT, GL_FALSE, GLsizei(sizeof(float)*6), nullptr);

    //colour array
    glEnableVertexArrayAttrib(star_model.vertex_AO, 1);
    //
    glVertexAttribPointer(GLuint(1),GLuint(3),GL_FLOAT, GL_FALSE, GLsizei(sizeof(float)*6) , (void*)(sizeof(float)*3));

    //drawing mit draw_mode points
    star_model.draw_mode = GL_POINTS;
    star_model.num_elements = GLsizei(numStars);
}
///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

  //Assignment 2:
  //shaders for stars
  m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/vao.vert"},
            {GL_FRAGMENT_SHADER, m_resource_path + "shaders/vao.frag"}}});
  m_shaders.at("star").u_locs["ModelViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

  /////Assignment 3:
  //uniform locations
  m_shaders.at("planet").u_locs["planetColor"] = -1;
  m_shaders.at("planet").u_locs["ambientColor"] = -1;
  m_shaders.at("planet").u_locs["lightPosition"] = -1;
  m_shaders.at("planet").u_locs["lightIntensity"] = -1;
  m_shaders.at("planet").u_locs["colorSpecular"] = -1;
  m_shaders.at("planet").u_locs["cameraPosition"] = -1;

    m_shaders.emplace("light", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/light.vert"},
                                                {GL_FRAGMENT_SHADER, m_resource_path + "shaders/light.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("light").u_locs["NormalMatrix"] = -1;
    m_shaders.at("light").u_locs["ModelMatrix"] = -1;
    m_shaders.at("light").u_locs["ViewMatrix"] = -1;
    m_shaders.at("light").u_locs["ProjectionMatrix"] = -1;

}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as a vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}


///////////////////////////// callback functions for window events ////////////
// handle key input
//Newly add
bool isCelShadingMode = false;

void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_A  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
      uploadView();
  }
  else if (key == GLFW_KEY_D  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.1f});
      uploadView();
  }
  //Newly add(Cel-Shading)
  else if(key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      isCelShadingMode = false;
  }
  else if(key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT)){
      isCelShadingMode = false;
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
  //imagine dividing window with coordinate system in the middle -> 4 cases
  /*
    if (pos_x > 0 && pos_y > 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.01f, 0.01f,0.0f});
        uploadView();
    }
    if (pos_x < 0 && pos_y > 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.01f, 0.01f,0.0f});
        uploadView();
    }
    if (pos_y < 0 && pos_x > 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.01f, -0.01f,0.0f});
        uploadView();
    }
    if (pos_y < 0 && pos_x < 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.01f, -0.01f,0.0f});
        uploadView();
    }*/
    if (pos_x > 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.01f, 0.0f,0.0f});
        uploadView();
    }
    if (pos_x < 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.01f, 0.0f,0.0f});
        uploadView();
    }
    if (pos_y > 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.01f,0.0f});
        uploadView();
    }
    if (pos_y < 0){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.01f,0.0f});
        uploadView();
    }
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}

//function to connect scenegraph to application
void ApplicationSolar::initializeSceneGraph() {
//root node
    Node root{"Root",glm::translate({},glm::vec3{0.0f,0.0f,0.0f}), glm::translate( {}, glm::vec3{1.0f, 0.0f, 0.0f})};
//Point Light
    PointLightNode pointLightNode("Sun", glm::vec3{1.0f,1.0f,1.0f}, 0.5f, std::make_shared<Node>(root)); //lightcolor = white
    //pointLightNode.setPlanetColor(glm::vec3{1.0f,1.0f,1.0f});
    GeometryNode sunGeometry("SunGeometry", glm::translate({}, glm::fvec3{0.0f, 0.0f, 0.0f }),std::make_shared<PointLightNode>(pointLightNode));
    root.addChild(std::make_shared<PointLightNode>(pointLightNode));
    Node sunNode("sun2" ,glm::translate({},glm::vec3{0.0f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{1.0f, 1.0f, 1.0f});
    GeometryNode sunGeo("sun2G", glm::translate({},glm::vec3{0.0f,0.0f,0.0f}), std::make_shared<Node>(sunNode));
    sunNode.addChild(std::make_shared<Node>(sunGeo)); //adding geonode to planetnode
    root.addChild(std::make_shared<Node>(sunNode));

    //Assignment 4
    sunNode.setTexture(m_resource_path + "textures/sunmap.jpg");

//Camera
    CameraNode cameraNode("Camera", glm::transpose(glm::inverse(m_view_transform)), std::make_shared<Node>(root)); //TODO
    root.addChild(std::make_shared<CameraNode>(cameraNode));

//Mercury
    Node mercuryNode("Mercury" ,glm::translate({},glm::vec3{5.79f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{1.0f, 0.5f, 0.0f});
    GeometryNode mercuryGeo("MercuryG", glm::translate({},glm::vec3{5.79f,0.0f,0.0f}), std::make_shared<Node>(mercuryNode));
    //geoNode do not translate
    mercuryNode.addChild(std::make_shared<Node>(mercuryGeo)); //adding geonode to planetnode
    root.addChild(std::make_shared<Node>(mercuryNode)); //adding node to root

    //Assignment 4
    mercuryNode.setTexture(m_resource_path + "textures/mercurymap.jpg");

//Venus
    Node venusNode("Venus" ,glm::translate({},glm::vec3{10.82f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{0.9f, 0.8f, 0.0f});
    GeometryNode venusGeo("VenusG", glm::translate({},glm::vec3{10.82f,0.0f,0.0f}), std::make_shared<Node>(venusNode));
    venusNode.addChild(std::make_shared<Node>(venusGeo));//adding geonode to planetnode
    root.addChild(std::make_shared<Node>(venusNode));//adding node to root

    //Assignment 4
    venusNode.setTexture(m_resource_path + "textures/venusmap.jpg");

//Earth
    Node earthNode("Earth" ,glm::translate({},glm::vec3{14.96f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{0.0f, 0.5f, 1.0f});
    GeometryNode earthGeo("EarthG" ,glm::translate({},glm::vec3{14.96f,0.0f,0.0f}),  std::make_shared<Node>(earthNode));
    earthNode.addChild(std::make_shared<Node>(earthGeo));

    //Assignment 4
    earthNode.setTexture(m_resource_path + "textures/earthmap1k.jpg");

//Moon TODO
//parent relation not right
    Node moonNode("Moon" ,glm::translate({},glm::vec3{10.0f,0.0f,0.0f}),  std::make_shared<Node>(earthNode), glm::vec3{0.7f, 0.7f, 0.7f}); //moon has earth as parent node
    GeometryNode moonGeo("MoonG" ,glm::translate({},glm::vec3{10.0f,0.0f,0.0f}),  std::make_shared<Node>(moonNode));
    moonNode.addChild(std::make_shared<Node>(moonGeo));
    earthNode.addChild(std::make_shared<Node>(moonNode)); //earth node holds moon node
    root.addChild(std::make_shared<Node>(earthNode));

//Mars
    Node marsNode("Mars" ,glm::translate({},glm::vec3{22.8f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{1.0f, 0.0f, 0.0f});
    GeometryNode marsGeo("MarsG" ,glm::translate({},glm::vec3{22.8f,0.0f,0.0f}),  std::make_shared<Node>(marsNode));
    marsNode.addChild(std::make_shared<Node>(marsGeo));
    root.addChild(std::make_shared<Node>(marsNode));

    //Assignment 4
    marsNode.setTexture(m_resource_path + "textures/mars_1k_color.jpg");

//Jupiter
    Node jupiterNode("Jupiter" ,glm::translate({},glm::vec3{77.85f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{0.7f, 0.3f, 0.1f});
    GeometryNode jupiterGeo("JupiterG" ,glm::translate({},glm::vec3{77.85f,0.0f,0.0f}),  std::make_shared<Node>(jupiterNode));
    jupiterNode.addChild(std::make_shared<Node>(jupiterGeo));
    root.addChild(std::make_shared<Node>(jupiterNode));

    //Assignment 4
    jupiterNode.setTexture(m_resource_path + "textures/jupitermap.jpg");

//Saturn
    Node saturnNode("Saturn" ,glm::translate({},glm::vec3{143.2f,0.0f,0.0f}), std::make_shared<Node>(root), glm::vec3{0.8f, 0.8f, 0.6f});
    GeometryNode saturnGeo("SaturnG" ,glm::translate({},glm::vec3{143.2f,0.0f,0.0f}),  std::make_shared<Node>(saturnNode));
    saturnNode.addChild(std::make_shared<Node>(saturnGeo));
    root.addChild(std::make_shared<Node>(saturnNode));

    //Assignment 4
    saturnNode.setTexture(m_resource_path + "textures/saturnmap.jpg");

//Uranus
    Node uranusNode("Uranus" ,glm::translate({},glm::vec3{250.0f,0.0f,0.0f}),  std::make_shared<Node>(root), glm::vec3{0.5f, 0.8f, 1.0f});
    GeometryNode uranusGeo("UranusG" ,glm::translate({},glm::vec3{250.0f,0.0f,0.0f}),  std::make_shared<Node>(uranusNode));
    uranusNode.addChild(std::make_shared<Node>(uranusGeo));
    root.addChild(std::make_shared<Node>(uranusNode));

    //Assignment 4
    uranusNode.setTexture(m_resource_path + "textures/uranusmap.jpg");
//Neptune
    Node neptunNode("Neptun" ,glm::translate({},glm::vec3{500.0f,0.0f,0.0f}), std::make_shared<Node>(root), glm::vec3{0.0f, 0.0f, 0.8f});
    GeometryNode neptunGeo("NeptunG" ,glm::translate({},glm::vec3{500.0f,0.0f,0.0f}),  std::make_shared<Node>(neptunNode));
    neptunNode.addChild(std::make_shared<Node>(neptunGeo));
    root.addChild(std::make_shared<Node>(neptunNode));

    //Assignment 4
    neptunNode.setTexture(m_resource_path + "textures/neptunemap.jpg");

    SceneGraph sceneGraph{"SceneGraph",root};
    sceneGraph_ = sceneGraph;
    //sceneGraph_.printGraph();
//std::cout << root.getPath() << std::endl;
//std::cout << root.getDepth() << std::endl;
}


void ApplicationSolar::render() const {

    renderPlanets();
    renderStars();
    //renderSkybox();
}

void ApplicationSolar::renderPlanets() const {
    //making new list for only planets
    std::vector<std::shared_ptr<Node>> planets;
    //
    // pushing planet nodes into new list
    planets.push_back(sceneGraph_.getRoot().getChildren("Mercury"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Venus"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Mercury"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Earth"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Moon"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Mars"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Jupiter"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Saturn"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Uranus"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Neptun"));
    planets.push_back(sceneGraph_.getRoot().getChildren("sun2"));

//sun
    // bind shader to upload uniforms
    /*
    glUseProgram(m_shaders.at("light").handle);
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 0.0f, 1.0f});
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, 0.0f});
    glUniformMatrix4fv(m_shaders.at("light").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));
    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("light").u_locs.at("NormalMatrix"),1, GL_FALSE, glm::value_ptr(normal_matrix));
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
*/
    ///////Assignment 3
    glUseProgram(m_shaders.at("planet").handle);

    auto sun = std::dynamic_pointer_cast<PointLightNode>(sceneGraph_.getRoot().getChildren("Sun"));

    //Get the location of uniform variable in shader program
    GLint planetColorLoc = glGetUniformLocation(m_shaders.at("planet").handle,"planetColor");
    GLint lightIntensityLoc = glGetUniformLocation(m_shaders.at("planet").handle,"lightIntensity");
    GLint colorSpecularLoc = glGetUniformLocation(m_shaders.at("planet").handle,"colorSpecular");
    GLint lightPositionLoc = glGetUniformLocation(m_shaders.at("planet").handle,"lightPosition");
    //Pass the specific value to the corresponding uniform variable for shader program to use during rending process

    glm::vec3 planetColor = sun->getPlanetColor();
    float lightIntensity = sun->getLightIntensity();
    glm::mat4 lightPosition = sun->getWorldTransform();
    //diffuse light
    glUniform3f(planetColorLoc,0.5f,0.5f,0.5f);
    //light intensity
    glUniform1f(lightIntensityLoc,lightIntensity);
    //specular light
    glUniform3f(colorSpecularLoc,1.0f,1.0f,1.0f);
    //light ray position vector
    glUniform3f(lightPositionLoc,lightPosition[3][0],lightPosition[3][1],lightPosition[3][2]);

    float time = 10.0f;

    //Assignment 4
    int sampler_location = glGetUniformLocation(m_shaders.at("planet").handle, "diffuseTexture");
    //

    for ( const std::shared_ptr<Node>& i : planets) {

        glm::mat4 matrix_render; //solution after rotation
        // bind shader to upload uniforms
        glUseProgram(m_shaders.at("planet").handle);

            std::shared_ptr<Node> planetGeo = i->getChildren(i->getName()+"G"); //getting geometry of planet
            //rotate identity matrix
            glm::mat4 matrix_rotation = glm::rotate(glm::fmat4{}, time*0.00001f , glm::vec3{0.0f,1.0f,0.0f}); //float cast or else function doesn't work
            i->setLocalTransform(matrix_rotation * i->getWorldTransform());
            matrix_render = glm::rotate(i->getLocalTransform(),time*0.00001f, glm::vec3{0.0f,1.0f,0.0f}); //matrix for rendering with computed local transformation matrix
            //TODO update the parent transform
            //TODO do not get the parent new world transform
        ++time;

        //uniformmatrix with matrix_render solution
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(matrix_render));
        matrix_render = glm::inverseTranspose(matrix_render);

        ///////Assignment 3:
        //planet color is ambient
        glUniform3f(m_shaders.at("planet").u_locs.at("ambientColor"), i->getPlanetColor()[0] ,i->getPlanetColor()[1],i->getPlanetColor()[2]);
        //get camera position
        glm::vec4 cameraPosition = sceneGraph_.getRoot().getChildren("Camera")->getLocalTransform() * m_view_transform * glm::vec4{0.0f,0.0f,0.0f,1.0f};
        //camera position for computing vectors
        glUniform3f(m_shaders.at("planet").u_locs.at("cameraPosition"), cameraPosition.x,cameraPosition.y,cameraPosition.z);

        //bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);
        //draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

        //Assignment 4
        GLuint textureID = i->getTexInt();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureID);
        glUniform1i(sampler_location,0);
        glUniform3f(planetColorLoc, i->getPlanetColor().x, i->getPlanetColor().y,i->getPlanetColor().z);
    }

}
void ApplicationSolar::renderStars() const {
    //use shaders
    glUseProgram(m_shaders.at("star").handle);
    //bind vertex array (vao)
    glBindVertexArray(star_model.vertex_AO);
    //draw array of stars
    //std::cout << star_model.num_elements;
    glDrawArrays(star_model.draw_mode,GLint(0),star_model.num_elements);

}

void ApplicationSolar::renderSun() const {
    glUseProgram(m_shaders.at("planet").handle);

}

//Assignment 4
void ApplicationSolar::initializeTextures() {

    //making new list for only planets
    std::vector<std::shared_ptr<Node>> planets;

    // pushing planet nodes into new list
    planets.push_back(sceneGraph_.getRoot().getChildren("Mercury"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Venus"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Earth"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Moon"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Mars"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Jupiter"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Saturn"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Uranus"));
    planets.push_back(sceneGraph_.getRoot().getChildren("Neptun"));
    planets.push_back(sceneGraph_.getRoot().getChildren("sun2"));

    for(const std::shared_ptr<Node>& i : planets){
        pixel_data planet_data;

        try{
            //planet_data = texture_loader::file(i->getTexture());
            planet_data = texture_loader::file(m_resource_path +"textures/venusmap.jpg");
            //planet_data = texture_loader::file(m_resource_path +"textures/uranusmap.jpg");
            //planet_data = texture_loader::file(m_resource_path+"textures/"+i->getName()+".jpg");
        }
        catch (std::exception e){
            std::cout<<"Textures load failure for "+ i->getName()<<":"<<e.what()<<std::endl;
        }

        //Creating texture objects
        glGenTextures(1,&m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        //Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //Generate Texture
        glTexImage2D(GL_TEXTURE_2D,0,planet_data.channels,(GLsizei)planet_data.width,(GLsizei)planet_data.height,0,planet_data.channels,planet_data.channel_type,planet_data.ptr());
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, planet_data.width, planet_data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, planet_data.ptr());
        i->setTexInt(m_texture);
    }

}

//Assignment 4
void ApplicationSolar::initializeSkybox() {
    float skyboxVertices[] = {
            // Vertex coordinates
            -1.0f,  1.0f, -1.0f, //Top left back
            1.0f,  1.0f, -1.0f, //Top right back
            -1.0f, -1.0f, -1.0f, //Bottom left back
            1.0f, -1.0f, -1.0f, //Bottom right back
            -1.0f,  1.0f,  1.0f, //Top left front
            1.0f,  1.0f,  1.0f, //Top right front
            -1.0f, -1.0f,  1.0f, //Bottom left front
            1.0f, -1.0f,  1.0f  //Bottom right front
    };

}

//Assignment 4
void ApplicationSolar::renderSkybox() const{
    //Disable writing to the depth buffer
    glDepthMask(GL_FALSE);
    //Use the skybox shader program
    glUseProgram(m_shaders.at("skybox").handle);
    //Set the uniform for the skybox texture
    GLint skyboxLoc = glGetUniformLocation(m_shaders.at("skybox").handle, "skybox_Texture");
    glUniform1i(skyboxLoc, m_skyboxTexture);
    //Activate texture unit 11
    glActiveTexture(GL_TEXTURE10);
    //Bind the skybox texture to the active texture unit
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    //Bind the vertex array object for the skybox
    glBindVertexArray(skybox_object.vertex_AO);
    //Draw the skybox
    glDrawArrays(skybox_object.draw_mode, 0, skybox_object.num_elements);
    //Enable writing to the depth buffer again
    glDepthMask(GL_TRUE);
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}