#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace L2 {

    inline std::unordered_map<std::string, int> REG_TO_COLOR = {
        // caller saved
        {"rax", 0},
        {"rdx", 1},
        {"rsi", 2},
        {"rdi", 3},
        {"r8",  4},
        {"r9",  5},
        {"r10", 6},
        {"r11", 7},
        {"rbx", 8},
        {"rbp", 9},
        {"r12", 10},
        {"r13", 11},
        {"r14", 12},
        {"r15", 13},
        {"rcx", 14}
    };

    inline std::vector<std::string> COLOR_TO_REG = {
        "rax",   // 0
        "rdx",   // 1
        "rsi",   // 2
        "rdi",   // 3
        "r8",    // 4
        "r9",    // 5
        "r10",   // 6
        "r11",   // 7
        "rbx",    // 8
        "rbp",   // 9
        "r12",   // 10
        "r13",   // 11
        "r14",   // 12
        "r15",   // 13
        "rcx",   // 14
    };

    class Node {
        public:
            int color;
            std::string var_name;
            bool removed;
            int degree;
            std::unordered_set<std::string> neighbors;
            std::vector<bool> colorable;
            
            Node(std::string var_name);
            void color_node(std::unordered_map<std::string, Node>& graph);
            void remove_node(std::unordered_map<std::string, Node>& graph);
    };
}