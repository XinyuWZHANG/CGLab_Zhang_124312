#ifndef OPENGL_FRAMEWORK_NODE_H
#define OPENGL_FRAMEWORK_NODE_H
#include <string>
#include <vector>
#include <glm/gtx/transform.hpp>
#include <memory>
/* node parent class */
class Node
{
public:
    Node();
    virtual ~Node(); //destructor
    //lots of different constructors
    Node(std::string name);
    Node(std::string name,std::shared_ptr<Node> parent);
    Node(std::string name, const glm::mat4& localTransform);
    Node(std::string name, const glm::mat4& localTransform, const glm::mat4& worldTransform);
    Node(std::string name, const glm::mat4& localTransform, const glm::mat4& worldTransform, std::shared_ptr<Node> parent);
    Node(std::string name, const glm::mat4& localTransform, std::shared_ptr<Node> parent, glm::vec3 const& color);
    Node(std::string name, const glm::mat4& localTransform, std::shared_ptr<Node> parent);

    Node(std::string name, const glm::mat4& localTransform, const glm::mat4& worldTransform, int depth);
    //getter and setter
    std::shared_ptr<Node> getParent() const;
    void setParent(std::shared_ptr<Node> new_node);
    std::shared_ptr<Node> getChildren(const std::string& name) const;
    std::vector<std::shared_ptr<Node>> getChildrenList() const;
    std::string getName() const;
    std::string getPath() const;
    int getDepth() const;
    glm::mat4 getLocalTransform() const;
    void setLocalTransform(const glm::mat4& localTransform);
    glm::mat4 getWorldTransform() const;
    void setWorldTransform(const glm::mat4& worldTransform);
    void addChild(const std::shared_ptr<Node>& new_node); //adding node child
    Node removeChild(const std::string& name); // removing node child

    glm::vec3 getPlanetColor();
    void setPlanetColor(glm::vec3 const& color);

    //Assignment4
    void setTexture(std::string texture);
    std::string getTexture();
    void setTexInt(unsigned int texInt);
    unsigned int getTexInt();

private:
    std::shared_ptr<Node> parent_;  //parent node
    std::vector<std::shared_ptr<Node>> children_; //vector of children nodes
    std::string name_;  //name of node
    std::string path_;
    int depth_; // length of path
    glm::mat4 localTransform_;  //local transformation matrix
    glm::mat4 worldTransform_;  //world transformation matrix
    glm::vec3 planetColor_;

    //Assignment 4
    std::string texture_;
    unsigned int texInt_;
};

#endif //OPENGL_FRAMEWORK_NODE_H