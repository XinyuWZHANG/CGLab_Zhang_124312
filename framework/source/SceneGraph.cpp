#include <utility>
#include <iostream>

#include "SceneGraph.h"
//constructor
SceneGraph::SceneGraph() = default;

SceneGraph::SceneGraph(std::string name, const Node& root):
name_{std::move(name)}, root_{root}{}
//destructor
SceneGraph::~SceneGraph() = default;
//getter for name
std::string SceneGraph::getName() const
{
    return name_;
}
//setter for name
void SceneGraph::setName(std::string new_name)
{
    this->name_ = std::move(new_name);
}
//getter for root
Node SceneGraph::getRoot() const
{
    return root_;
}
//setter for root
void SceneGraph::setRoot(const Node& node)
{
    root_ = node;
}
//printing children names on console, is recursive
std::string printChild(const std::vector<std::shared_ptr<Node>>& children, bool hasChild){
    //go through loop for all children
    for ( auto const& i : children) {
        std::cout<< i->getName() << "  ->  "; //parent name ->
        if (!i->getChildrenList().empty()){ 
            printChild(i->getChildrenList(), false); //print children name, if they exist
        }
        if (hasChild){
            std::cout<<std::endl; //new line
        }
        //todo: test if formating is right
    }
}

std::string SceneGraph::printGraph() const
{
    std::cout<<"Root = " + getRoot().getName()<<std::endl;
    std::cout<<"Children:  "<<std::endl;
    std::vector<std::shared_ptr<Node>> children = root_.getChildrenList();
    printChild(children, true);

}



