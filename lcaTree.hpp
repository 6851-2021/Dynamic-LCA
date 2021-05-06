#include <list>

class TreeNode {
     public:
        int nodeId;
        std::list<TreeNode> children;
        int subtreeSize;
        int startInterval;
        int endInterval;
        

        TreeNode(int id);
        void print();

        TreeNode lca(TreeNode nodeA, TreeNode nodeB);
        void assignSubtreeSizes();
        void assignIntervals();
    private:
        void print(int level);
 };