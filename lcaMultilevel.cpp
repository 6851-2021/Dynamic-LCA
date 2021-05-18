#include "lcaMultilevel.hpp"
#include <iostream>
#include <bitset>
#include <deque>


void MultilevelTreeNode::add_leaf(MultilevelTreeNode* leaf) {
    children.push_back(leaf);
    leaf->parent = this;

    if (twoSubtreeRoot->twoSubtreeSize == twoSubtreeMaxSize) {
        // Case 1: subtree containing x was full
        // `leaf` should be made the leaf of a new subtree
        leaf->twoSubtreeRoot = leaf;
        leaf->twoSubtreeSize = 1;
        leaf->summaryNode = NULL;
        // leaf->intToSubtreeNode already holds leaf - it does not need to be modified
        leaf->ancestorWord = 1;
    } else {
        // Case 2: subtree containing x was not previously full
        // Add `leaf` to this subtree & update root->twoSubtreeSize

        // Everything is 0 indexed, so do this operation before incrememnting subtreeSize
        unsigned long long curr_bit = 1; // Immediately doing (1 << ...) will treat 1 as 32 bits
        leaf->ancestorWord = ancestorWord + (curr_bit << twoSubtreeRoot->twoSubtreeSize);
        twoSubtreeRoot->intToSubtreeNode.push_back(leaf);

        leaf->twoSubtreeRoot = twoSubtreeRoot;
        twoSubtreeRoot->twoSubtreeSize += 1;


        if (twoSubtreeRoot->twoSubtreeSize == twoSubtreeMaxSize) {
            // If subtree is now full...
            ExpensiveTreeNode* currSummary = new ExpensiveTreeNode(twoSubtreeRoot->data, twoSubtreeRoot);
            twoSubtreeRoot->summaryNode = currSummary;
            if (twoSubtreeRoot->parent) {
                ExpensiveTreeNode* parentSummary = twoSubtreeRoot->parent->twoSubtreeRoot->summaryNode;
                parentSummary->add_leaf(currSummary);
            } // Otherwise, summaryNode is the root: leave parent as NULL
        }
    }
}

MultilevelTreeNode* MultilevelTreeNode::lca(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY) {
    MultilevelTreeNode* x = nodeX;
    MultilevelTreeNode* y = nodeY;

    if (x->twoSubtreeRoot != y->twoSubtreeRoot) {
        // If x and y do not belong to the same 2-subtree, 
        // use the summary tree to change x and y so that they do

        // If x-hat is not full, set x to full parent 
        if (x->twoSubtreeRoot->twoSubtreeSize < twoSubtreeMaxSize) {
            x = x->twoSubtreeRoot->parent;
        }

        // If y-hat is not full, set y to full parent
        if (y->twoSubtreeRoot->twoSubtreeSize < twoSubtreeMaxSize) {
            y = y->twoSubtreeRoot->parent;
        }

        // LCA on summary tree
        ExpensiveTreeNode* xSummary = x->twoSubtreeRoot->summaryNode;
        ExpensiveTreeNode* ySummary = y->twoSubtreeRoot->summaryNode;
        ExpensiveTreeNode::caTuple summaryCas = ExpensiveTreeNode::cas(xSummary, ySummary);

        if (summaryCas.lca != summaryCas.ca_x) {
            x = summaryCas.ca_x->associatedTwoSubtree->parent;
        }
        if (summaryCas.lca != summaryCas.ca_y) {
            y = summaryCas.ca_y->associatedTwoSubtree->parent;
        }
    }

    // LCA query on the 2-subtree
    MultilevelTreeNode* lcaNode = lcaWithinSubtree(x, y);

    return lcaNode;
}

MultilevelTreeNode* MultilevelTreeNode::lcaWithinSubtree(MultilevelTreeNode* nodeX, MultilevelTreeNode* nodeY) {
    assert(nodeX->twoSubtreeRoot == nodeY->twoSubtreeRoot);

    if(nodeX == nodeY) {
        return nodeX;
    }

    unsigned long long differences = nodeX->ancestorWord & nodeY->ancestorWord;

    // Based on https://github.com/6851-2021/MostSignificantSetBit,
    // 63-__builtin_clzll(word) calculates the MSSB
    int msb = 63-__builtin_clzll(differences);

    return (nodeX->twoSubtreeRoot->intToSubtreeNode[msb]);
}

std::string nodeData(MultilevelTreeNode* node) {
    if (node) {
        return node->data;
    } else {
        return "NULL";
    }
}


void MultilevelTreeNode::print(int level, bool details) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << data;
    if (details){
        std:: cout << "(twoSubtreeRoot = " << nodeData(twoSubtreeRoot) << ", "
                   << "twoSubtreeSize = " << twoSubtreeSize << ", "
                   << "summaryNode = " << summaryNode << ", "
                   << "ancestorWord = " << std::bitset<64>(ancestorWord) << ")";
    }
    std::cout << std::endl;

    for (MultilevelTreeNode* child : children) {
        child->print(level + 1, details);
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