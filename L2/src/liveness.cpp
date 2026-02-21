#include "L2.h"

namespace L2 {


    std::vector<std::string> ARG_REGS = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    std::vector<std::string> CALLER_SAVED_REGS = {"r10", "r11", "r8", "rax", "rcx", "rdi", "rdx", "rsi"};
    std::vector<std::string> CALLEE_SAVED_REGS = {"r12", "r13", "r14", "r15", "rbp", "rbx"};

    void Instruction_ret::set_gen_set() {
        gen_set.insert("rax");
        gen_set.insert(CALLEE_SAVED_REGS.begin(), CALLEE_SAVED_REGS.end());
    }
    void Instruction_ret::set_kill_set() {
        return;
    }

    void Instruction_S_to_W_assignment::set_gen_set() {
        if (s->type == ItemType::Register || s->type == ItemType::Variable){
            gen_set.insert(s->to_string());
        }
    }
    void Instruction_S_to_W_assignment::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_Mem_to_W_assignment::set_gen_set() {
        if (memory_access->get_X()->to_string() != "rsp")
            gen_set.insert(memory_access->get_X()->to_string());
    }
    void Instruction_Mem_to_W_assignment::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_S_to_Mem_assignment::set_gen_set() {
        if (s->type == ItemType::Register || s->type == ItemType::Variable){
            gen_set.insert(s->to_string());
        }
        if (memory_access->get_X()->to_string() != "rsp")
            gen_set.insert(memory_access->get_X()->to_string());
    }
    void Instruction_S_to_Mem_assignment::set_kill_set() {
        // nothing happens, value inside memory_access->get_X() does not change (an address), the value at the offset addresss changes
    }

    void Instruction_Stack_Arg_M_to_W::set_gen_set() {
        // nothing
    }
    void Instruction_Stack_Arg_M_to_W::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_W_aop_T::set_gen_set() {
        gen_set.insert(w->to_string());
        if (t->type == ItemType::Register || t->type == ItemType::Variable){
            gen_set.insert(t->to_string());
        }
    }
    void Instruction_W_aop_T::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_W_sop_SX::set_gen_set() {
        gen_set.insert(w->to_string());
        gen_set.insert(sx->to_string());
    }
    void Instruction_W_sop_SX::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_W_sop_N::set_gen_set() {
        gen_set.insert(w->to_string());
    }
    void Instruction_W_sop_N::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_Mem_increment_T::set_gen_set() {
        if (t->type == ItemType::Register || t->type == ItemType::Variable){
            gen_set.insert(t->to_string());
        }
        if (memory_access->get_X()->to_string() != "rsp")
            gen_set.insert(memory_access->get_X()->to_string());
    }
    void Instruction_Mem_increment_T::set_kill_set() {
        // nothing
    }

    void Instruction_Mem_decrement_T::set_gen_set() {
        if (t->type == ItemType::Register || t->type == ItemType::Variable){
            gen_set.insert(t->to_string());
        }
        if (memory_access->get_X()->to_string() != "rsp")
            gen_set.insert(memory_access->get_X()->to_string());
    }
    void Instruction_Mem_decrement_T::set_kill_set() {
        // nothing
    }

    void Instruction_W_increment_Mem::set_gen_set() {
        gen_set.insert(w->to_string());
        if (memory_access->get_X()->to_string() != "rsp")
            gen_set.insert(memory_access->get_X()->to_string());
    }
    void Instruction_W_increment_Mem::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_W_decrement_Mem::set_gen_set() {
        gen_set.insert(w->to_string());
        if (memory_access->get_X()->to_string() != "rsp")
            gen_set.insert(memory_access->get_X()->to_string());
    }
    void Instruction_W_decrement_Mem::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_label::set_gen_set() {}
    void Instruction_label::set_kill_set() {}
    void Instruction_goto_label::set_gen_set() {}
    void Instruction_goto_label::set_kill_set() {}

    void Instruction_function_call::set_gen_set() {
        if (call_target->type == ItemType::Register || call_target->type == ItemType::Variable){
            gen_set.insert(call_target->to_string());
        }
        // insert args
        for (int i = 0; i < std::min<int>(arg_count, 6); i++) {
            gen_set.insert(ARG_REGS[i]);
        }
    }
    void Instruction_function_call::set_kill_set() {
        kill_set.insert(CALLER_SAVED_REGS.begin(), CALLER_SAVED_REGS.end());
    }

    void Instruction_print_call::set_gen_set() {
        gen_set.insert(ARG_REGS[0]);
    }
    void Instruction_print_call::set_kill_set() {
        kill_set.insert(CALLER_SAVED_REGS.begin(), CALLER_SAVED_REGS.end());
    }
    
    void Instruction_W_T_cmp_T::set_gen_set() {
        if (operand1->type == ItemType::Variable || operand1->type == ItemType::Register){
            gen_set.insert(operand1->to_string());
        }
        if (operand2->type == ItemType::Variable || operand2->type == ItemType::Register){
            gen_set.insert(operand2->to_string());
        }
    }
    void Instruction_W_T_cmp_T::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_cjump_T_cmp_T_label::set_gen_set() {
        if (operand1->type == ItemType::Variable || operand1->type == ItemType::Register){
            gen_set.insert(operand1->to_string());
        }
        if (operand2->type == ItemType::Variable || operand2->type == ItemType::Register){
            gen_set.insert(operand2->to_string());
        }
    }
    void Instruction_cjump_T_cmp_T_label::set_kill_set() {
        // nothing
    }

    void Instruction_input::set_gen_set() {
        // nothing
    }
    void Instruction_input::set_kill_set() {
        kill_set.insert(CALLER_SAVED_REGS.begin(), CALLER_SAVED_REGS.end());
    }

    void Instruction_allocate::set_gen_set() {
        gen_set.insert(ARG_REGS[0]);
        gen_set.insert(ARG_REGS[1]);
    }
    void Instruction_allocate::set_kill_set() {
        kill_set.insert(CALLER_SAVED_REGS.begin(), CALLER_SAVED_REGS.end());
    }

    void Instruction_tuple_error::set_gen_set() {
        gen_set.insert(ARG_REGS[0]);
        gen_set.insert(ARG_REGS[1]);
        gen_set.insert(ARG_REGS[2]);
    }
    void Instruction_tuple_error::set_kill_set() {
        kill_set.insert(CALLER_SAVED_REGS.begin(), CALLER_SAVED_REGS.end());
    }

    void Instruction_tensor_error::set_gen_set() {
        for (int i = 0; i < f->number; i++){
            gen_set.insert(ARG_REGS[i]);
        }
    }
    void Instruction_tensor_error::set_kill_set() {
        kill_set.insert(CALLER_SAVED_REGS.begin(), CALLER_SAVED_REGS.end());
    }

    void Instruction_W_increment::set_gen_set() {
        gen_set.insert(w->to_string());
    }
    void Instruction_W_increment::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_W_decrement::set_gen_set() {
        gen_set.insert(w->to_string());
    }
    void Instruction_W_decrement::set_kill_set() {
        kill_set.insert(w->to_string());
    }

    void Instruction_address_calculation::set_gen_set() {
        gen_set.insert(w2->to_string());
        gen_set.insert(w3->to_string());
    }
    void Instruction_address_calculation::set_kill_set() {
        kill_set.insert(w1->to_string());
    }

}