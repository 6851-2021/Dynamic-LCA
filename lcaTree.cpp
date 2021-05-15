#include "lcaTree.hpp"
#include <iostream>
#include <deque>

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

void TreeNode::preprocess() {
    this->assignSubtreeSizes();
    this->assignApex();
    this->assignLevels();

    this->compressTree();
    this->assignIntervals();
    this->fillAllAncestors();
    this->setPreprocessedFlag();
}

void TreeNode::setPreprocessedFlag() {
    isPreprocessed = true;
    for (TreeNode* child : children) {
        child->setPreprocessedFlag();
    }
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
    //std::cout << "FILLING ANCESTOR TABLE " << std::endl;
    size_t i = 0;
    //std::cout << "table size: " << ancestors.size() << std::endl;
    TreeNode* currNode = this;
    TreeNode* nextNode = currNode->parent;

    bool currIsLess = (c - 2) * intpow(currNode->subtreeSize, e) < intpow(beta, i);
    while (!currIsLess) {
        i += 1;
        currIsLess = (c - 2) * intpow(currNode->subtreeSize, e) < intpow(beta, i);
    }

    bool nextIsMore;
    while (currNode) {
        //std::cout << "TABLE: "<< strAncestorTable(ancestors) << std::endl;
        currIsLess = (c - 2) * intpow(currNode->subtreeSize, e) < intpow(beta, i);
        //std::cout << "before nextIsMore" << std::endl;
        nextIsMore = !nextNode || ((c - 2) * intpow(nextNode->subtreeSize, e) >= intpow(beta, i));
        //std::cout << "currIsLess: " << currIsLess << ", nextIsMore: " << nextIsMore << std::endl;
        while (currIsLess && nextIsMore && i < ancestors.size()) {
            //std::cout << "inside loop" << std::endl;
            ancestors[i] = currNode;
            i += 1;
            //std::cout << "    i: "<< i << std::endl;
            //std::cout << "    TABLE: "<< strAncestorTable(ancestors) << std::endl;

            currIsLess = (c - 2) * intpow(currNode->subtreeSize, e) < intpow(beta, i);
            nextIsMore = !nextNode || ((c - 2) * intpow(nextNode->subtreeSize, e) >= intpow(beta, i));
        }

        //std::cout << "after loop" << std::endl;

        currNode = nextNode;
        nextNode = nextNode ? nextNode->parent : NULL;
    }
}


// void TreeNode::fillAncestorTable(){
//     int i = ancestors.size() - 1;
//     TreeNode* currNode = this->parent;
//     while (currNode) {
//         while ((c - 2) * intpow(currNode->subtreeSize, e) < intpow(beta, i)) {
//             ancestors[i] = currNode;
//             i = i - 1;
//         }
//         currNode = currNode->parent;
//     }
// }

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
    if (!nodeX->isPreprocessed or !nodeY->isPreprocessed) {
        std::cout << "Error: Tree must be preprocessed before calling LCA." << std::endl;
        exit(-1);
    } else if (nodeX == nodeY) {
        return nodeX;
    }

    //std::cout << "before compressed" << std::endl;
    TreeNode::caTuple cas = lcaCompressed(nodeX, nodeY);
    //std::cout << "after compressed: " << getId(cas.lca) << ", " << getId(cas.ca_x) << ", " << getId(cas.ca_y) << std::endl;
    TreeNode* b_x = (cas.ca_x->inPath(cas.lca)) ?
                     cas.ca_x : cas.ca_x->uncompressedParent;
    TreeNode* b_y = (cas.ca_y->inPath(cas.lca)) ?
                     cas.ca_y : cas.ca_y->uncompressedParent;
    TreeNode* result = ((b_x->uncompressedLevel < b_y->uncompressedLevel) ?
                        b_x : b_y);

    return result;
}

bool TreeNode::inPath(TreeNode* apex) {
    if (this == apex) {
        return true;
    } else {
        return (!isApex && (parent == apex)); // "parent" refers to compressed parent
    }
}

// Note: nodeX cannot be equal to nodeY
TreeNode::caTuple TreeNode::lcaCompressed(TreeNode* nodeX, TreeNode* nodeY) {
    assert(nodeX != nodeY);

    if (!nodeX->isPreprocessed or !nodeY->isPreprocessed) {
        std::cout << "Error: Tree must be preprocessed before calling LCA." << std::endl;
        exit(-1);
    }

    int i = floor(log(abs(nodeX->start - nodeY->start))/log(beta));
    //std::cout  << "    i: " << i << std::endl;
    TreeNode* v = nodeX->ancestors[i];
    //std::cout  << "    v: " << getId(v) << std::endl;
    TreeNode* w;
    if (v) {
        w = v->parent;
    } else {
        w = nodeX;
    }
    //std::cout  << "    w: " << getId(w) << std::endl;

    TreeNode* b;
    TreeNode* b_x;
    if ((c - 2) * intpow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b_x = v;} else {b_x = nodeX;}
    } else {
        b = w->parent;
        b_x = w;
    }
    //std::cout  << "    b: " << getId(b) << std::endl;
    //std::cout  << "    b_x: " << getId(b_x) << std::endl;

    TreeNode* a;
    TreeNode* a_x;
    if (b->isAncestor(nodeY)) {
        //std::cout << "    b is an ancestor of y" << std::endl;
        a = b;
        a_x = b_x;
    } else {
        //std::cout << "    b is NOT an ancestor of y" << std::endl;
        a = b->parent;
        a_x = b;
    }

    //std::cout  << "    a: " << getId(a) << std::endl;
    //std::cout  << "    a_x: " << getId(a_x) << std::endl;

    // Symetric computation to compute a_y
    // TODO: Clean up this code
    //std::cout  << "    ---------SYMMETRIC WITH Y----- " << std::endl;
    v = nodeY->ancestors[i];
    //std::cout  << "    v: " << getId(v) << std::endl;

    if (v) {w = v->parent;} else {w = nodeY;}
    //std::cout  << "    w: " << getId(w) << std::endl;

    TreeNode* b_y;
    if ((c - 2) * intpow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b_y = v;} else {b_y = nodeY;}
    } else {
        b = w->parent;
        b_y = w;
    }
    //std::cout  << "    b: " << getId(b) << std::endl;
    //std::cout  << "    b_y: " << getId(b_x) << std::endl;


    TreeNode* a_y;
    if (b->isAncestor(nodeX)) {
        //std::cout << "    b is an ancestor of x" << std::endl;
        a = b;
        a_y = b_y;
    } else {
        //std::cout << "    b is NOT an ancestor of x" << std::endl;
        a = b->parent;
        a_y = b;
    }
    //std::cout  << "    a: " << getId(a) << std::endl;
    //std::cout  << "    a_x: " << getId(a_x) << std::endl;


    caTuple toReturn;
    toReturn.lca = a;
    toReturn.ca_x = a_x;
    toReturn.ca_y = a_y;
    return(toReturn);
}

bool TreeNode::isAncestor(TreeNode* node) {
    return (start <= node->start) && (node->start <= end);
}

TreeNode* TreeNode::naiveLca(TreeNode* nodeX, TreeNode* nodeY) {
    if (nodeX == nodeY) {
        return nodeX;
    }
    std::deque<TreeNode*> xPath;
    std::deque<TreeNode*> yPath;
    
    TreeNode* currNode = nodeX;
    while (currNode) {
        xPath.push_front(currNode);
        currNode = currNode->uncompressedParent;
    }

    currNode = nodeY;
    while (currNode) {
        yPath.push_front(currNode);
        currNode = currNode->uncompressedParent;
    }

    int i = 0;
    while (xPath[i] == yPath[i] && i < xPath.size() && i < yPath.size()) {
        i++;
    }

    TreeNode* lca = xPath[i-1];
    return(lca);
}

///////////////////////////
// Basic Tree Operations //
///////////////////////////

TreeNode::TreeNode(std::string id, int treeSize) {
    nodeId = id;
    parent = NULL;
    uncompressedParent = NULL;
    subtreeSize = 0;
    isApex = false;
    uncompressedLevel = -1;
    isPreprocessed = false;

    start = 0, startBuffered = 0, end = 0, endBuffered = 0;

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
              << "endB = " << endBuffered << ", "
              << "len = "  << (c-2) * intpow(subtreeSize, 2) << ")"
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