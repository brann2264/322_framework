#include "instruction_selection.h"
// #define TILE_DEBUG 1
#ifdef TILE_DEBUG
#define TILE_DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define TILE_DEBUG_PRINT(x) // Does nothing
#endif

namespace L3
{

    bool isContextExcluded(Instruction *instruction)
    {
        return (dynamic_cast<Instruction_Label *>(instruction) != nullptr) ||
               (dynamic_cast<Instruction_Call_Function *>(instruction) != nullptr) ||
               (dynamic_cast<Instruction_Var_Function_Assignment *>(instruction) != nullptr);
    }

    bool IsContextBreak(Instruction *instruction)
    {
        return (dynamic_cast<Instruction_Br_Label *>(instruction) != nullptr) ||
               (dynamic_cast<Instruction_Br_T_Label *>(instruction) != nullptr) ||
               isContextExcluded(instruction);
    }

    void Function::create_contexts()
    {

        std::unique_ptr<Context> current_context = std::make_unique<Context>();

        for (int i = 0; i < instructions.size(); i++)
        {
            if (IsContextBreak(instructions[i].get()))
            {

                if (!isContextExcluded(instructions[i].get()))
                    current_context->add(i);
                else {
                    Instruction_Call_Function* call = dynamic_cast<Instruction_Call_Function *>(instructions[i].get()); 
                    Instruction_Var_Function_Assignment* call_assignment = dynamic_cast<Instruction_Var_Function_Assignment *>(instructions[i].get()); 
                    
                    if (call != nullptr){
                        for (auto& arg : call->args){
                            current_context->end_contexts_vars.insert(arg->to_string());
                        }
                    } else if (call_assignment != nullptr){
                        for (auto& arg : call_assignment->function_call_instruction->args){
                            current_context->end_contexts_vars.insert(arg->to_string());
                        }
                    }
                }

                if (!current_context->empty())
                    contexts.push_back(std::move(current_context));

                current_context = std::make_unique<Context>();
            }
            else
            {
                current_context->add(i);
            }
        }

        if (!current_context->empty())
            contexts.push_back(std::move(current_context));
    }

    std::unordered_set<std::string> InstructionTree::getWriteVars()
    {
        std::unordered_set<std::string> write_vars;

        if (isWrite && isVar)
        {
            write_vars.insert(value);
        }

        for (auto &child : children)
        {
            std::unordered_set<std::string> child_write_vars = child->getWriteVars();
            write_vars.insert(child_write_vars.begin(), child_write_vars.end());
        }
        return write_vars;
    }
    std::unordered_set<std::string> InstructionTree::getReadVars()
    {
        std::unordered_set<std::string> read_vars;

        if (isRead && isVar)
        {
            read_vars.insert(value);
        }

        for (auto &child : children)
        {
            std::unordered_set<std::string> child_read_vars = child->getReadVars();
            read_vars.insert(child_read_vars.begin(), child_read_vars.end());
        }
        return read_vars;
    }

    void Context::merge_trees()
    {

        for (int i = 0; i < trees.size(); i++)
        {

            // not candidate -> skip
            if (trees[i] == nullptr || !trees[i]->isVar || !trees[i]->isWrite)
                continue;

            std::string target_var = trees[i]->value;
            std::unordered_set<std::string> def_reads = trees[i]->getReadVars();
            bool merged_this_tree = false;

            // find candidate
            for (int j = i + 1; j < trees.size(); j++)
            {
                if (trees[j] == nullptr)
                    continue;

                std::unordered_set<std::string> use_reads = trees[j]->getReadVars();
                std::unordered_set<std::string> use_writes = trees[j]->getWriteVars();

                if (use_reads.find(target_var) != use_reads.end())
                {

                    std::vector<InstructionTree *> leaves;
                    trees[j]->get_leaves(leaves);

                    int root_use_count = 0;
                    for (auto *leaf : leaves)
                    {
                        if (leaf->value == target_var && !leaf->isWrite)
                            root_use_count++;
                    }

                    // used multiple times in this, abort merge
                    if (root_use_count != 1)
                    {
                        break;
                    }

                    // check if dead after
                    bool is_dead_after = true;

                    if (use_writes.find(target_var) == use_writes.end()){

                        for (int k = j + 1; k < trees.size(); k++)
                        {
                            if (trees[k] == nullptr)
                                continue;

                            std::unordered_set<std::string> future_reads = trees[k]->getReadVars();
                            std::unordered_set<std::string> future_writes = trees[k]->getWriteVars();

                            if (future_reads.find(target_var) != future_reads.end())
                            {
                                is_dead_after = false;
                                break;
                            }
                            // gets killed by an assignment -> good to merge
                            if (future_writes.find(target_var) != future_writes.end())
                                break;
                        }

                        // check if end context needs it (i.e. function call
                        if (end_contexts_vars.find(target_var) != end_contexts_vars.end()){
                            is_dead_after = true;
                        }
                    }

                    if (!is_dead_after)
                        break;

                    trees[j]->merge_tree(trees[i]);
                    merged_this_tree = true;
                    break;
                }

                // check if instructions between interfere

                // no definitions of variable used by this instruction

                bool interference = false;
                for (const std::string &read_var : def_reads)
                {
                    if (use_writes.find(read_var) != use_writes.end())
                    {
                        interference = true;
                        break;
                    }
                }
                if (interference)
                    break;

                // redefines our candidate -> dead
                if (use_writes.find(target_var) != use_writes.end())
                    break;
            }
        }

        // cleanup
        std::vector<std::unique_ptr<InstructionTree>> newTrees;
        for (int i = 0; i < trees.size(); i++)
        {
            if (trees[i] != nullptr)
            {   
                // trees[i]->print("", true);
                newTrees.push_back(std::move(trees[i]));
            }
        }
        trees = std::move(newTrees);
    }
    std::unique_ptr<InstructionTree> Instruction_Var_S_Assignment::generate_tree() const
    {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(s->to_string(), s->type);
        dst_node->item = var;
        dst_node->isWrite = true;
        dst_node->isRead = false;
        src_node->item = s;
        dst_node->add_child(std::move(src_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Var_T_Op_T_Assignment::generate_tree() const
    {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> op_node = std::make_unique<InstructionTree>(op->to_string(), op->type);
        std::unique_ptr<InstructionTree> operand1_node = std::make_unique<InstructionTree>(t1->to_string(), t1->type);
        std::unique_ptr<InstructionTree> operand2_node = std::make_unique<InstructionTree>(t2->to_string(), t2->type);
        dst_node->item = var;
        dst_node->isWrite = true;
        dst_node->isRead = false;
        op_node->item = op;
        operand1_node->item = t1;
        operand2_node->item = t2;

        op_node->add_child(std::move(operand1_node));
        op_node->add_child(std::move(operand2_node));
        dst_node->add_child(std::move(op_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Var_T_Cmp_T_Assignment::generate_tree() const
    {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> cmp_node = std::make_unique<InstructionTree>(cmp->to_string(), cmp->type);
        std::unique_ptr<InstructionTree> operand1_node = std::make_unique<InstructionTree>(t1->to_string(), t1->type);
        std::unique_ptr<InstructionTree> operand2_node = std::make_unique<InstructionTree>(t2->to_string(), t2->type);
        dst_node->item = var;
        dst_node->isWrite = true;
        dst_node->isRead = false;
        cmp_node->item = cmp;
        operand1_node->item = t1;
        operand2_node->item = t2;
        cmp_node->add_child(std::move(operand1_node));
        cmp_node->add_child(std::move(operand2_node));
        dst_node->add_child(std::move(cmp_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Var_Load_Var_Assignment::generate_tree() const
    {
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var1->to_string(), var1->type);
        std::unique_ptr<InstructionTree> load_node = std::make_unique<InstructionTree>("$LOAD", ItemType::Other);
        std::unique_ptr<InstructionTree> operand_node = std::make_unique<InstructionTree>(var2->to_string(), var2->type);

        dst_node->item = var1;
        dst_node->isWrite = true;
        dst_node->isRead = false;
        operand_node->item = var2;

        load_node->add_child(std::move(operand_node));
        dst_node->add_child(std::move(load_node));

        return dst_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Store_Var_S_Assignment::generate_tree() const
    {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$STORE", ItemType::Other);
        std::unique_ptr<InstructionTree> dst_node = std::make_unique<InstructionTree>(var->to_string(), var->type);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(s->to_string(), s->type);

        dst_node->item = var;
        src_node->item = s;

        root_node->add_child(std::move(dst_node));
        root_node->add_child(std::move(src_node));

        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Return::generate_tree() const
    {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$RETURN", ItemType::Other);
        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Return_T::generate_tree() const
    {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$RETURN", ItemType::Other);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(t->to_string(), t->type);

        src_node->item = t;
        root_node->add_child(std::move(src_node));
        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Br_Label::generate_tree() const
    {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$BRANCH", ItemType::Other);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(label->to_string(), label->type);

        src_node->item = label;
        root_node->add_child(std::move(src_node));
        return root_node;
    }

    std::unique_ptr<InstructionTree> Instruction_Br_T_Label::generate_tree() const
    {
        std::unique_ptr<InstructionTree> root_node = std::make_unique<InstructionTree>("$BRANCH", ItemType::Other);
        std::unique_ptr<InstructionTree> src_node = std::make_unique<InstructionTree>(label->to_string(), label->type);
        std::unique_ptr<InstructionTree> condition_node = std::make_unique<InstructionTree>(t->to_string(), t->type);
        src_node->item = label;
        condition_node->item = t;
        root_node->add_child(std::move(condition_node));
        root_node->add_child(std::move(src_node));
        return root_node;
    }

    void Context::generate_trees(std::vector<std::unique_ptr<Instruction>> &instructions)
    {
        for (int instruction_id : instruction_ids)
        {
            trees.push_back(std::move(instructions[instruction_id]->generate_tree()));
        }
    }

    void InstructionTree::tile_tree(InstructionTree &root)
    {
        // size 6, cost 2
        if (Mem_Increment_T_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("Mem_Increment_T_Tile starting at " + value);
            return;
        }
        if (Mem_Decrement_T_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("Mem_Decrement_T_Tile starting at " + value);
            return;
        }
        // size 5, cost 2
        if (W_Increment_Mem_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Increment_Mem_Tile starting at " + value);
            return;
        }
        if (W_Decrement_Mem_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Decrement_Mem_Tile starting at " + value);
            return;
        }
        // size 4, cost 2
        if (W_Aop_T_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Aop_T_Tile starting at " + value);
            return;
        }
        if (W_Sop_Sx_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Sop_Sx_Tile starting at " + value);
            return;
        }
        if (W_Sop_N_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Sop_N_Tile starting at " + value);
            return;
        }
        if (W_Assign_T_Cmp_T_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Assign_T_Cmp_T_Tile starting at " + value);
            return;
        }
        if (W_Assign_T_Aop_T_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Assign_T_Aop_T_Tile starting at " + value);
            return;
        }
        if (W_Assign_T_Sop_T_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Assign_T_Sop_T_Tile starting at " + value);
            return;
        }
        // size 3, cost 1
        if (W_Assign_Mem_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Assign_Mem_Tile starting at " + value);
            return;
        }
        if (Mem_Assign_S_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("Mem_Assign_S_Tile starting at " + value);
            return;
        }
        if (Cjump_T_Label_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("Cjump_T_Label_Tile starting at " + value);
            return;
        }
        if (W_Increment_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Increment_Tile starting at " + value);
            return;
        }
        if (W_Decrement_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Decrement_Tile starting at " + value);
            return;
        }
        // size 2, cost 1
        if (Goto_Label_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("Goto_Label_Tile starting at " + value);
            return;
        }
        if (W_Assign_S_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("W_Assign_S_Tile starting at " + value);
            return;
        }
        // size 1, cost 1
        if (Return_Tile::tileable(*this, root))
        {
            TILE_DEBUG_PRINT("Return_Tile starting at " + value);
            return;
        }

        if (children.size() == 0)
            return;

        throw std::runtime_error("Not tileable");
    }

    // L3 data types: vars, label, number, function_name
    // L2 data types: w, s, t, u
    // w -> registers, var; CHECK: var
    // s -> registers, label, function_name, var, number; CHECK: label, function_name, var, number
    // t -> registers, var, number; CHECK: var, number
    // MEM X M -> STORE/LOAD, CHECK CHILD == VAR

    bool isT(InstructionTree &tree)
    {
        return tree.type == ItemType::Variable || tree.type == ItemType::Number;
    }
    bool isW(InstructionTree &tree)
    {
        return tree.type == ItemType::Variable;
    }
    bool isS(InstructionTree &tree)
    {
        return isT(tree) || tree.type == ItemType::Label || tree.type == ItemType::FunctionName;
    }
    bool isU(InstructionTree &tree)
    {
        return isW(tree) || tree.type == ItemType::FunctionName;
    }

    // size 3, cost: 1
    // bool W_Assign_W_Assign_S_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    // {
    //     if (!isW(tree))
    //         return false;
    //     if (tree.children.size() != 1 || !isW(*tree.children[0]))
    //         return false;
    //     if (tree.children[0]->children.size() != 1 || !isS(*tree.children[0]->children[0]))
    //         return false;

    //     // tileable, call tile_tree on children
    //     root.tiles.push_back(std::make_unique<W_Assign_W_Assign_S_Tile>(tree));
    //     tree.children[0]->children[0]->tile_tree(root);
    //     return true;
    // }

    // size 2, cost: 1
    bool W_Assign_S_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || !isS(*tree.children[0]))
            return false;

        // tileable, call tile_tree on children

        root.tiles.push_back(std::make_unique<W_Assign_S_Tile>(tree));
        tree.children[0]->tile_tree(root);
        return true;
    }

    //  size 3: cost 1
    bool W_Assign_Mem_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {

        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Other || tree.children[0]->value != "$LOAD")
            return false;
        if (tree.children[0]->children.size() != 1 || !isW(*tree.children[0]->children[0]))
            return false;

        // tileable, call tile_tree on children
        root.tiles.push_back(std::make_unique<W_Assign_Mem_Tile>(tree));
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    // size 3: cost 1
    // TODO: MEM offset is a result of merging
    bool Mem_Assign_S_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (tree.type != ItemType::Other || tree.value != "$STORE")
            return false;
        if (tree.children.size() != 2 || !isW(*tree.children[0]) || !isS(*tree.children[1]))
            return false;

        root.tiles.push_back(std::make_unique<Mem_Assign_S_Tile>(tree));
        tree.children[1]->tile_tree(root);
        tree.children[0]->tile_tree(root);
        return true;
    }

    // bool W_Assign_StackArg_M_Tile::tileable(InstructionTree& tree) {};

    // size 4; cost 2
    // w += t
    // w = w + t, w have to be the same
    bool W_Aop_T_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<W_Aop_T_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    // size 4; cost 2
    // w = w << var
    bool W_Sop_Sx_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<W_Sop_Sx_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    // size 4; cost 2
    // w = w << N
    bool W_Sop_N_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<W_Sop_N_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
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
    bool Mem_Increment_T_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<Mem_Increment_T_Tile>(tree));
        tree.children[0]->tile_tree(root);
        tree.children[1]->children[0]->children[0]->tile_tree(root);
        tree.children[1]->children[1]->tile_tree(root);
        return true;
    }

    // size 6; cost 2
    // mem x M -= t
    // Store Var = Load Var + t
    //       Store
    //.   Var     -
    //         Load T
    //          Var
    bool Mem_Decrement_T_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<Mem_Decrement_T_Tile>(tree));
        tree.children[0]->tile_tree(root);
        tree.children[1]->children[0]->children[0]->tile_tree(root);
        tree.children[1]->children[1]->tile_tree(root);
        return true;
    }

    // size 5; cost 2
    //      W
    //      +
    //    W   Load
    //         Var
    bool W_Increment_Mem_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<W_Increment_Mem_Tile>(tree));
        tree.children[0]->children[0]->tile_tree(root);
        tree.children[0]->children[1]->children[0]->tile_tree(root);
        return true;
    }
    // size 5; cost 2
    //      W
    //      -
    //    W   Load
    //         Var
    bool W_Decrement_Mem_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<W_Decrement_Mem_Tile>(tree));
        tree.children[0]->children[0]->tile_tree(root);
        tree.children[0]->children[1]->children[0]->tile_tree(root);
        return true;
    }

    // size 4; cost 2
    bool W_Assign_T_Cmp_T_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Comparator)
            return false;
        if (tree.children[0]->children.size() != 2 || !isT(*tree.children[0]->children[0]) || !isT(*tree.children[0]->children[1]))
            return false;

        root.tiles.push_back(std::make_unique<W_Assign_T_Cmp_T_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    // size 3; cost 1
    bool Cjump_T_Label_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (tree.type != ItemType::Other || tree.value != "$BRANCH")
            return false;

        if (tree.children.size() != 2 || !isT(*tree.children[0]) || tree.children[1]->type != ItemType::Label)
            return false;

        root.tiles.push_back(std::make_unique<Cjump_T_Label_Tile>(tree));
        tree.children[1]->tile_tree(root);
        tree.children[0]->tile_tree(root);
        return true;
    }

    // size 2; cost 1
    bool Goto_Label_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (tree.type != ItemType::Other || tree.value != "$BRANCH")
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Label)
            return false;
        root.tiles.push_back(std::make_unique<Goto_Label_Tile>(tree));
        tree.children[0]->tile_tree(root);
        return true;
    }

    // size 1; cost 1
    bool Return_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (tree.type != ItemType::Other || tree.value != "$RETURN")
            return false;

        root.tiles.push_back(std::make_unique<Return_Tile>(tree));
        if (tree.children.size() == 1 && isT(*tree.children[0]))
        {
            tree.children[0]->tile_tree(root);
        }

        return true;
    }

    // size 3; cost 1
    // w++
    // w = w + 1
    bool W_Increment_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "+")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        if (tree.children[0]->children[1]->value != "1" || tree.children[0]->children[1]->type != ItemType::Number)
            return false;

        root.tiles.push_back(std::make_unique<W_Increment_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    // size 3; cost 1
    // w--
    // w = w - 1
    bool W_Decrement_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (!isW(tree))
            return false;
        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator || tree.children[0]->value != "-")
            return false;
        if (tree.children[0]->children.size() != 2 || !isW(*tree.children[0]->children[0]) || tree.children[0]->children[0]->value != tree.value)
            return false;
        if (tree.children[0]->children[1]->value != "1" || tree.children[0]->children[1]->type != ItemType::Number)
            return false;

        root.tiles.push_back(std::make_unique<W_Decrement_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    /*
            w
            +
          w   *
            w   N

    */
    bool Address_Calculation_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
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

        root.tiles.push_back(std::make_unique<Address_Calculation_Tile>(tree));
        tree.children[0]->children[0]->tile_tree(root);
        tree.children[0]->children[1]->children[1]->tile_tree(root);
        tree.children[0]->children[1]->children[0]->tile_tree(root);
        return true;
    }

    // size 4; cost 2
    /*
            w
            +
          t   t
    */
    bool W_Assign_T_Aop_T_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (!isW(tree))
            return false;

        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator)
            return false;

        std::string op = tree.children[0]->value;
        if (op != "+" && op != "-" && op != "*" && op != "&")
            return false;

        if (tree.children[0]->children.size() != 2 ||
            !isT(*tree.children[0]->children[0]) ||
            !isT(*tree.children[0]->children[1]))
            return false;

        root.tiles.push_back(std::make_unique<W_Assign_T_Aop_T_Tile>(tree));

        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);

        return true;
    }

    // size 4; cost 2
    /*
            w
            <<
          t    t
    */
    bool W_Assign_T_Sop_T_Tile::tileable(InstructionTree &tree, InstructionTree &root)
    {
        if (!isW(tree))
            return false;

        if (tree.children.size() != 1 || tree.children[0]->type != ItemType::Operator)
            return false;

        std::string op = tree.children[0]->value;
        if (op != "<<" && op != ">>")
            return false;

        if (tree.children[0]->children.size() != 2 ||
            !isT(*tree.children[0]->children[0]) ||
            !isT(*tree.children[0]->children[1]))
            return false;

        root.tiles.push_back(std::make_unique<W_Assign_T_Sop_T_Tile>(tree));
        tree.children[0]->children[1]->tile_tree(root);
        tree.children[0]->children[0]->tile_tree(root);
        return true;
    }

    W_Assign_T_Sop_T_Tile::W_Assign_T_Sop_T_Tile(InstructionTree &tree)
    {
        w = tree.item;
        sop = tree.children[0]->item;
        t1 = tree.children[0]->children[0]->item;
        t2 = tree.children[0]->children[1]->item;
    }

    W_Assign_S_Tile::W_Assign_S_Tile(InstructionTree &tree)
    {
        w = tree.item;
        s = tree.children[0]->item;
    }
    W_Assign_Mem_Tile::W_Assign_Mem_Tile(InstructionTree &tree)
    {
        w = tree.item;
        x = tree.children[0]->children[0]->item;
    }
    Mem_Assign_S_Tile::Mem_Assign_S_Tile(InstructionTree &tree)
    {
        x = tree.children[0]->item;
        s = tree.children[1]->item;
    }
    W_Aop_T_Tile::W_Aop_T_Tile(InstructionTree &tree)
    {
        w = tree.item;
        op = tree.children[0]->item;
        t = tree.children[0]->children[1]->item;
    }
    W_Sop_Sx_Tile::W_Sop_Sx_Tile(InstructionTree &tree)
    {
        w = tree.item;
        sop = tree.children[0]->item;
        sx = tree.children[0]->children[1]->item;
    }
    W_Sop_N_Tile::W_Sop_N_Tile(InstructionTree &tree)
    {
        w = tree.item;
        sop = tree.children[0]->item;
        n = tree.children[0]->children[1]->item;
    }
    Mem_Increment_T_Tile::Mem_Increment_T_Tile(InstructionTree &tree)
    {
        x = tree.children[0]->item;
        t = tree.children[1]->children[1]->item;
    }
    Mem_Decrement_T_Tile::Mem_Decrement_T_Tile(InstructionTree &tree)
    {
        x = tree.children[0]->item;
        t = tree.children[1]->children[1]->item;
    }
    W_Increment_Mem_Tile::W_Increment_Mem_Tile(InstructionTree &tree)
    {
        w = tree.children[0]->children[1]->children[0]->item;
    }
    W_Decrement_Mem_Tile::W_Decrement_Mem_Tile(InstructionTree &tree)
    {
        w = tree.children[0]->children[1]->children[0]->item;
    }
    W_Assign_T_Cmp_T_Tile::W_Assign_T_Cmp_T_Tile(InstructionTree &tree)
    {
        w = tree.item;
        t1 = tree.children[0]->children[0]->item;
        cmp = tree.children[0]->item;
        t2 = tree.children[0]->children[1]->item;
    }
    Cjump_T_Label_Tile::Cjump_T_Label_Tile(InstructionTree &tree)
    {
        t = tree.children[0]->item;
        label = tree.children[1]->item;
    }
    Goto_Label_Tile::Goto_Label_Tile(InstructionTree &tree)
    {
        label = tree.children[0]->item;
    }
    Return_Tile::Return_Tile(InstructionTree &tree)
    {
        if (tree.children.size() == 1)
            t = tree.children[0]->item;
    }
    W_Increment_Tile::W_Increment_Tile(InstructionTree &tree)
    {
        w = tree.item;
    }
    W_Decrement_Tile::W_Decrement_Tile(InstructionTree &tree)
    {
        w = tree.item;
    }
    Address_Calculation_Tile::Address_Calculation_Tile(InstructionTree &tree)
    {
        w1 = tree.item;
        w2 = tree.children[0]->children[0]->item;
        w3 = tree.children[0]->children[1]->children[0]->item;
        E = tree.children[0]->children[1]->children[1]->item;
    }
    W_Assign_T_Aop_T_Tile::W_Assign_T_Aop_T_Tile(InstructionTree &tree)
    {
        w = tree.item;
        aop = tree.children[0]->item;
        t1 = tree.children[0]->children[0]->item;
        t2 = tree.children[0]->children[1]->item;
    }
}