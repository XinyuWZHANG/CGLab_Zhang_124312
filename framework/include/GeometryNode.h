#ifndef OPENGL_FRAMEWORK_GEOMETRYNODE_H
#define OPENGL_FRAMEWORK_GEOMETRYNODE_H
#include "model.hpp"
#include "Node.h"

/* class for the geometry of a planet */
class GeometryNode: public Node
{
public:
    //constructor
    GeometryNode();
    GeometryNode(model geo);
    GeometryNode(std::string name, const glm::mat4& localTransformation, std::shared_ptr<Node> parent);
    //constructor
    ~GeometryNode() override;
    //getter and setter
    model getGeometry();
    void setGeometry(model geometry);

private:
    model geometry_;
};
#endif //OPENGL_FRAMEWORK_GEOMETRYNODE_H