#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace L2 {

    class Node {
        public:
            int color;
            std::string var_name;
            bool removed;
            std::unordered_set<std::string> neighbors;
            std::unordered_set<int> colorable;
            
            Node(std::string var_name);
            void color_node(std::unordered_map<std::string, Node>& graph);


    };
}