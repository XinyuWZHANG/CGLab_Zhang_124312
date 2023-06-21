#ifndef OPENGL_FRAMEWORK_SCENEGRAPH_H
#define OPENGL_FRAMEWORK_SCENEGRAPH_H
#include <string>
#include "Node.h"

//TODO make SceneGraph a Singleton (only one scenegraph should be allowed to exist) ?

/* SceneGraph class for constructing a tree made out of root, camera, pointlight and planets  */
class SceneGraph
{
public:
    //constructors and destructor
    SceneGraph();
    SceneGraph(std::string name, const Node& root);
    ~SceneGraph();
    //getter
    std::string getName() const;
    Node getRoot() const;
    std::string printGraph() const;

private:
    std::string name_;
    Node root_;
    //setter
    void setName(std::string name);
    void setRoot(const Node& new_node);
};
#endif //OPENGL_FRAMEWORK_SCENEGRAPH_H