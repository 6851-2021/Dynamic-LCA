#include "lcaTree.hpp"
#include <iostream>

///////////////////////////////////////////
////////    LCA for Static Trees    ///////
///////////////////////////////////////////

///////////////////
// Preprocessing //
///////////////////

// Computes x^{n}
int intpow(int x, int n) {
    int result = 1;
    for (int i = 0; i < n; i++) {result *= x;}
    return(result);
}

int TreeNode::assignSubtreeSizes() {
    subtreeSize = 1;
    for (TreeNode* child : children) {
        subtreeSize += child->assignSubtreeSizes();
    }
    return subtreeSize;
}

// A node is apex if it is not a a heavy child
void TreeNode::assignApex() {
    if (parent) {
        isApex = (subtreeSize * 2 <= parent->subtreeSize);
    } else {
        isApex = true;
    }

    for (TreeNode* child : children) {child->assignApex();}
}

// uncompressedParent and uncompressedLevel remains unchanged
// "parent", "children", and "subtreeSize" now refer to the compressed tree
void TreeNode::compressTree(){
    // Set parent pointer
    if (parent) {
        if (parent->isApex) {
            //closestApex is already parent - do nothing (closestApex = parent;)
        } else {
            // parent has already been updated, so the parent will be the child of the correct apex node
            (parent->parent)->children.push_back(this);
            parent = parent->parent;
        }
    } // else do nothing (root in original = root in compressed)

    // Update children pointers
    for (TreeNode* child : children) {child->compressTree();} //Recurse first
    if (isApex) {
        // do nothing (it keeps all its children)
    } else {
        children.clear(); //light children can have no children(?)
    }

    this->assignSubtreeSizes();
}

void TreeNode::assignIntervals(){
    this->startBuffered = 0;
    this->endBuffered = c * intpow(this->subtreeSize, e);
    this->contAssignIntervals();
}

//TODO: make this more general by passing in a funtion "sigma" as an argument
void TreeNode::contAssignIntervals() {
    // TODO: Calculate fat preorder numbering
    int buffer = intpow(this->subtreeSize, e);
    start = startBuffered + buffer;
    end = endBuffered - buffer;
    
    // Assign buffered intervals to children & recurse
    int currChildStart = start + 1;
    int intervalSize;
    for (TreeNode* child : children) {
        intervalSize = c * intpow(child->subtreeSize, e);
        child->startBuffered = currChildStart;
        child->endBuffered = currChildStart + intervalSize;
        currChildStart = currChildStart + intervalSize + 1;

        child->contAssignIntervals();
    }
}

void TreeNode::fillAllAncestors(){
    this->fillAncestorTable();
    for (TreeNode* child : children) {
        child->fillAllAncestors();
    }
}

std::string getId(TreeNode* node) {
    if (node) {return (node->nodeId);}
    else {return "NULL";}
}

std::string strAncestorTable(std::vector<TreeNode*> ancestors) {
    std::string result = "[";
    for (size_t i = 0; i < ancestors.size(); ++i)
    {
        result = result + getId(ancestors[i]) + ", ";
    }
    result = result + "]";
    return(result);
}

void TreeNode::fillAncestorTable(){
    int i = ancestors.size() - 1;
    TreeNode* currNode = this->parent;
    while (currNode) {
        while ((c - 2) * intpow(currNode->subtreeSize, e) < intpow(beta, i)) {
            ancestors[i] = currNode;
            i = i - 1;
        }
        currNode = currNode->parent;
    }
}

void TreeNode::assignLevels(int level) {
    this->uncompressedLevel = level;

    for (TreeNode* child : children) {
        child->assignLevels(level + 1);
    }
}

///////////////////////
// Answering Queries //
///////////////////////


TreeNode* TreeNode::lca(TreeNode* nodeX, TreeNode* nodeY) {
    TreeNode::caTuple cas = lcaCompressed(nodeX, nodeY);
    TreeNode* b_x = (cas.ca_x->isApex) ?
                    cas.ca_x->uncompressedParent : cas.ca_x;
    TreeNode* b_y = (cas.ca_y->isApex) ?
                    cas.ca_y->uncompressedParent : cas.ca_y;
    TreeNode* result = ((b_x->uncompressedLevel < b_y->uncompressedLevel) ?
                        b_x : b_y);

    std::cout << "LCA of " << getId(nodeX) << " and " << getId(nodeY) << ": " << getId(result) << std::endl;
    return result;
}

TreeNode::caTuple TreeNode::lcaCompressed(TreeNode* nodeX, TreeNode* nodeY) {
    int i = floor(log(abs(nodeX->start - nodeY->start))/log(beta));
    TreeNode* v = nodeX->ancestors[i];

    TreeNode* w;
    if (v) {
        w = v->parent;
    } else {
        w = nodeX;
    }

    TreeNode* b;
    TreeNode* b_x;
    if ((c - 2) * intpow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b = v;} else {b = nodeX;}
    } else {
        b = w->parent;
        b_x = w;
    }

    TreeNode* a;
    TreeNode* a_x;
    if (b->isAncestor(nodeY)) {
        a = b;
        a_x = b_x;
    } else {
        a = b->parent;
        a_x = b;
    }

    // Symetric computation to compute a_y
    // TODO: Clean up this code
    v = nodeY->ancestors[i];

    if (v) {w = v->parent;} else {w = nodeX;}

    TreeNode* b_y;
    if ((c - 2) * intpow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b = v;} else {b = nodeY;}
    } else {
        b = w->parent;
        b_y = w;
    }

    TreeNode* a_y;
    if (b->isAncestor(nodeX)) {
        a = b;
        a_y = b_y;
    } else {
        a = b->parent;
        a_y = b;
    }

    caTuple toReturn;
    toReturn.lca = a;
    toReturn.ca_x = a_x;
    toReturn.ca_y = a_y;
    //std::cout << "a: " << getId(a) << ", a_x: " << getId(a_x) << ", a_y: " << getId(a_y) << std::endl;
    return(toReturn);
}

bool TreeNode::isAncestor(TreeNode* node) {
    return (start <= node->start) && (node->start <= end);
}

///////////////////////////
// Basic Tree Operations //
///////////////////////////

TreeNode::TreeNode(std::string id, int treeSize) {
    nodeId = id;
    parent = NULL;
    uncompressedParent = NULL;
    subtreeSize = 0;
    start = 0;
    end = 0;
    isApex = false;
    closestApex = NULL;

    int ancestorSize = 1 + floor(log(c * intpow(treeSize, e))/log(beta));
    ancestors.resize(ancestorSize);
    for (int i = 0; i < ancestorSize; ++i) {ancestors[i] = NULL;}
}

int TreeNode::getSubtreeSize(){
    return (this->subtreeSize);
}

void TreeNode::addChild(TreeNode* child) {
    children.push_back(child);
    child->parent = this;
    child->uncompressedParent = this;
}

void TreeNode::print() {
    this->print(0);
}

void TreeNode::print(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId
              << "(subtree size = " << subtreeSize << ", "
              << "isApex = " << isApex << ", "
              << "parentId = " << getId(parent) << ", "
              << "uncompparentId = " << getId(uncompressedParent) << ", "
              << "level = " << uncompressedLevel << ")"
              << std::endl;
    for (TreeNode* child : children) {
        child->print(level + 1);
    }
}

void TreeNode::printIntervals(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId
              << "(startB = " << startBuffered << ", "
              << "start = " << start << ", "
              << "end = " << end << ", "
              << "endB = " << endBuffered << ")"
              << std::endl;
    for (TreeNode* child : children) {
        child->printIntervals(level + 1);
    }
}

void TreeNode::printAncestors(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId
              << strAncestorTable(this->ancestors)
              << std::endl;
    for (TreeNode* child : children) {
        child->printAncestors(level + 1);
    }
}