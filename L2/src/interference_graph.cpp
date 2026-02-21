#include "L2.h"

namespace L2 {

    std::unordered_map<std::string, int> REG_TO_COLOR = {
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

    std::vector<std::string> COLOR_TO_REG = {
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

    Node::Node(std::string var_name) : var_name(var_name) {
        
        if (REG_TO_COLOR.find(var_name) == REG_TO_COLOR.end()) {
            color = -1;
        } else {
            color = REG_TO_COLOR[var_name];

            for (std::string reg : COLOR_TO_REG) {
                if (reg == var_name) continue;
                neighbors.insert(reg);
            }
        }        
    }


    void Function::construct_graph(bool verbose) {
        // assumes determine liveness is already run

        for (auto& instruction : instructions){

            // create all regs
            for (std::string reg : COLOR_TO_REG) {
               graph.insert({reg, Node(reg)});
            }

            auto sx_i = dynamic_cast<Instruction_W_sop_SX*>(instruction.get());

            // special case: connect var to all other regs if is a shift op
            if (sx_i != nullptr) {
                auto var_ptr = dynamic_cast<Variable*>(sx_i->sx.get());
                if (var_ptr != nullptr) {

                    if (graph.find(var_ptr->var_name) == graph.end()) 
                        graph.insert({var_ptr->var_name, Node(var_ptr->var_name)});
                    
                    for (std::string reg : COLOR_TO_REG){

                        if (reg == "rcx") continue;
                        graph.at(reg).neighbors.insert(var_ptr->var_name);
                        graph.at(var_ptr->var_name).neighbors.insert(reg);
                    }
                }
            }
            
            // in_set edges
            for (auto i = instruction->in_set.begin(); i != instruction->in_set.end(); ++i){

                const std::string& var_i = *i;

                if (graph.find(var_i) == graph.end()){
                    graph.insert({var_i, Node(var_i)});
                }

                for (auto j = instruction->in_set.begin(); j != i; ++j){
                    const std::string& var_j = *j;

                    graph.at(var_i).neighbors.insert(var_j);
                    graph.at(var_j).neighbors.insert(var_i);
                }
            }
            // out_set edges
            for (auto i = instruction->out_set.begin(); i != instruction->out_set.end(); ++i){

                const std::string& var_i = *i;

                if (graph.find(var_i) == graph.end()){
                    graph.insert({var_i, Node(var_i)});
                }

                for (auto j = instruction->out_set.begin(); j != i; ++j){
                    const std::string& var_j = *j;

                    graph.at(var_i).neighbors.insert(var_j);
                    graph.at(var_j).neighbors.insert(var_i);
                }
            }
            // dead code vars
            for (auto i = instruction->kill_set.begin(); i != instruction->kill_set.end(); ++i){

                const std::string& var_i = *i;

                if (graph.find(var_i) == graph.end()){
                    graph.insert({var_i, Node(var_i)});
                }

                for (auto j = instruction->out_set.begin(); j != instruction->out_set.end(); ++j){
                    const std::string& var_j = *j;

                    if (var_i == var_j) continue;

                    graph.at(var_i).neighbors.insert(var_j);
                    graph.at(var_j).neighbors.insert(var_i);
                }
            }

        }

        if (verbose) {

            std::vector<std::string> vars;
            for (auto& node : graph) {
                vars.push_back(node.first);
            }
            std::sort(vars.begin(), vars.end());

            for (std::string var : vars){
                
                std::cout << var;

                std::vector<std::string> neighbors(graph.at(var).neighbors.begin(), graph.at(var).neighbors.end());
                std::sort(neighbors.begin(), neighbors.end());

                for (std::string neighbor : neighbors) {
                    std::cout << " " << neighbor;
                }
                std::cout << "\n";
            }
        }
    }

    void Program::construct_graphs(bool verbose){
        for (auto& f: functions){
            f->construct_graph(verbose);
        }
    }

    void Function::color_graph() {
        
    }
}