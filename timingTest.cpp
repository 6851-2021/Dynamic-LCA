#include <list>
#include <string>
#include <iostream>
#include <chrono>
#include "lcaTree.hpp"
#include "generateRandTrees.hpp"
#include "lcaMultilevel.hpp"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::microseconds;
using std::chrono::nanoseconds;

template <typename T>
struct treeAndTiming {
    T* tree;
    std::vector<T*> nodes;
    int staticCreation;
    int staticPreprocessing;
    int staticTotal;
    int incrementalTotal;
    int multilevelTotal;
};

/* Times the creation of a static tree */
treeAndTiming<ExpensiveTreeNode> seqToStaticTree(std::vector<int> leaves, std::vector<int> parents) {
    auto t1 = high_resolution_clock::now();

    int numNodes = leaves.size() + 1;
    std::vector<ExpensiveTreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new ExpensiveTreeNode(std::to_string(i));
    }

    ExpensiveTreeNode* root = nodes[parents[parents.size() - 1]];

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->addLeafNoPreprocessing(nodes[leaves[i]]);
    }

    auto t2 = high_resolution_clock::now();
    
    root->preprocess();
    
    auto t3 = high_resolution_clock::now();

    auto creating = duration_cast<microseconds>(t2 - t1);
    auto processing = duration_cast<microseconds>(t3 - t2);

    treeAndTiming<ExpensiveTreeNode> toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    toReturn.staticCreation = creating.count();
    toReturn.staticPreprocessing = processing.count();
    toReturn.staticTotal = creating.count() + processing.count();
    return toReturn;
}


/* Times the creation of an ExpensiveTreeNode tree built with add_leaf */
treeAndTiming<ExpensiveTreeNode> seqToIncrementalTree(std::vector<int> leaves, std::vector<int> parents) {
    auto t1 = high_resolution_clock::now();

    int numNodes = leaves.size() + 1;
    std::vector<ExpensiveTreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new ExpensiveTreeNode(std::to_string(i));
    }

    ExpensiveTreeNode* root = nodes[parents[parents.size() - 1]];
    root->preprocess();

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }

    auto t2 = high_resolution_clock::now();
    auto total = duration_cast<microseconds>(t2 - t1);
    
    treeAndTiming<ExpensiveTreeNode> toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    toReturn.incrementalTotal = total.count();
    return toReturn;
}

/* Times the creation of a MultilevelTreeNode tree built with add_leaf */
treeAndTiming<MultilevelTreeNode> seqToIncrementalMultilevelTree(std::vector<int> leaves, std::vector<int> parents) {
    auto t1 = high_resolution_clock::now();

    int numNodes = leaves.size() + 1;
    std::vector<MultilevelTreeNode*> nodes(numNodes);
    for (int i = 0; i < numNodes; ++i)
    {
        nodes[i] = new MultilevelTreeNode(std::to_string(i));
    }

    MultilevelTreeNode* root = nodes[parents[parents.size() - 1]];

    for (int i = leaves.size() - 1; i >= 0; --i) {
        nodes[parents[i]]->add_leaf(nodes[leaves[i]]);
    }

    auto t2 = high_resolution_clock::now();
    auto total = duration_cast<microseconds>(t2 - t1);

    treeAndTiming<MultilevelTreeNode> toReturn;
    toReturn.tree = root;
    toReturn.nodes = nodes;
    toReturn.multilevelTotal = total.count();
    return toReturn;
}

int main()
{
    int numNodes = 10000;

    int numRandTrees = 100;
    int numIter = 10;
    int numQueries = 1000;

    unsigned long long avgCreation = 0;
    unsigned long long avgStatic = 0;
    unsigned long long avgIncremental = 0;
    unsigned long long avgMultilevel = 0;

    unsigned long long avgNaiveQuery = 0;
    unsigned long long avgStaticQuery = 0;
    unsigned long long avgIncrementalQuery = 0;
    unsigned long long avgMultilevelQuery = 0;

    for (int i = 0; i < numRandTrees; ++i)
    {
        std::cout << "Generating random tree " << i << std::endl;
        std::vector<std::vector<int>> sequences = randInsertionSeq(numNodes);
        std::vector<int> leaves = sequences[0];
        std::vector<int> parents = sequences[1];

        for (int j = 0; j < numIter; ++j)
        {
            treeAndTiming<ExpensiveTreeNode> randStatic = seqToStaticTree(leaves, parents);
            treeAndTiming<ExpensiveTreeNode> randIncr = seqToIncrementalTree(leaves, parents);
            treeAndTiming<MultilevelTreeNode> randMultilevel = seqToIncrementalMultilevelTree(leaves, parents);
    
            avgCreation += randStatic.staticCreation;
            avgStatic += randStatic.staticTotal;
            avgIncremental += randIncr.incrementalTotal;
            avgMultilevel += randMultilevel.multilevelTotal;

            for (int k = 0; k < numQueries; ++k)
            {
                int nodeX = rand() % numNodes;
                int nodeY = rand() % numNodes;

                ExpensiveTreeNode* lcaExpensive;
                MultilevelTreeNode* lcaMultilevel;
                
                auto t0 = high_resolution_clock::now();
                lcaExpensive = ExpensiveTreeNode::naiveLca(randStatic.nodes[nodeX], randStatic.nodes[nodeY]);
                auto t1 = high_resolution_clock::now();
                lcaExpensive = ExpensiveTreeNode::lca(randStatic.nodes[nodeX], randStatic.nodes[nodeY]);
                auto t2 = high_resolution_clock::now();
                lcaExpensive = ExpensiveTreeNode::lca(randIncr.nodes[nodeX], randIncr.nodes[nodeY]);
                auto t3 = high_resolution_clock::now();
                lcaMultilevel = MultilevelTreeNode::lca(randMultilevel.nodes[nodeX], randMultilevel.nodes[nodeY]);
                auto t4 = high_resolution_clock::now();

                auto naiveQ = duration_cast<nanoseconds>(t1 - t0);
                auto staticQ = duration_cast<nanoseconds>(t2 - t1);
                auto incrQ = duration_cast<nanoseconds>(t3 - t2);
                auto multiQ = duration_cast<nanoseconds>(t4 - t3);

                avgNaiveQuery += naiveQ.count();
                avgStaticQuery += staticQ.count();
                avgIncrementalQuery += incrQ.count();
                avgMultilevelQuery += multiQ.count();
            }

            randStatic.tree->deleteNode();
            randIncr.tree->deleteNode();
            randMultilevel.tree->deleteNode();

        }
    }

    std::cout << "-------" << std::endl;
    std::cout << "Average Static Creation: " << avgCreation  * 1.0/(numIter * numRandTrees)<< std::endl;
    std::cout << "Average Static:" << avgStatic  * 1.0/(numIter * numRandTrees)<< std::endl;
    std::cout << "Average Incr:" << avgIncremental  * 1.0/(numIter * numRandTrees)<< std::endl;
    std::cout << "Average Multilevel:" << avgMultilevel  * 1.0/(numIter * numRandTrees)<< std::endl;
    std::cout << std::endl;
    std::cout << "Average Naive Query: " << avgNaiveQuery * 1.0/(numIter * numRandTrees * numQueries) << std::endl;
    std::cout << "Average Static Query: " << avgStaticQuery* 1.0/(numIter * numRandTrees * numQueries) << std::endl;
    std::cout << "Average Incr Query: " << avgIncrementalQuery* 1.0/(numIter * numRandTrees * numQueries) << std::endl;
    std::cout << "Average Multilevel Query: " << avgMultilevelQuery * 1.0/(numIter * numRandTrees * numQueries)<< std::endl;
    std::cout << "-------" << std::endl;


    return 0;
}