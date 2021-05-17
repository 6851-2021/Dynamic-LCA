#include "lcaTree.hpp"
#include <iostream>
#include <deque>

using std::cout;
using std::endl;


///////////////////////////////////////////
//////         Helper Methods       ///////
///////////////////////////////////////////

std::string getId(ExpensiveTreeNode* node) {
    if (node) {return (node->nodeId);}
    else {return "NULL";}
}

std::string strAncestorTable(std::vector<ExpensiveTreeNode*> ancestors) {
    std::string result = "[";
    for (size_t i = 0; i < ancestors.size(); ++i)
    {
        result = result + getId(ancestors[i]) + ", ";
    }
    result = result + "]";
    return(result);
}


///////////////////////////////////////////
////////    LCA for Static Trees    ///////
///////////////////////////////////////////

///////////////////
// Preprocessing //
///////////////////


void ExpensiveTreeNode::preprocess() {
    assignSubtreeSizes(false); // Subtree sizes based on uncompressed values
    assignApex(true);
    assignLevels(0);
    assignRoot(this);

    compressTree(true);
    assignSubtreeSizes(true); // Subtree sizes based on compressed values
    
    assignIntervals();
    fillAllAncestors();
    setPreprocessedFlag();
}

void ExpensiveTreeNode::recompress() {
    assignSubtreeSizes(false);
    assignApex(true); // Treat the current node as the "root"

    compressTree(true);
    assignSubtreeSizes(true);
    if (parent) {
        // assign this buffered interval <- [Q(u), Q(u) + cs(v)^e]
        startBuffered = parent->largestChildEndBuffer;
        endBuffered = parent->largestChildEndBuffer +
                      c * pow(subtreeSize, e);
        parent->largestChildEndBuffer = endBuffered;

        // assign fat preordering to children
        contAssignIntervals();
    } else {
        assignIntervals();
    }
}

void ExpensiveTreeNode::setPreprocessedFlag() {
    isPreprocessed = true;
    for (ExpensiveTreeNode* child : children) {
        child->setPreprocessedFlag();
    }
}

int ExpensiveTreeNode::assignSubtreeSizes(bool useCompressed) {
    subtreeSize = 1;
    dynamicSubtreeSize = 1;
    std::list<ExpensiveTreeNode*> childrenList = useCompressed ? children : uncompressedChildren;
    for (ExpensiveTreeNode* child : childrenList) {
        int childSize = child->assignSubtreeSizes(useCompressed);
        subtreeSize += childSize;
        dynamicSubtreeSize += childSize;
    }
    return subtreeSize;
}

// A node is apex if it is not a heavy child
void ExpensiveTreeNode::assignApex(bool isRoot) {
    // Assign apex based on subtreeSize
    if (isRoot) {
        isApex = true;
    } else {
        isApex = (subtreeSize * 2 <= uncompressedParent->subtreeSize);
    }

    // Update parent's heavyChild pointer if necessary
    if (!isApex) {
        uncompressedParent->heavyChild = this;
    }

    for (ExpensiveTreeNode* child : uncompressedChildren) {child->assignApex(false);}
}

void ExpensiveTreeNode::assignRoot(ExpensiveTreeNode* node) {
    root = node;
    for (ExpensiveTreeNode* child : children) {child->assignRoot(node);}
}

// uncompressedParent and uncompressedChildren and uncompressedLevel remain unchanged
// "parent", "children", and "subtreeSize" now refer to the compressed tree
void ExpensiveTreeNode::compressTree(bool isRoot){
    children.clear();

    if (uncompressedParent) { // if root, do nothing (root in original => root in compressec)
        if (uncompressedParent->isApex) {
            //The closest apex is already parent in uncompressed tree
            parent = uncompressedParent;
        } else {
            // note that uncompressedParent has already been updated
            parent = uncompressedParent->parent;
        }
    }

    if (!isRoot && parent) {
        parent->children.push_back(this);
    }

    // Recursively update
    for (ExpensiveTreeNode* child : uncompressedChildren) {child->compressTree();}
}

void ExpensiveTreeNode::assignIntervals(){
    this->startBuffered = 0;
    this->endBuffered = c * pow(this->subtreeSize, e);
    this->contAssignIntervals();
}

void ExpensiveTreeNode::contAssignIntervals() {
    // Calculate fat preorder numbering
    long long int buffer = pow(this->subtreeSize, e);
    start = startBuffered + buffer;
    end = endBuffered - buffer;

    largestChildEndBuffer = start; //Edge case when there are no children
    
    // Assign buffered intervals to children & recurse
    long long int currChildStart = start + 1;
    long long int intervalSize;
    for (ExpensiveTreeNode* child : children) {
        intervalSize = c * pow(child->subtreeSize, e);
        child->startBuffered = currChildStart;
        child->endBuffered = currChildStart + intervalSize;
        currChildStart = currChildStart + intervalSize + 1;

        child->contAssignIntervals();

        // Mantain "largest" augmented value
        largestChildEndBuffer = currChildStart + intervalSize;
    }
}


void ExpensiveTreeNode::fillAllAncestors(){
    this->fillAncestorTable();
    for (ExpensiveTreeNode* child : children) {
        child->fillAllAncestors();
    }
}

void ExpensiveTreeNode::fillAncestorTable(){
    int currTreeSize = root->subtreeSize;
    int ancestorSize = 1 + floor(log(c * pow(currTreeSize, e))/log(beta)); //add +1 as buffer?
    ancestors.resize(ancestorSize, NULL);
    for (int i = 0; i < ancestorSize; ++i)
    {
        ancestors[i] = NULL;
    }

    size_t i = 0;
    ExpensiveTreeNode* currNode = this;
    ExpensiveTreeNode* nextNode = currNode->parent;

    bool currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
    while (!currIsLess) {
        i += 1;
        currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
    }

    bool nextIsMore;
    while (currNode) {
        currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
        nextIsMore = !nextNode || ((c - 2) * pow(nextNode->subtreeSize, e) >= pow(beta, i));
        while (currIsLess && nextIsMore && i < ancestors.size()) {
            ancestors[i] = currNode;
            i += 1;

            currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
            nextIsMore = !nextNode || ((c - 2) * pow(nextNode->subtreeSize, e) >= pow(beta, i));
        }

        currNode = nextNode;
        nextNode = nextNode ? nextNode->parent : NULL;
    }
}

void ExpensiveTreeNode::assignLevels(int level) {
    this->uncompressedLevel = level;

    for (ExpensiveTreeNode* child : uncompressedChildren) {
        child->assignLevels(level + 1);
    }
}

////////////////////////
// Dynamic Operations //
////////////////////////
void ExpensiveTreeNode::add_leaf(ExpensiveTreeNode* leaf) {
    // Add leaf to original tree
    uncompressedChildren.push_back(leaf);
    leaf->uncompressedParent = this;
    leaf->root = root;
    leaf->uncompressedLevel = uncompressedLevel + 1;

    // Set leaf path
    leaf->isApex = true;
    if (isApex) {
        children.push_back(leaf);
        leaf->parent = this;
    } else {
        leaf->parent = parent;
        parent->children.push_back(leaf); //if !isApex, `this` has a parent
    }

    // Update subtree sizes
    ExpensiveTreeNode* currNode = leaf;

    leaf->subtreeSize = 1;
    leaf->dynamicSubtreeSize = 0; // Start at 0 so that we increment to 1 on the first loop
    
    while (currNode) {
        currNode->dynamicSubtreeSize += 1;
        currNode = currNode->parent;
    }

    // Record last node where
    // dynamicSubtreeSize >= alpha * subtreeSize (ie. last "broken" node)
    currNode = leaf; //By convention, the leaf is "broken"
    bool nextIsBroken = currNode->parent && currNode->parent->dynamicSubtreeSize >= alpha * currNode->parent->subtreeSize;

    while(nextIsBroken) {
        currNode = currNode->parent;
        nextIsBroken = currNode->parent && currNode->parent->dynamicSubtreeSize >= alpha * currNode->parent->subtreeSize;
    }

    // Recompress last "broken" node and update ancestor tables
    // cout << "--> Adding leaf " << leaf->nodeId << " to " << nodeId << ": updating subtree " << getId(currNode) << endl;
    currNode->recompress();
    currNode->fillAllAncestors();
    currNode->setPreprocessedFlag();
}


///////////////////////
// Answering Queries //
///////////////////////


ExpensiveTreeNode* ExpensiveTreeNode::lca(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY) {
    ExpensiveTreeNode::caTuple allCas = cas(nodeX, nodeY);
    return allCas.lca;
}

ExpensiveTreeNode::caTuple ExpensiveTreeNode::cas(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY) {
    if (!nodeX->isPreprocessed or !nodeY->isPreprocessed) {
        std::cout << "Error: Tree must be preprocessed before calling LCA." << std::endl;
        exit(-1);
    } else if (nodeX == nodeY) {
        ExpensiveTreeNode::caTuple result = {nodeX, nodeX, nodeX};
        return result;
    }

    ExpensiveTreeNode::caTuple compressedCas = casCompressed(nodeX, nodeY);
            
    // Find LCA from compressed CAs
    ExpensiveTreeNode* b_x = (compressedCas.ca_x->inPath(compressedCas.lca)) ?
                     compressedCas.ca_x : compressedCas.ca_x->uncompressedParent;
    ExpensiveTreeNode* b_y = (compressedCas.ca_y->inPath(compressedCas.lca)) ?
                     compressedCas.ca_y : compressedCas.ca_y->uncompressedParent;
    ExpensiveTreeNode* lca = ((b_x->uncompressedLevel < b_y->uncompressedLevel) ?
                        b_x : b_y);

    // Find CA_X from compressed CAs
    ExpensiveTreeNode* ca_x;
    if (lca != b_x) {
        // In this case, lca must have a heavy child (cannot be NULL)
        ca_x = lca->heavyChild;
    } else if (lca == b_x && lca != compressedCas.ca_x) {
        ca_x = compressedCas.ca_x;
    } else {
        // ie. lca == b_x && lca == compressedCas.ca_x
        ca_x = nodeX;
    }

    // Symmetrically, find CA_Y from compressed CAs
    ExpensiveTreeNode* ca_y;
    if (lca != b_y) {
        ca_y = lca->heavyChild;
    } else if (lca == b_y && lca != compressedCas.ca_y) {
        ca_y = compressedCas.ca_y;
    } else {
        ca_y = nodeY;
    }

    // Return result
    ExpensiveTreeNode::caTuple result = {lca, ca_x, ca_y};
    return result;
}

bool ExpensiveTreeNode::inPath(ExpensiveTreeNode* apex) {
    if (this == apex) {
        return true;
    } else {
        return (!isApex && (parent == apex)); // "parent" refers to compressed parent
    }
}

// Note: nodeX cannot be equal to nodeY
//
// Implementation based on Fig. 2 of Gabow's paper
// "A Data Structure for Nearest Common Ancestors with Linking"
ExpensiveTreeNode::caTuple ExpensiveTreeNode::casCompressed(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY) {
    assert(nodeX != nodeY);

    if (!nodeX->isPreprocessed or !nodeY->isPreprocessed) {
        std::cout << "Error: Tree must be preprocessed before calling LCA." << std::endl;
        exit(-1);
    }

    int i = floor(log(abs(nodeX->start - nodeY->start))/log(beta));
    ExpensiveTreeNode* v = nodeX->ancestors[i];
    ExpensiveTreeNode* w;
    if (v) {
        w = v->parent;
    } else {
        w = nodeX;
    }

    ExpensiveTreeNode* b;
    ExpensiveTreeNode* b_x;
    if ((c - 2) * pow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b_x = v;} else {b_x = nodeX;}
    } else {
        b = w->parent;
        b_x = w;
    }

    ExpensiveTreeNode* a;
    ExpensiveTreeNode* a_x;
    if (b->isAncestorOf(nodeY)) {
        a = b;
        a_x = b_x;
    } else {
        a = b->parent;
        a_x = b;
    }

    // Symetric computation to compute a_y
    // TODO: Clean up this code
    v = nodeY->ancestors[i];
    if (v) {w = v->parent;} else {w = nodeY;}

    ExpensiveTreeNode* b_y;
    if ((c - 2) * pow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b_y = v;} else {b_y = nodeY;}
    } else {
        b = w->parent;
        b_y = w;
    }

    ExpensiveTreeNode* a_y;
    if (b->isAncestorOf(nodeX)) {
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
    return(toReturn);
}

bool ExpensiveTreeNode::isAncestorOf(ExpensiveTreeNode* node) {
    return (start <= node->start) && (node->start <= end);
}

ExpensiveTreeNode* ExpensiveTreeNode::naiveLca(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY) {
    ExpensiveTreeNode::caTuple allCas = naiveCas(nodeX, nodeY);
    return(allCas.lca);
}

ExpensiveTreeNode::caTuple ExpensiveTreeNode::naiveCas(ExpensiveTreeNode* nodeX, ExpensiveTreeNode* nodeY) {
    if (nodeX == nodeY) {
        ExpensiveTreeNode::caTuple toReturn = {nodeX, nodeX, nodeX};
        return(toReturn);
    }

    std::deque<ExpensiveTreeNode*> xPath;
    std::deque<ExpensiveTreeNode*> yPath;
    
    ExpensiveTreeNode* currNode = nodeX;
    while (currNode) {
        xPath.push_front(currNode);
        currNode = currNode->uncompressedParent;
    }

    currNode = nodeY;
    while (currNode) {
        yPath.push_front(currNode);
        currNode = currNode->uncompressedParent;
    }

    size_t i = 0;
    while (i < xPath.size() && i < yPath.size() && xPath[i] == yPath[i]) {
        i++;
    }

    ExpensiveTreeNode* lca = xPath[i-1];
    ExpensiveTreeNode* ca_x = i < xPath.size() ? xPath[i] : xPath[xPath.size() - 1];
    ExpensiveTreeNode* ca_y = i < yPath.size() ? yPath[i] : yPath[yPath.size() - 1];
    ExpensiveTreeNode::caTuple toReturn = {lca, ca_x, ca_y};
    
    return (toReturn);
}

///////////////////////////
// Basic Tree Operations //
///////////////////////////

void ExpensiveTreeNode::init(std::string id) {
    nodeId = id;
    parent = NULL;
    root = NULL;
    uncompressedParent = NULL;
    subtreeSize = 0;
    dynamicSubtreeSize = 0;
    isApex = false;
    heavyChild = NULL;
    uncompressedLevel = -1;
    isPreprocessed = false;

    start = 0, startBuffered = 0, end = 0, endBuffered = 0;
    largestChildEndBuffer = 0;

    associatedTwoSubtree = NULL;

}

// ExpensiveTreeNode::ExpensiveTreeNode* ExpensiveTreeNode::createRoot(std::string id) {
//     ExpensiveTreeNode::
// }

ExpensiveTreeNode::ExpensiveTreeNode(std::string id) {
    init(id);
}

ExpensiveTreeNode::ExpensiveTreeNode(std::string id, MultilevelTreeNode* twoSubtree) {
    init(id);
    associatedTwoSubtree = twoSubtree;
    preprocess();
}


void ExpensiveTreeNode::addLeafNoPreprocessing(ExpensiveTreeNode* child) {
    children.push_back(child);
    uncompressedChildren.push_back(child);
    child->parent = this;
    child->uncompressedParent = this;
}

void ExpensiveTreeNode::deleteNode() {
    if(uncompressedParent) {
        uncompressedParent->uncompressedChildren.remove(this);
    }

    if (parent) {
        parent->children.remove(this);
    }

    for (ExpensiveTreeNode* child : uncompressedChildren) {
        child->deleteNode();
    }
    
    delete this;
}

void ExpensiveTreeNode::print() {
    this->print(0);
}

void ExpensiveTreeNode::print(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId
              << "(subtree size = " << subtreeSize << ", "
              << "isApex = " << isApex << ", "
              << "parentId = " << getId(parent) << ", "
              << "uncompparentId = " << getId(uncompressedParent) << ", "
              << "level = " << uncompressedLevel << ", "
              << "heavyChild = " << getId(heavyChild) << ")"
              << std::endl;
    for (ExpensiveTreeNode* child : uncompressedChildren) {
        child->print(level + 1);
    }
}

void ExpensiveTreeNode::printIntervals(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId
              << "(startB = " << startBuffered << ", "
              << "start = " << start << ", "
              << "end = " << end << ", "
              << "endB = " << endBuffered << ", "
              << "len = "  << (c-2) * pow(subtreeSize, e) << ", "
              << "subSize = " << subtreeSize << ", "
              << "dynamicSubSize = " << dynamicSubtreeSize << ", "
              << "isApex = " << isApex << ")"
              << std::endl;
    for (ExpensiveTreeNode* child : children) {
        child->printIntervals(level + 1);
    }
}

void ExpensiveTreeNode::printAncestors(int level) {
    for (int i = 0; i < level; i++){
        std::cout << "    ";
    }

    std::cout << "Node " << nodeId
              << strAncestorTable(this->ancestors)
              << std::endl;
    for (ExpensiveTreeNode* child : children) {
        child->printAncestors(level + 1);
    }
}