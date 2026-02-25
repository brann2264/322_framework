#include "L2.h"

namespace L2 {

    // pair.first -> need to add a new memory load
    // pair.second -> need to add a new memory store

    bool MemoryAccess::spill(std::string var_name, std::string spilled_name){
        Variable* x_var = dynamic_cast<Variable*>(x.get());

        if (x_var != nullptr){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            x = std::move(replacement_var);
            return true;
        }
        return false;
    }

    std::pair<bool, bool> Instruction_S_to_W_assignment::spill(std::string var_name, std::string spilled_name) {
        
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());
        Variable* s_var = dynamic_cast<Variable*>(s.get());

        std::pair<bool, bool> hasSpilled = {false, false};
        
        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.second = true;
        }
        if (s_var != nullptr && s_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            s = std::move(replacement_var);
            hasSpilled.first = true;
        }

        return hasSpilled;
    }

    std::pair<bool, bool> Instruction_Mem_to_W_assignment::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.second = true;
        }
        if (memory_access->spill(var_name, spilled_name)) {
            hasSpilled.first = true;
        }

        return hasSpilled;
    }

    std::pair<bool, bool> Instruction_S_to_Mem_assignment::spill(std::string var_name, std::string spilled_name) {
        
        Variable* s_var = dynamic_cast<Variable*>(s.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (s_var != nullptr && s_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            s = std::move(replacement_var);
            hasSpilled.first = true;
        }
        if (memory_access->spill(var_name, spilled_name)) {
            hasSpilled.first = true;
        }
        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_Stack_Arg_M_to_W::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            return {false, true};
        }
        return {false, false};
    }
    std::pair<bool, bool> Instruction_W_aop_T::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());
        Variable* t_var = dynamic_cast<Variable*>(t.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.first = true;
            hasSpilled.second = true;
        }
        if (t_var != nullptr && t_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            t = std::move(replacement_var);
            hasSpilled.first = true;
        }

        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_W_sop_SX::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());
        Variable* sx_var = dynamic_cast<Variable*>(sx.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.first = true;
            hasSpilled.second = true;
        }
        if (sx_var != nullptr && sx_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            sx = std::move(replacement_var);
            hasSpilled.first = true;
        }

        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_W_sop_N::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            return {true, true};
        }
        return {false, false};
    }
    std::pair<bool, bool> Instruction_Mem_increment_T::spill(std::string var_name, std::string spilled_name) {
        
        Variable* t_var = dynamic_cast<Variable*>(t.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (t_var != nullptr && t_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            t = std::move(replacement_var);
            hasSpilled.first = true;
        }
        if (memory_access->spill(var_name, spilled_name)){
            hasSpilled.first = true;
        }
        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_Mem_decrement_T::spill(std::string var_name, std::string spilled_name) {
        
        Variable* t_var = dynamic_cast<Variable*>(t.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (t_var != nullptr && t_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            t = std::move(replacement_var);
            hasSpilled.first = true;
        }
        if (memory_access->spill(var_name, spilled_name)){
            hasSpilled.first = true;
        }
        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_W_increment_Mem::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.first = true;
            hasSpilled.second = true;
        }
        if (memory_access->spill(var_name, spilled_name)){
            hasSpilled.first = true;
        }

        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_W_decrement_Mem::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.first = true;
            hasSpilled.second = true;
        }
        if (memory_access->spill(var_name, spilled_name)){
            hasSpilled.first = true;
        }

        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_function_call::spill(std::string var_name, std::string spilled_name) {
        
        Variable* u_var = dynamic_cast<Variable*>(call_target.get());

        if (u_var != nullptr && u_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            call_target = std::move(replacement_var);
            return {true, false};
        }
        return {false, false};
    }
    std::pair<bool, bool> Instruction_W_T_cmp_T::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());
        Variable* t1_var = dynamic_cast<Variable*>(operand1.get());
        Variable* t2_var = dynamic_cast<Variable*>(operand2.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            hasSpilled.second = true;
        }
        if (t1_var != nullptr && t1_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            operand1 = std::move(replacement_var);
            hasSpilled.first = true;
        }
        if (t2_var != nullptr && t2_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            operand2 = std::move(replacement_var);
            hasSpilled.first = true;
        }
        return hasSpilled;
    }
    std::pair<bool, bool> Instruction_cjump_T_cmp_T_label::spill(std::string var_name, std::string spilled_name) {
        
        Variable* t1_var = dynamic_cast<Variable*>(operand1.get());
        Variable* t2_var = dynamic_cast<Variable*>(operand2.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (t1_var != nullptr && t1_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            operand1 = std::move(replacement_var);
            hasSpilled.first = true;
        }
        if (t2_var != nullptr && t2_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            operand2 = std::move(replacement_var);
            hasSpilled.first = true;
        }
        return hasSpilled;
    }
    std::pair<bool,bool> Instruction_W_increment::spill(std::string var_name, std::string spilled_name){
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            return {true, true};
        }
        return {false, false};
    }

    std::pair<bool, bool> Instruction_W_decrement::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w_var = dynamic_cast<Variable*>(w.get());

        if (w_var != nullptr && w_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w = std::move(replacement_var);
            return {true, true};
        }
        return {false, false};
    }
    std::pair<bool, bool> Instruction_address_calculation::spill(std::string var_name, std::string spilled_name) {
        
        Variable* w1_var = dynamic_cast<Variable*>(w1.get());
        Variable* w2_var = dynamic_cast<Variable*>(w2.get());
        Variable* w3_var = dynamic_cast<Variable*>(w3.get());

        std::pair<bool, bool> hasSpilled = {false, false};

        if (w1_var != nullptr && w1_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w1 = std::move(replacement_var);
            hasSpilled.second = true;
        }
        if (w2_var != nullptr && w2_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w2 = std::move(replacement_var);
            hasSpilled.first = true;
        }
        if (w3_var != nullptr && w3_var->var_name == var_name){
            std::unique_ptr<Variable> replacement_var = std::make_unique<Variable>(spilled_name);
            w3 = std::move(replacement_var);
            hasSpilled.first = true;
        }
        return hasSpilled;
    }

    void Function::spill_var(std::string var_name, std::string spill_prefix){

        std::vector<std::unique_ptr<Instruction>> new_Instructions;
        bool hasSpilled = false;

        for (int i = 0; i < instructions.size(); i++){

            std::pair<bool, bool> isSpilled = instructions[i]->spill(var_name, spill_prefix + std::to_string(spill_count));

            // std::cout << isSpilled.first << " , " << isSpilled.second << std::endl;

            if (isSpilled.first){
                std::unique_ptr<Variable> w = std::make_unique<Variable>(spill_prefix + std::to_string(spill_count));

                std::unique_ptr<Register> rsp = std::make_unique<Register>(ERegister::rsp);
                std::unique_ptr<Number> offset = std::make_unique<Number>(locals* 8);
                std::unique_ptr<MemoryAccess> memory_access = std::make_unique<MemoryAccess>(std::move(rsp), std::move(offset));

                std::unique_ptr<Instruction_Mem_to_W_assignment> new_load = std::make_unique<Instruction_Mem_to_W_assignment>(std::move(memory_access), std::move(w));
                new_Instructions.push_back(std::move(new_load));
            }

            new_Instructions.push_back(std::move(instructions[i]));

            if (isSpilled.second){
                std::unique_ptr<Variable> s = std::make_unique<Variable>(spill_prefix + std::to_string(spill_count));

                std::unique_ptr<Register> rsp = std::make_unique<Register>(ERegister::rsp);
                std::unique_ptr<Number> offset = std::make_unique<Number>(locals * 8);
                std::unique_ptr<MemoryAccess> memory_access = std::make_unique<MemoryAccess>(std::move(rsp), std::move(offset));

                std::unique_ptr<Instruction_S_to_Mem_assignment> new_store = std::make_unique<Instruction_S_to_Mem_assignment>(std::move(s), std::move(memory_access));
                
                new_Instructions.push_back(std::move(new_store));
            }

            if (isSpilled.first || isSpilled.second){
                spill_count++;
                hasSpilled = true;
            }

        }

        instructions = std::move(new_Instructions);

        if (hasSpilled)
            locals++;
    }

    void Program::spill_test(bool verbose){

        for (auto& f : functions){
            f->spill_var(spill_variable, spill_prefix);

            if (verbose) std::cout << f->to_string() << std::endl;
        }
    }


}