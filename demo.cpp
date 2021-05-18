#include "lcaMultilevel.hpp"
#include <iostream>

int main(){
    MultilevelTreeNode* root = new MultilevelTreeNode("root");
    MultilevelTreeNode* node1 = new MultilevelTreeNode("node1");
    MultilevelTreeNode* node2 = new MultilevelTreeNode("node2");
    MultilevelTreeNode* node3 = new MultilevelTreeNode("node3");

    root->add_leaf(node1);
    root->add_leaf(node2);
    node2->add_leaf(node3);

    root->print();

    MultilevelTreeNode* lcaNode = MultilevelTreeNode::lca(node1, node3);
    std::cout << "LCA of node1 and node3: " << lcaNode->data << std::endl;

    root->deleteNode();

    return 0;
}