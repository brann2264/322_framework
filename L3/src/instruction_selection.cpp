#include "instruction_selection.h"

namespace L3 {

    bool isContextExcluded(Instruction* instruction) {
        return (dynamic_cast<Instruction_Label*>(instruction) != nullptr) ||
            (dynamic_cast<Instruction_Call_Function*>(instruction) != nullptr) ||
            (dynamic_cast<Instruction_Var_Function_Assignment*>(instruction) != nullptr);

    }

    bool IsContextBreak(Instruction* instruction){
        return (dynamic_cast<Instruction_Br_Label*>(instruction) != nullptr) ||
            (dynamic_cast<Instruction_Br_T_Label*>(instruction) != nullptr) ||
            isContextExcluded(instruction);
    }

    void Function::create_contexts(){

        std::unique_ptr<Context> current_context = std::make_unique<Context>();

        for (int i = 0; i < instructions.size(); i++){
            if (IsContextBreak(instructions[i].get())) {

                if (!isContextExcluded(instructions[i].get()))
                    current_context->add(i);

                if (!current_context->empty())
                    contexts.push_back(std::move(current_context));

                current_context = std::make_unique<Context>();
            } else {
                current_context->add(i);
            }
        }

        if (!current_context->empty())
            contexts.push_back(std::move(current_context));

    }

    std::unique_ptr<InstructionTree> Instruction_Var_S_Assignment::generate_tree() const {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(s->to_string(), s->type);

        dst_node->add_child(std::move(src_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Var_T_Op_T_Assignment::generate_tree() const {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> op_node = std::make_unique<InstructionTree>(op->to_string(), op->type);
        std::unique_ptr<InstructionTree> operand1_node = std::make_unique<InstructionTree>(t1->to_string(), t1->type);
        std::unique_ptr<InstructionTree> operand2_node = std::make_unique<InstructionTree>(t2->to_string(), t2->type);

        op_node->add_child(std::move(operand1_node));
        op_node->add_child(std::move(operand2_node));
        dst_node->add_child(std::move(op_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Var_T_Cmp_T_Assignment::generate_tree() const {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> cmp_node = std::make_unique<InstructionTree>(cmp->to_string(), cmp->type);
        std::unique_ptr<InstructionTree> operand1_node = std::make_unique<InstructionTree>(t1->to_string(), t1->type);
        std::unique_ptr<InstructionTree> operand2_node = std::make_unique<InstructionTree>(t2->to_string(), t2->type);

        cmp_node->add_child(std::move(operand1_node));
        cmp_node->add_child(std::move(operand2_node));
        dst_node->add_child(std::move(cmp_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Var_Load_Var_Assignment::generate_tree() const {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var1->to_string(), var1->type);
        std::unique_ptr<InstructionTree> load_node = std::make_unique<InstructionTree>("$LOAD", ItemType::Other);
        std::unique_ptr<InstructionTree> operand_node = std::make_unique<InstructionTree>(var2->to_string(), var2->type);

        load_node->add_child(std::move(operand_node));
        dst_node->add_child(std::move(load_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Store_Var_S_Assignment::generate_tree() const {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$STORE", ItemType::Other);
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(s->to_string(), s->type);

        root_node->add_child(std::move(dst_node));
        root_node->add_child(std::move(src_node));

        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Return::generate_tree() const {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$RETURN", ItemType::Other);
        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Return_T::generate_tree() const {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$RETURN", ItemType::Other);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(t->to_string(), t->type);
        root_node->add_child(std::move(src_node));
        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Br_Label::generate_tree() const {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$BRANCH", ItemType::Other);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(label->to_string(), label->type);
        root_node->add_child(std::move(src_node));
        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Br_T_Label::generate_tree() const {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$BRANCH", ItemType::Other);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(label->to_string(), label->type);
        std::unique_ptr<InstructionTree> condition_node = std::make_unique<InstructionTree>(t->to_string(), t->type);
        
        root_node->add_child(std::move(src_node));
        root_node->add_child(std::move(condition_node));
        return root_node;
    }

    void Context::generate_trees(std::vector<std::unique_ptr<Instruction>>& instructions){
        for (int instruction_id : instruction_ids) {
            trees.push_back(std::move(instructions[instruction_id]->generate_tree()));
        }
    }

    void InstructionTree::tile_tree() {
        // std::cout << value << std::endl;
        // size 6, cost 2
        if (Mem_Increment_T_Tile::tileable(*this)) return;
        if (Mem_Decrement_T_Tile::tileable(*this)) return;
        // size 5, cost 2
        if (W_Increment_Mem_Tile::tileable(*this)) return;
        if (W_Decrement_Mem_Tile::tileable(*this)) return;
        // size 4, cost 2
        if (W_Aop_T_Tile::tileable(*this)) return;
        if (W_Sop_Sx_Tile::tileable(*this)) return;
        if (W_Sop_N_Tile::tileable(*this)) return;
        if (W_Assign_T_Cmp_T_Tile::tileable(*this)) return;   
        // size 3, cost 1
        if (W_Assign_Mem_Tile::tileable(*this)) return;    
        if (Mem_Assign_S_Tile::tileable(*this)) return;    
        if (Cjump_T_Cmp_T_Label_Tile::tileable(*this)) return;    
        if (W_Increment_Tile::tileable(*this)) return;    
        if (W_Decrement_Tile::tileable(*this)) return; 
        // size 2, cost 1
        if (Goto_Label_Tile::tileable(*this)) return;    
        if (W_Assign_S_Tile::tileable(*this)) return;    
        // size 1, cost 1
        if (Return_Tile::tileable(*this)) return; 

        if (children.size() == 0) return;
        throw std::runtime_error("Not tileable");
    }

    // L3 data types: vars, label, number, function_name
    // L2 data types: w, s, t, u
    // w -> registers, var; CHECK: var
    // s -> registers, label, function_name, var, number; CHECK: label, function_name, var, number
    // t -> registers, var, number; CHECK: var, number
    // MEM X M -> STORE/LOAD, CHECK CHILD == VAR

    bool isT(InstructionTree& tree){
        return tree.type == ItemType::Variable || tree.type == ItemType::Number;
    }
    bool isW(InstructionTree& tree){
        return tree.type == ItemType::Variable;
    }
    bool isS(InstructionTree& tree){
        return isT(tree) || tree.type == ItemType::Label || tree.type == ItemType::FunctionName;
    }
    bool isU(InstructionTree& tree){
        return isW(tree) || tree.type == ItemType::FunctionName;
    }


    // size 2, cost: 1
    bool W_Assign_S_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || !isS(*tree.children[0]))
            return false;

        // tileable, call tile_tree on children
        tree.tiles.push_back(std::make_unique<W_Assign_S_Tile>(tree));
        for (auto& child : tree.children){
            child->tile_tree();
        }
        return true;
    }

    //  size 3: cost 1
    bool W_Assign_Mem_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Other || tree.children[0]->value != "$LOAD")
            return false;
        if (tree.children[0]->children.size() != 1 || !isW(*tree.children[0]->children[0]))
            return false;
        
        // tileable, call tile_tree on children
        tree.tiles.push_back(std::make_unique<W_Assign_Mem_Tile>(tree));
        for (auto& child : tree.children){
            child->tile_tree();
        }
        return true;
    }

    // size 3: cost 1
    // TODO: MEM offset is a result of merging
    bool Mem_Assign_S_Tile::tileable(InstructionTree& tree) {
        if (tree.type != ItemType::Other || tree.value != "$STORE")
            return false;
        if (tree.children.size() != 2 || !isW(*tree.children[0]) || !isS(*tree.children[1]))
            return false;

        tree.tiles.push_back(std::make_unique<Mem_Assign_S_Tile>(tree));
        for (auto& child : tree.children){
            child->tile_tree();
        }
        return true;
    }

    // bool W_Assign_StackArg_M_Tile::tileable(InstructionTree& tree) {};

    // size 4; cost 2
    // w += t
    // w = w + t, w have to be the same
    bool W_Aop_T_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator)
            return false;
        if (tree.children[0]->value == "<<" || tree.children[0]->value == ">>")
            return false;
        if (tree.children[0]->children.size() != 2 || !isT(*tree.children[0]->children[1]))
            return false;
        if (!isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        
        tree.tiles.push_back(std::make_unique<W_Aop_T_Tile>(tree));
        for (auto& child : tree.children[0]->children){
            child->tile_tree();
        }
        return true;
    }

    // size 4; cost 2
    // w = w << var
    bool W_Sop_Sx_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator)
            return false;
        if (tree.children[0]->value != "<<" && tree.children[0]->value != ">>")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[1]))
            return false;
        if (!isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        
        tree.tiles.push_back(std::make_unique<W_Sop_Sx_Tile>(tree));
        for (auto& child : tree.children[0]->children){
            child->tile_tree();
        }
        return true;
    }

    // size 4; cost 2
    // w = w << N
    bool W_Sop_N_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator)
            return false;
        if (tree.children[0]->value != "<<" && tree.children[0]->value != ">>")
            return false;
        if (tree.children[0]->children.size() != 2 || tree.children[0]->children[1]->type != ItemType::Number)
            return false;
        if (!isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        
        tree.tiles.push_back(std::make_unique<W_Sop_N_Tile>(tree));
        for (auto& child : tree.children[0]->children){
            child->tile_tree();
        }
        return true;
    }

    // technically + operator is associative
    // size 6; cost 2
    // mem x M += t
    // Store Var = Load Var + t
    //       Store
    //.   Var     +
    //         Load T
    //          Var    
    bool Mem_Increment_T_Tile::tileable(InstructionTree& tree) {
        if (tree.type != ItemType::Other || tree.value != "$STORE")
            return false;
        if (tree.children.size() != 2 || !isW(*tree.children[0]) || tree.children[1]->type != ItemType::Operator)
            return false;
        if (tree.children[1]->value != "++")
            return false;
        if (tree.children[1]->children.size() != 2 || tree.children[1]->children[0]->type != ItemType::Other ||
            tree.children[1]->children[0]->value != "$LOAD" || !isT(*tree.children[1]->children[1]))
            return false;
        if (tree.children[1]->children[0]->children.size() != 1 || isW(*tree.children[1]->children[0]->children[0]) || tree.children[1]->children[0]->children[0]->value != tree.children[0]->value)
            return false;
        
        tree.tiles.push_back(std::make_unique<Mem_Increment_T_Tile>(tree));
        tree.children[0]->tile_tree();
        tree.children[1]->children[0]->children[0]->tile_tree();
        tree.children[1]->children[1]->tile_tree();
        return true;
    }

    // size 6; cost 2
    // mem x M -= t
    // Store Var = Load Var + t
    //       Store
    //.   Var     -
    //         Load T
    //          Var    
    bool Mem_Decrement_T_Tile::tileable(InstructionTree& tree) {
        if (tree.type != ItemType::Other || tree.value != "$STORE")
            return false;
        if (tree.children.size() != 2 || !isW(*tree.children[0]) || tree.children[1]->type != ItemType::Operator)
            return false;
        if (tree.children[1]->value != "--")
            return false;
        if (tree.children[1]->children.size() != 2 || tree.children[1]->children[0]->type != ItemType::Other ||
            tree.children[1]->children[0]->value != "$LOAD" || !isT(*tree.children[1]->children[1]))
            return false;
        if (tree.children[1]->children[0]->children.size() != 1 || isW(*tree.children[1]->children[0]->children[0]) || tree.children[1]->children[0]->children[0]->value != tree.children[0]->value)
            return false;
        
        tree.tiles.push_back(std::make_unique<Mem_Decrement_T_Tile>(tree));
        tree.children[0]->tile_tree();
        tree.children[1]->children[0]->children[0]->tile_tree();
        tree.children[1]->children[1]->tile_tree();
        return true;
    }

    // size 5; cost 2
    //      W
    //      +
    //    W   Load
    //         Var
    bool W_Increment_Mem_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "+")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]) || tree.children[0]->children[1]->value != "$LOAD" || tree.children[0]->children[1]->type != ItemType::Other)
            return false;
        if (tree.children[0]->children[1]->children.size() != 1 || isW(*tree.children[0]->children[1]->children[0]))
            return false;
        if (tree.value != tree.children[0]->children[0]->value)
            return false;
        
        tree.tiles.push_back(std::make_unique<W_Increment_Mem_Tile>(tree));
        tree.children[0]->children[0]->tile_tree();
        tree.children[0]->children[1]->children[0]->tile_tree();
        return true;
    }
    // size 5; cost 2
    //      W
    //      -
    //    W   Load
    //         Var
    bool W_Decrement_Mem_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "-")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]) || tree.children[0]->children[1]->value != "$LOAD" || tree.children[0]->children[1]->type != ItemType::Other)
            return false;
        if (tree.children[0]->children[1]->children.size() != 1 || isW(*tree.children[0]->children[1]->children[0]))
            return false;
        if (tree.value != tree.children[0]->children[0]->value)
            return false;
        
        tree.tiles.push_back(std::make_unique<W_Decrement_Mem_Tile>(tree));
        tree.children[0]->children[0]->tile_tree();
        tree.children[0]->children[1]->children[0]->tile_tree();
        return true;
    }

    // size 4; cost 2
    bool W_Assign_T_Cmp_T_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Comparator)
            return false;
        if (tree.children[0]->children.size() != 2 || !isT(*tree.children[0]->children[0]) || !isT(*tree.children[0]->children[1]))
            return false;
        
        tree.tiles.push_back(std::make_unique<W_Assign_T_Cmp_T_Tile>(tree));
        tree.children[0]->children[0]->tile_tree();
        tree.children[0]->children[1]->tile_tree();
        return true;
    }

    // size 3; cost 1
    // L3 br t label doesnt not have a cmp t
    bool Cjump_T_Cmp_T_Label_Tile::tileable(InstructionTree& tree) {
        if (tree.type != ItemType::Other || tree.value != "$BRANCH")
            return false;
        if (tree.children.size() != 2 || !isT(*tree.children[0]) || tree.children[1]->type != ItemType::Label)
            return false;
        
        tree.tiles.push_back(std::make_unique<Cjump_T_Cmp_T_Label_Tile>(tree));
        tree.children[0]->tile_tree();
        tree.children[0]->tile_tree();
        return true;
    }

    // size 2; cost 1
    bool Goto_Label_Tile::tileable(InstructionTree& tree) {
        if (tree.type != ItemType::Other || tree.value != "$BRANCH")
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Label)
            return false;
        
        tree.tiles.push_back(std::make_unique<Goto_Label_Tile>(tree));
        tree.children[0]->tile_tree();
        return true;
    }

    // size 1; cost 1
    bool Return_Tile::tileable(InstructionTree& tree) {
        if (tree.type != ItemType::Other || tree.value != "$RETURN")
            return false;

        tree.tiles.push_back(std::make_unique<Return_Tile>(tree));
        if (tree.children.size() == 1 && isT(*tree.children[0])){
            tree.children[0]->tile_tree();
        }
        
        return true;
    }

    // size 3; cost 1
    // w++
    // w = w + 1
    bool W_Increment_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "+")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        if (tree.children[0]->children[1]->value != "1" || tree.children[0]->children[1]->type != ItemType::Number)
            return false;

        tree.tiles.push_back(std::make_unique<W_Increment_Tile>(tree));
        tree.children[0]->children[0]->tile_tree();
        tree.children[0]->children[1]->tile_tree();
        return true;
    }

    // size 3; cost 1
    // w--
    // w = w - 1
    bool W_Decrement_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "-")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        if (tree.children[0]->children[1]->value != "1" || tree.children[0]->children[1]->type != ItemType::Number)
            return false;

        tree.tiles.push_back(std::make_unique<W_Decrement_Tile>(tree));
        tree.children[0]->children[0]->tile_tree();
        tree.children[0]->children[1]->tile_tree();
        return true;
    }
    
    /*
            w
            +
          w   *
            w   N
    
    */  
    bool Address_Calculation_Tile::tileable(InstructionTree& tree) {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "+")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]))
            return false;
        if (tree.children[0]->children[1]->type != ItemType::Operator || tree.children[0]->children[1]->value != "*")
            return false;
        if (tree.children[0]->children[1]->children.size() != 2 || !isW(*tree.children[0]->children[1]->children[0]) || tree.children[0]->children[1]->children[1]->type != ItemType::Number)
            return false;
        
        tree.tiles.push_back(std::make_unique<Address_Calculation_Tile>(tree));
        tree.children[0]->children[0]->tile_tree();
        tree.children[0]->children[1]->children[0]->tile_tree();
        tree.children[0]->children[1]->children[1]->tile_tree();
        return true;
    }

    W_Assign_S_Tile::W_Assign_S_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " <- " + tree.children[0]->value;
    }
    W_Assign_Mem_Tile::W_Assign_Mem_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " <- mem " + tree.children[0]->children[0]->value + " 0";
    }
    Mem_Assign_S_Tile::Mem_Assign_S_Tile(InstructionTree& tree){
        instruction_translation = "mem " + tree.children[0]->value + " 0 <- " + tree.children[1]->value;
    }
    W_Aop_T_Tile::W_Aop_T_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " " + tree.children[0]->value + "= " + tree.children[0]->children[1]->value;
    }
    W_Sop_Sx_Tile::W_Sop_Sx_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " " + tree.children[0]->value + "= " + tree.children[0]->children[1]->value;
    }
    W_Sop_N_Tile::W_Sop_N_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " " + tree.children[0]->value + "= " + tree.children[0]->children[1]->value;
    }
    Mem_Increment_T_Tile::Mem_Increment_T_Tile(InstructionTree& tree){
        instruction_translation = "mem " + tree.children[0]->value + " 0 += " + tree.children[1]->children[1]->value;
    }
    Mem_Decrement_T_Tile::Mem_Decrement_T_Tile(InstructionTree& tree){
        instruction_translation = "mem " + tree.children[0]->value + " 0 -= " + tree.children[1]->children[1]->value;
    }
    W_Increment_Mem_Tile::W_Increment_Mem_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " += mem " + tree.children[0]->children[1]->children[0]->value + " 0"; 
    }
    W_Decrement_Mem_Tile::W_Decrement_Mem_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " -= mem " + tree.children[0]->children[1]->children[0]->value + " 0"; 
    }
    W_Assign_T_Cmp_T_Tile::W_Assign_T_Cmp_T_Tile(InstructionTree& tree){
        if (tree.children[0]->value == ">"){
            instruction_translation = tree.value + " <- " + tree.children[0]->children[1]->value + " < " + tree.children[0]->children[0]->value;
        } else if (tree.children[0]->value == ">=") {
            instruction_translation = tree.value + " <- " + tree.children[0]->children[1]->value + " <= " + tree.children[0]->children[0]->value;
        } else {
            instruction_translation = tree.value + " <- " + tree.children[0]->children[0]->value + " " + tree.children[0]->value + " " + tree.children[0]->children[1]->value;
        }
    }
    Cjump_T_Cmp_T_Label_Tile::Cjump_T_Cmp_T_Label_Tile(InstructionTree& tree){
        instruction_translation = "cjump " + tree.children[0]->value + " = 1 " + tree.children[1]->value; 
    }
    Goto_Label_Tile::Goto_Label_Tile(InstructionTree& tree){
        instruction_translation = "goto " + tree.children[0]->value;
    }
    Return_Tile::Return_Tile(InstructionTree& tree){
        if (tree.children.size() == 1)
            instruction_translation = "rax <- " + tree.children[0]->value + "\nreturn";
        else {
            instruction_translation = "return";
        }
    }
    W_Increment_Tile::W_Increment_Tile(InstructionTree& tree){
        instruction_translation = tree.value + "++";
    }
    W_Decrement_Tile::W_Decrement_Tile(InstructionTree& tree){
        instruction_translation = tree.value + "--";
    }
    Address_Calculation_Tile::Address_Calculation_Tile(InstructionTree& tree){
        instruction_translation = tree.value + " @ " + tree.children[0]->children[0]->value + " " + tree.children[0]->children[1]->children[0]->value + " " + tree.children[0]->children[1]->children[1]->value;
    }
}