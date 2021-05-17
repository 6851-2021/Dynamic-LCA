#include "lcaMultilevel.hpp"
#include <iostream>
#include <bitset>
#include <deque>

void MultilevelTreeNode::add_leaf(MultilevelTreeNode* leaf) {
    children.push_back(leaf);
    leaf->parent = this;

    if (twoSubtreeRoot->twoSubtreeSize == twoSubtreeMaxSize) {
        // Case 1: x-hat was full
        // `leaf` should be made the leaf of a new subtree
        leaf->twoSubtreeRoot = leaf;
        leaf->twoSubtreeSize = 1;
        leaf->summaryNode = NULL;
        leaf->intToSubtreeNode.push_back(leaf);
        leaf->ancestorWord = 1;
    } else {
        // Case 2: x-hat was not previously full
        // Add `leaf` to this subtree & update root->twoSubtreeSize

        // Everything is 0 indexed, so do this operation before incrememnting subtreeSize
        leaf->ancestorWord = ancestorWord + (1 << (twoSubtreeRoot->twoSubtreeSize));
        std::cout << "Adding leaf with ancestor word ancestor " << std::bitset<32>(leaf->ancestorWord) << std::endl;
        twoSubtreeRoot->intToSubtreeNode.push_back(leaf);

        leaf->twoSubtreeRoot = twoSubtreeRoot;
        twoSubtreeRoot->twoSubtreeSize += 1;


        if (twoSubtreeRoot->twoSubtreeSize == twoSubtreeMaxSize) {
            // If subtree is now full...
            ExpensiveTreeNode* currSummary = new ExpensiveTreeNode(twoSubtreeRoot->data);
            twoSubtreeRoot->summaryNode = currSummary;
            currSummary->preprocess(); //TODO: fix
            if (twoSubtreeRoot->parent) {
                ExpensiveTreeNode* parentSummary = twoSubtreeRoot->parent->twoSubtreeRoot->summaryNode;
                parentSummary->add_leaf(currSummary);
            } // Otherwise, summaryNode is the root...
            // TODO: use create_tree from ExpensiveTreeNode
        }
    }
}

MultilevelTreeNode* MultilevelTreeNode::lca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY) {
    if (nodeX->twoSubtreeRoot != nodeY->twoSubtreeRoot) {
        //Make them the same...
        std::cout << "ERROR: NOT YET IMPLEMENTED " << std::endl;
        return NULL;
    }
    // LCA query on the 2-subtree
    return lcaWithinSubtree(nodeX, nodeY);
}

MultilevelTreeNode* MultilevelTreeNode::lcaWithinSubtree(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY) {
    assert(nodeX->twoSubtreeRoot == nodeY->twoSubtreeRoot);

    if(nodeX == nodeY) {
        return nodeX;
    }

    unsigned int differences = nodeX->ancestorWord & nodeY->ancestorWord;
    int msb = 31-__builtin_clz(differences);
    std::cout << "    MSB: " << msb << std::endl; 
    std::cout << "    intToSubtree: [";
    for (MultilevelTreeNode* node : nodeX->twoSubtreeRoot->intToSubtreeNode) {
        std::cout << node->data << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "    Returning node: " << nodeX->twoSubtreeRoot->intToSubtreeNode[msb]->data << std::endl; 

    return (nodeX->twoSubtreeRoot->intToSubtreeNode[msb]);
}

std::string nodeData(MultilevelTreeNode* node) {
    if (node) {
        return node->data;
    } else {
        return "NULL";
    }
}


void MultilevelTreeNode::print(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << data
              << "(twoSubtreeRoot = " << nodeData(twoSubtreeRoot) << ", "
              << "twoSubtreeSize = " << twoSubtreeSize << ", "
              << "summaryNode = " << summaryNode << ", "
              << "ancestorWord = " << std::bitset<32>(ancestorWord) << ")"
              << std::endl;
    for (MultilevelTreeNode* child : children) {
        child->print(level + 1);
    }
}

MultilevelTreeNode::MultilevelTreeNode(std::string id) {
        data = id;
        twoSubtreeSize = 1;
        twoSubtreeRoot = this;
        summaryNode = NULL;
        ancestorWord = 1;

        parent = NULL;
        intToSubtreeNode.push_back(this);
}

// Slightly modified from ExpensiveTreeNode::naiveCas
MultilevelTreeNode* MultilevelTreeNode::naiveLca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY) {
    if (nodeX == nodeY) {
        return(nodeX);
    }

    std::deque<MultilevelTreeNode*> xPath;
    std::deque<MultilevelTreeNode*> yPath;
    
    MultilevelTreeNode* currNode = nodeX;
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

    MultilevelTreeNode* lca = xPath[i-1];    
    return (lca);
}

void MultilevelTreeNode::deleteTree() {
    if (summaryNode) {
        delete summaryNode;
    }

    for(MultilevelTreeNode* child : children) {
        child->deleteTree();
    }

    delete this;
}