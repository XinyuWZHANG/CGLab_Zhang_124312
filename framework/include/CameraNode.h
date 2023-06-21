#ifndef OPENGL_FRAMEWORK_CAMERANODE_H
#define OPENGL_FRAMEWORK_CAMERANODE_H
#include <glm/gtx/transform.hpp>
#include "Node.h"
/* for camera derived from node*/
class CameraNode: public Node{
public:
    //constructor and destructor
    CameraNode();
    CameraNode(std::string const& name, glm::mat4 const& projectionMatrix, std::shared_ptr<Node> parentNode);
    ~CameraNode() override;
//getter and setter
    bool getPerspective();
    bool getEnabled();
    void setEnabled(bool isEnabled);
    glm::mat4 getProjectionMatrix();
    void setProjectionMatrix(const glm::mat4& projectionMatrix);

private:
    std::string name_;
    bool isPerspective_; //
    bool isEnabled_; //is camera on?
    glm::mat4 projectionMatrix_; //Projection matrix for transformation
};
#endif //OPENGL_FRAMEWORK_CAMERANODE_H