#include "lcaTree.hpp"
#include <iostream>
#include <deque>

using std::cout;
using std::endl;
///////////////////////////////////////////
////////    LCA for Static Trees    ///////
///////////////////////////////////////////

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
    this->assignSubtreeSizes(false);
    this->assignApex();
    this->assignLevels();
    this->assignRoot(this);

    this->compressTree(true);
    this->assignIntervals();
    this->fillAllAncestors();
    this->setPreprocessedFlag();
}

void TreeNode::recompress() {
    this->assignSubtreeSizes(false); //with compressed = false
    this->assignApex();
    isApex = true; // Treat the current node as the "root", even though it could have a parent
    this->assignLevels();

    cout << "After assigning OriginalSubtreeSizes/Apex" << endl;
    print();

    this->compressTree(true); //TODO: update with compressed = true for subtreeSizes
    if (this->parent) {
        //get largest endBuffered in this->parent->children
        cout << "    This parent: " << parent->nodeId << endl;
        cout << "    Parent largest end buffer: " << parent->largestChildEndBuffer<< endl;
        
        // assign this buffered interval <- [Q(u), Q(u) + cs(v)^e]
        startBuffered = parent->largestChildEndBuffer; //TODO: Check off by 1 error?
        endBuffered = parent->largestChildEndBuffer + c * pow(subtreeSize, e);
        parent->largestChildEndBuffer = endBuffered;
        cout << "    Assigning interval to node " << nodeId <<": [" << startBuffered << ", " << endBuffered << "]" << endl;

        // assign fat preordering to children
        contAssignIntervals();
    } else {
        this->assignIntervals();
    }
}

void TreeNode::setPreprocessedFlag() {
    isPreprocessed = true;
    for (TreeNode* child : children) {
        child->setPreprocessedFlag();
    }
}

// TODO: Take in a parameter "compressed" and switch between children lists based on that
int TreeNode::assignSubtreeSizes(bool useCompressed) {
    subtreeSize = 1;
    dynamicSubtreeSize = 1;
    std::list<TreeNode*> childrenList = useCompressed ? children : uncompressedChildren;
    for (TreeNode* child : childrenList) {
        int childSize = child->assignSubtreeSizes(useCompressed);
        subtreeSize += childSize;
        dynamicSubtreeSize += childSize;
    }
    return subtreeSize;
}

// A node is apex if it is not a a heavy child
void TreeNode::assignApex() {
    if (parent) {
        isApex = (subtreeSize * 2 <= uncompressedParent->subtreeSize);
    } else {
        isApex = true;
    }

    for (TreeNode* child : uncompressedChildren) {child->assignApex();}
}

void TreeNode::assignRoot(TreeNode* node) {
    root = node;
    for (TreeNode* child : children) {child->assignRoot(node);}
}

// uncompressedParent and uncompressedChildren and uncompressedLevel remain unchanged
// "parent", "children", and "subtreeSize" now refer to the compressed tree
void TreeNode::compressTree(bool isRoot){
    children.clear();
    // Set parent pointer

    // "new" parent?
    if (uncompressedParent) {
        if (uncompressedParent->isApex) {
            parent = uncompressedParent;
            //closestApex is already parent - do nothing (closestApex = parent;)
        } else {
            // parent has already been updated, so the parent will be the child of the correct apex node
            parent = uncompressedParent->parent;
        }
    }

    if (!isRoot && parent) {
        parent->children.push_back(this);
    }

     // else do nothing (root in original = root in compressed)
    // cout << "    Compressing node " << nodeId << ": parent now " << getId(parent) << endl;
    // Update children pointers
    for (TreeNode* child : uncompressedChildren) {child->compressTree();} //Recurse first
    if (isApex) {
        // do nothing (it keeps all its children)
    } else {
        children.clear(); //light children can have no children(?)
    }

    this->assignSubtreeSizes(true);
}

void TreeNode::assignIntervals(){
    this->startBuffered = 0;
    this->endBuffered = c * pow(this->subtreeSize, e);
    this->contAssignIntervals();
}

//TODO: make this more general by passing in a funtion "sigma" as an argument
void TreeNode::contAssignIntervals() {
    // TODO: Calculate fat preorder numbering
    long long buffer = pow(this->subtreeSize, e);
    //if (nodeId == "0"){std::cout << "subtreeSize: " << this->subtreeSize << std::endl;}
    //if (nodeId == "0"){std::cout << "e: " << e << std::endl;}
    //if (nodeId == "0"){std::cout << "BUFFER: " << buffer << std::endl;}
    start = startBuffered + buffer;
    end = endBuffered - buffer;

    largestChildEndBuffer = start; //Edge case when there are no children (children would be assigned starting at `start`)
    
    // Assign buffered intervals to children & recurse
    long long currChildStart = start + 1;
    long long intervalSize;
    for (TreeNode* child : children) {
        intervalSize = c * pow(child->subtreeSize, e);
        child->startBuffered = currChildStart;
        child->endBuffered = currChildStart + intervalSize;
        currChildStart = currChildStart + intervalSize + 1;

        child->contAssignIntervals();

        // Mantain "largest" augmented value
        largestChildEndBuffer = currChildStart + intervalSize;
    }
}


void TreeNode::fillAllAncestors(){
    // if (nodeId == "0") {
    //     std::cout << "ROOT StartBuffered: " << startBuffered << std::endl;
    //     std::cout << "ROOT Start: " << start << std::endl;
    //     std::cout << "ROOT End: " << end << std::endl;
    //     std::cout << "ROOT EndBuffered: " << endBuffered << std::endl;
    // }

    this->fillAncestorTable();
    // std::cout << "    " << nodeId << " TABLE: "<< strAncestorTable(ancestors) << std::endl;
    for (TreeNode* child : children) {
        child->fillAllAncestors();
    }
}

void TreeNode::fillAncestorTable(){
    int currTreeSize = root->subtreeSize;
    int ancestorSize = 1 + floor(log(c * pow(currTreeSize, e))/log(beta)); //add +1 as buffer?
    ancestors.resize(ancestorSize, NULL);
    for (int i = 0; i < ancestorSize; ++i)
    {
        ancestors[i] = NULL;
    }

    size_t i = 0;
    //std::cout << "table size: " << ancestors.size() << std::endl;
    TreeNode* currNode = this;
    TreeNode* nextNode = currNode->parent;

    bool currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
    while (!currIsLess) {
        i += 1;
        currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
    }

    bool nextIsMore;
    while (currNode) {
        //std::cout << "TABLE: "<< strAncestorTable(ancestors) << std::endl;
        currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
        //std::cout << "before nextIsMore" << std::endl;
        nextIsMore = !nextNode || ((c - 2) * pow(nextNode->subtreeSize, e) >= pow(beta, i));
        //std::cout << "currIsLess: " << currIsLess << ", nextIsMore: " << nextIsMore << std::endl;
        while (currIsLess && nextIsMore && i < ancestors.size()) {
            //std::cout << "inside loop" << std::endl;
            ancestors[i] = currNode;
            i += 1;
            //std::cout << "    i: "<< i << std::endl;
            //std::cout << "    TABLE: "<< strAncestorTable(ancestors) << std::endl;

            currIsLess = (c - 2) * pow(currNode->subtreeSize, e) < pow(beta, i);
            nextIsMore = !nextNode || ((c - 2) * pow(nextNode->subtreeSize, e) >= pow(beta, i));
        }

        //std::cout << "after loop" << std::endl;

        currNode = nextNode;
        nextNode = nextNode ? nextNode->parent : NULL;
    }
}

void TreeNode::assignLevels(int level) {
    this->uncompressedLevel = level;

    for (TreeNode* child : uncompressedChildren) {
        child->assignLevels(level + 1);
    }
}

////////////////////////
// Dynamic Operations //
////////////////////////
void TreeNode::add_leaf(TreeNode* leaf) {
    cout << "---" << endl;
    cout << "Inserting " << leaf->nodeId << endl;
    // Add leaf to original tree
    // cout << "    Adding uncompressed children" << endl;
    uncompressedChildren.push_back(leaf);
    leaf->uncompressedParent = this;
    leaf->root = root;

    // Set leaf path
    // cout << "    Setting path/apex" << endl;
    leaf->isApex = true;
    if (isApex) {
        children.push_back(leaf);
        leaf->parent = this;
    } else {
        leaf->parent = parent;
        parent->children.push_back(leaf); //if !isApex, `this` has a parent
    }

    // Update subtree sizes
    // cout << "    Updating dynamic subtree sizes" << endl;
    TreeNode* currNode = leaf;

    leaf->subtreeSize = 1; //TODO check - By convention, dynamicSubtreeSize >= alpha * subtreeSize holds for `leaf`: so start it at 0
    leaf->dynamicSubtreeSize = 0; //start at 0 so that we increment to 1 on the first loop
    
    while (currNode) {
        currNode->dynamicSubtreeSize += 1;
        currNode = currNode->parent;
    }

    // Record last node where
    // dynamicSubtreeSize >= alpha * subtreeSize (ie. last "broken" node)
    // cout << "    Finding last broken node" << endl;
    // cout << "    Current Compressed Tree" << endl;
    // this->root->printIntervals(0);
    // cout << "    Current Uncompressed Tree" << endl;
    // this->root->print();

    currNode = leaf; //By convention, leaf is "broken"
    bool nextIsBroken = currNode->parent && currNode->parent->dynamicSubtreeSize >= alpha * currNode->parent->subtreeSize;

    while(nextIsBroken) {
        currNode = currNode->parent;
        nextIsBroken = currNode->parent && currNode->parent->dynamicSubtreeSize >= alpha * currNode->parent->subtreeSize;
    }

    // Recompress last "broken" node and update ancestor tables
    cout << "    Last broken node: " << getId(currNode) << endl;
    // cout << "Recompressing" << endl;
    currNode->recompress();
    // cout << "    Current Tree" << endl;
    // this->root->printIntervals(0);
    // cout << "    Filling ancestor tables" << endl;
    currNode->fillAllAncestors();
    // cout << "    Setting `preprocessed` for subtree starting at " << currNode->nodeId << endl;
    currNode->setPreprocessedFlag();
    currNode->printIntervals(0);
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
    std::cout << "--------------\n after compressed: " << getId(cas.lca) << ", " << getId(cas.ca_x) << ", " << getId(cas.ca_y) << std::endl;
    
    if (cas.ca_x->inPath(cas.lca)) {
        cout << "ca_x is in path of ca_lca (ca_x is apex? " << cas.ca_x->isApex << ")" << endl;
    } else {
        cout << "ca_x is NOT in path of ca_lca (ca_x is apex? " << cas.ca_x->isApex << ")" << endl;
    }

    if (cas.ca_y->inPath(cas.lca)) {
        cout << "ca_y is in path of ca_lca (ca_y is apex? " << cas.ca_y->isApex << ")" << endl;
    } else {
        cout << "ca_y is NOT in path of ca_lca (ca_y is apex? " << cas.ca_y->isApex << ")" << endl;
    }
        
    TreeNode* b_x = (cas.ca_x->inPath(cas.lca)) ?
                     cas.ca_x : cas.ca_x->uncompressedParent;
    TreeNode* b_y = (cas.ca_y->inPath(cas.lca)) ?
                     cas.ca_y : cas.ca_y->uncompressedParent;
    cout << "b_x: " << getId(b_x) << endl;
    cout << "b_y: " << getId(b_y) << endl;
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
    std::cout  << "    size of ancestors: " << nodeX->ancestors.size() << std::endl;
    std::cout  << "    i: " << i << std::endl;
    std::cout  << "    start: " << nodeX->start << std::endl;
    std::cout  << "    end: " << nodeY->start << std::endl;
    std::cout  << "    abs(diff): " << abs(nodeX->start - nodeY->start) << std::endl;
    std::cout  << "    log_beta abs(diff): " << log(abs(nodeX->start - nodeY->start))/log(beta) << std::endl;
    TreeNode* v = nodeX->ancestors[i];
    std::cout  << "    v: " << getId(v) << std::endl;
    TreeNode* w;
    if (v) {
        w = v->parent;
    } else {
        w = nodeX;
    }
    std::cout  << "    w: " << getId(w) << std::endl;

    TreeNode* b;
    TreeNode* b_x;
    if ((c - 2) * pow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b_x = v;} else {b_x = nodeX;}
    } else {
        b = w->parent;
        b_x = w;
    }
    std::cout  << "    b: " << getId(b) << std::endl;
    std::cout  << "    b_x: " << getId(b_x) << std::endl;

    TreeNode* a;
    TreeNode* a_x;
    if (b->isAncestorOf(nodeY)) {
        std::cout << "    b is an ancestor of y" << std::endl;
        a = b;
        a_x = b_x;
    } else {
        std::cout << "    b is NOT an ancestor of y" << std::endl;
        a = b->parent;
        a_x = b;
    }

    std::cout  << "    a: " << getId(a) << std::endl;
    std::cout  << "    a_x: " << getId(a_x) << std::endl;

    // Symetric computation to compute a_y
    // TODO: Clean up this code
    // std::cout  << "    ---------SYMMETRIC WITH Y----- " << std::endl;
    v = nodeY->ancestors[i];
    // std::cout  << "    v: " << getId(v) << std::endl;

    if (v) {w = v->parent;} else {w = nodeY;}
    // std::cout  << "    w: " << getId(w) << std::endl;

    TreeNode* b_y;
    if ((c - 2) * pow(w->subtreeSize, e) > abs(nodeX->start - nodeY->start)) {
        b = w;
        if (v) {b_y = v;} else {b_y = nodeY;}
    } else {
        b = w->parent;
        b_y = w;
    }
    // std::cout  << "    b: " << getId(b) << std::endl;
    // std::cout  << "    b_y: " << getId(b_x) << std::endl;


    TreeNode* a_y;
    if (b->isAncestorOf(nodeX)) {
        // std::cout << "    b is an ancestor of x" << std::endl;
        a = b;
        a_y = b_y;
    } else {
        // std::cout << "    b is NOT an ancestor of x" << std::endl;
        a = b->parent;
        a_y = b;
    }
    // std::cout  << "    a: " << getId(a) << std::endl;
    // std::cout  << "    a_y: " << getId(a_y) << std::endl;


    caTuple toReturn;
    toReturn.lca = a;
    toReturn.ca_x = a_x;
    toReturn.ca_y = a_y;
    return(toReturn);
}

bool TreeNode::isAncestorOf(TreeNode* node) {
    // std::cout << "Checking if "<< nodeId << " isAncestor of " << getId(node) << std::endl;
    // std::cout << "    myStart: "<< start << ", nodeStart: " << node->start << std::endl;
    return (start <= node->start) && (node->start <= end);
}

TreeNode* TreeNode::naiveLca(TreeNode* nodeX, TreeNode* nodeY) {
    if (nodeX == nodeY) {
        return nodeX;
    }

    // std::deque<TreeNode*> xPathComp;
    // std::deque<TreeNode*> yPathComp;

    // TreeNode* currNode = nodeX;
    // while (currNode) {
    //     xPathComp.push_front(currNode);
    //     currNode = currNode->parent;
    // }

    // currNode = nodeY;
    // while (currNode) {
    //     yPathComp.push_front(currNode);
    //     currNode = currNode->parent;
    // }

    // std::cout << "X PATH COMP: ";
    // for (TreeNode* node : xPathComp) {
    //     std::cout << getId(node) << "->";
    // }
    // std::cout << std::endl;

    // std::cout << "Y PATH COMP: ";
    // for (TreeNode* node : yPathComp) {
    //     std::cout << getId(node) << "->";
    // }
    // std::cout << std::endl;

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

    // std::cout << "X PATH: ";
    // for (TreeNode* node : xPath) {
    //     std::cout << getId(node) << "->";
    // }
    // std::cout << std::endl;

    // std::cout << "Y PATH: ";
    // for (TreeNode* node : yPath) {
    //     std::cout << getId(node) << "->";
    // }
    // std::cout << std::endl;

    size_t i = 0;
    while (i < xPath.size() && i < yPath.size() && xPath[i] == yPath[i]) {
        i++;
    }

    TreeNode* lca = xPath[i-1];
    return(lca);
}

///////////////////////////
// Basic Tree Operations //
///////////////////////////

TreeNode::TreeNode(std::string id) {
    nodeId = id;
    parent = NULL;
    root = NULL;
    uncompressedParent = NULL;
    subtreeSize = 0;
    dynamicSubtreeSize = 0;
    isApex = false;
    uncompressedLevel = -1;
    isPreprocessed = false;

    start = 0, startBuffered = 0, end = 0, endBuffered = 0;
    largestChildEndBuffer = 0;

    //int ancestorSize = 1 + floor(log(c * pow(treeSize, e))/log(beta)); //add +1 as buffer?
    //ancestors.resize(ancestorSize, NULL);
    //for (int i = 0; i < ancestorSize; ++i) {ancestors[i] = NULL;}
}

int TreeNode::getSubtreeSize(){
    return (this->subtreeSize);
}

void TreeNode::addChild(TreeNode* child) {
    children.push_back(child);
    uncompressedChildren.push_back(child);
    child->parent = this;
    child->uncompressedParent = this;
}

// Removes edges WITHOUT freeing memory
void TreeNode::deleteNode() {
    if(uncompressedParent) {
        uncompressedParent->uncompressedChildren.remove(this);
    }

    if (parent) {
        parent->children.remove(this);
    }

    parent = NULL;
    uncompressedParent = NULL;
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
              << "level = " << uncompressedLevel << ", "
              << "preprocessed = " << isPreprocessed << ")"
              << std::endl;
    for (TreeNode* child : uncompressedChildren) {
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
              << "len = "  << (c-2) * pow(subtreeSize, e) << ", "
              << "subSize = " << subtreeSize << ", "
              << "dynamicSubSize = " << dynamicSubtreeSize << ", "
              << "isApex = " << isApex << ")"
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