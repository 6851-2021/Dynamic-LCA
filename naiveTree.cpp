#include "naiveTree.hpp"
#include <deque>

NaiveTreeNode::NaiveTreeNode(std::string id){
    data = id;
    parent = NULL;
}


void NaiveTreeNode::add_leaf(NaiveTreeNode* leaf) {
    children.push_back(leaf);
    leaf->parent = this;
}

void NaiveTreeNode::deleteNode() {
    while(children.size() > 0) {
        children.front()->deleteNode();
    }

    delete this;
}

NaiveTreeNode* NaiveTreeNode::lca(NaiveTreeNode* nodeX, NaiveTreeNode* nodeY) {
    if (nodeX == nodeY) {
        return(nodeX);
    }

    std::deque<NaiveTreeNode*> xPath;
    std::deque<NaiveTreeNode*> yPath;
    
    NaiveTreeNode* currNode = nodeX;
    while (currNode) {
        xPath.push_front(currNode);
        currNode = currNode->parent;
    }

    currNode = nodeY;
    while (currNode) {
        yPath.push_front(currNode);
        currNode = currNode->parent;
    }

    size_t i = 0;
    while (i < xPath.size() && i < yPath.size() && xPath[i] == yPath[i]) {
        i++;
    }

    NaiveTreeNode* lcaNode = xPath[i-1];    
    return lcaNode;
}