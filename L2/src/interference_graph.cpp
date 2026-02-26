#include "L2.h"

namespace L2 {

    const std::string SPILL_PREFIX = "SSSS";

    Node::Node(std::string var_name) : var_name(var_name), removed(false), color(-1) {

        for (int i = 0; i < 15; i++){
            colorable.push_back(true);
        }

        if (REG_TO_COLOR.find(var_name) != REG_TO_COLOR.end()) {
            for (std::string reg : COLOR_TO_REG) {
                if (reg == var_name) continue;
                neighbors.insert(reg);
            }
        }

    }

    void Function::construct_graph(bool verbose) {
        // assumes determine liveness is already run

        graph.clear();

        // create all regs
        for (std::string reg : COLOR_TO_REG) {
            graph.insert({reg, Node(reg)});
        }

        for (auto& instruction : instructions){

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

                std::string var_i = *i;
                // remove the % from vars
                if (!var_i.empty() && var_i[0] == '%')
                    var_i = var_i.substr(1);

                if (graph.find(var_i) == graph.end()){
                    graph.insert({var_i, Node(var_i)});
                }

                for (auto j = instruction->in_set.begin(); j != i; ++j){
                    std::string var_j = *j;
                    // remove the % from vars
                    if (!var_j.empty() && var_j[0] == '%')
                        var_j = var_j.substr(1);

                    graph.at(var_i).neighbors.insert(var_j);
                    graph.at(var_j).neighbors.insert(var_i);
                }
            }
            // out_set edges
            for (auto i = instruction->out_set.begin(); i != instruction->out_set.end(); ++i){

                std::string var_i = *i;
                // remove the % from vars
                if (!var_i.empty() && var_i[0] == '%')
                    var_i = var_i.substr(1);

                if (graph.find(var_i) == graph.end()){
                    graph.insert({var_i, Node(var_i)});
                }

                for (auto j = instruction->out_set.begin(); j != i; ++j){
                    std::string var_j = *j;
                    // remove the % from vars
                    if (!var_j.empty() && var_j[0] == '%')
                        var_j = var_j.substr(1);

                    graph.at(var_i).neighbors.insert(var_j);
                    graph.at(var_j).neighbors.insert(var_i);
                }
            }
            // dead code vars
            for (auto i = instruction->kill_set.begin(); i != instruction->kill_set.end(); ++i){

                std::string var_i = *i;
                // remove the % from vars
                if (!var_i.empty() && var_i[0] == '%')
                    var_i = var_i.substr(1);

                if (graph.find(var_i) == graph.end()){
                    graph.insert({var_i, Node(var_i)});
                }

                for (auto j = instruction->out_set.begin(); j != instruction->out_set.end(); ++j){
                    std::string var_j = *j;
                    // remove the % from vars
                    if (!var_j.empty() && var_j[0] == '%')
                        var_j = var_j.substr(1);

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

    void Node::remove_node(std::unordered_map<std::string, Node>& graph){
        removed = true;

        for (std::string neighbor : neighbors){
            graph.at(neighbor).degree -= 1;
        }
    }

    void Node::color_node(std::unordered_map<std::string, Node>& graph){
        
        if (REG_TO_COLOR.find(var_name) != REG_TO_COLOR.end()){
            color = REG_TO_COLOR.at(var_name);
        } else {
            for (int i = 0; i < 15; i++){
                if (colorable[i]){
                    color = i;
                    break;
                }
            }
            // spill
            if (color == -1) return;
        }

        for (std::string neighbor : neighbors){
            graph.at(neighbor).colorable[color] = false;
        }
    }

    void Function::color_graph(bool verbose) {
        
        std::vector<std::string> coloring_stack;

        for (auto& node : graph){
            node.second.degree = node.second.neighbors.size();
        }

        // color registers first
        for (std::string reg : COLOR_TO_REG){
            graph.at(reg).color_node(graph);
        }

        // color nodes previously spilled associated with shift ops as they can only use rcx
        for (auto& instruction : instructions){
            auto sx_i = dynamic_cast<Instruction_W_sop_SX*>(instruction.get());

            if (sx_i != nullptr) {
                auto var_ptr = dynamic_cast<Variable*>(sx_i->sx.get());
                if (var_ptr != nullptr && var_ptr->var_name.substr(0, SPILL_PREFIX.length()) == SPILL_PREFIX) {
                    graph.at(var_ptr->var_name).remove_node(graph);
                    graph.at(var_ptr->var_name).color_node(graph);
                }
            }
        }

        // remove nodes with <15 neighbors first, starting with the one with most edges (but <15)
        while (true) {
            std::string max_node;
            int max_degree = -1;
            
            for (auto& node : graph){

                if (REG_TO_COLOR.find(node.first) != REG_TO_COLOR.end()) continue;

                if (!node.second.removed && node.second.degree >= max_degree && node.second.degree < 15){
                    
                    //if (node.second.degree == max_degree && node.first[0] != 'S') continue;

                    max_node = node.first;
                    max_degree = node.second.degree;
                }
            }

            if (max_node.empty()){
                break;
            }
            graph.at(max_node).remove_node(graph);
            coloring_stack.push_back(max_node);
        }

        // remove nodes starting from node with highest number of edges
        while (true) {
            std::string max_node;
            int max_degree = -1;

            for (auto& node : graph){

                if (REG_TO_COLOR.find(node.first) != REG_TO_COLOR.end()) continue;

                if (!node.second.removed && node.second.degree > max_degree){
                    max_node = node.first;
                    max_degree = node.second.degree;
                }
            }

            if (max_node.empty()){
                break;
            }
            graph.at(max_node).remove_node(graph);
            coloring_stack.push_back(max_node);
        }

        // color according to stack

        while (coloring_stack.size() != 0){
            std::string node = coloring_stack.back();
            coloring_stack.pop_back();

            if (graph.at(node).color != -1) continue;

            graph.at(node).color_node(graph);
        }

        if (verbose){
            for (auto& node : graph){
                std::cout << node.first << " : " << node.second.color << std::endl;
            }
        }
    }

    void Program::allocate_registers(){

        for (auto& f : functions){

            bool colorable = false;

            while (!colorable){

                f->determine_liveness(false);
                f->construct_graph(false);
                f->color_graph(false);

                // for (auto& node : f->graph){
                //     std::cout << node.first << " : " << node.second.color << ", ";
                // }
                // std::cout << std::endl;

                bool hasSpilled = false;

                for (auto& node : f->graph){
                    if (node.second.color == -1){

                        std::string node_to_spill = node.first;

                        // fix later, use something less common than S
                        if (node.first.substr(0, SPILL_PREFIX.length()) == SPILL_PREFIX) {
                            // S not spillable, spill a neighbor instead
                            for (std::string neighbor : node.second.neighbors) {
                                if (neighbor.substr(0, SPILL_PREFIX.length()) != SPILL_PREFIX && REG_TO_COLOR.find(neighbor) == REG_TO_COLOR.end()) {
                                    node_to_spill = neighbor;
                                    break;
                                }
                            }
                        }

                        f->spill_var(node_to_spill, SPILL_PREFIX);
                        hasSpilled = true;
                        break;
                    }
                }

                colorable = !hasSpilled;
            }
            // f->determine_liveness(true);

            // std::cout << f->to_string() << std::endl;
            // std::cout << f->name << " colored" << std::endl;
            
        }

    }

}