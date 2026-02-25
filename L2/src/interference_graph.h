#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace L2 {

    inline std::unordered_map<std::string, int> REG_TO_COLOR = {
        // caller saved
        {"rax", 0},
        {"rcx", 1},
        {"rdx", 2},
        {"rsi", 3},
        {"rdi", 4},
        {"r8",  5},
        {"r9",  6},
        {"r10", 7},
        {"r11", 8},
        // callee saved
        {"rbx", 9},
        {"rbp", 10},
        {"r12", 11},
        {"r13", 12},
        {"r14", 13},
        {"r15", 14}
    };

    inline std::vector<std::string> COLOR_TO_REG = {
        "rax",   // 0
        "rcx",   // 1
        "rdx",   // 2
        "rsi",   // 3
        "rdi",   // 4
        "r8",    // 5
        "r9",    // 6
        "r10",   // 7
        "r11",   // 8
        "rbx",   // 9
        "rbp",   // 10
        "r12",   // 11
        "r13",   // 12
        "r14",   // 13
        "r15"    // 14
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