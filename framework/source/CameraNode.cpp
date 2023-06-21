#include "CameraNode.h"

#include <utility>

CameraNode::CameraNode():
        Node(),
        projectionMatrix_{}{}

CameraNode::~CameraNode() = default;

bool CameraNode::getPerspective()
{
    return isPerspective_;
}

bool CameraNode::getEnabled()
{
    return isEnabled_;
}

void CameraNode::setEnabled(bool isEnabled)
{
    isEnabled_ = true;
}

glm::mat4 CameraNode::getProjectionMatrix()
{
    return projectionMatrix_;
}

void CameraNode::setProjectionMatrix(const glm::mat4& projectionMatrix)
{
    this->projectionMatrix_ = projectionMatrix;
}

CameraNode::CameraNode(const std::string &name, glm::mat4 const& projectionMatrix, std::shared_ptr<Node> parentNode):
    Node{name, projectionMatrix, std::move(parentNode)}
{}
