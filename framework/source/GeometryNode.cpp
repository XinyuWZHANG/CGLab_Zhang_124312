#include <utility>

#include "../include/GeometryNode.h"
//default constructor
GeometryNode::GeometryNode():
Node(),geometry_{}{}

GeometryNode::~GeometryNode() = default;
//getter
model GeometryNode::getGeometry()
{
    return geometry_;
}
//setter
void GeometryNode::setGeometry(model new_geometry)
{
    this->geometry_ = std::move(new_geometry);
}
//constructor with model
GeometryNode::GeometryNode(model geo):
Node(), geometry_{std::move(geo)}{}

GeometryNode::GeometryNode(std::string name, const glm::mat4& localTransformation, std::shared_ptr<Node> parent):
Node{std::move(name), localTransformation,std::move(parent) }{
}
