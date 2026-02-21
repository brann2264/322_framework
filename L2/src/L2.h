#pragma once

#include <vector>
#include <string>
#include <variant>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <set>
#include <unordered_map>

#include "interference_graph.h"

namespace L2 {

  enum class ItemType{Item, Name, Label, FunctionName, Number, Comparator, ShiftOp, AssignmentOp, Register, MemoryAccess, Variable};
  inline int64_t itemTypeToInt(ItemType type) {
    switch (type) {
        case ItemType::Item:           return 0;
        case ItemType::Name:           return 1;
        case ItemType::Label:          return 2;
        case ItemType::FunctionName:   return 3;
        case ItemType::Number:         return 4;
        case ItemType::Comparator:     return 5;
        case ItemType::ShiftOp:        return 6;
        case ItemType::AssignmentOp:   return 7;
        case ItemType::Register:       return 8;
        case ItemType::MemoryAccess:   return 9;
        case ItemType::Variable:       return 10; 
        default:
            throw std::invalid_argument("Unknown ItemType");
    }
  }
  class Item {
    public:
      virtual ~Item() = default;
      virtual std::string to_string() const = 0;
      virtual void generate_code(std::ofstream& stream) const = 0;
      ItemType type = ItemType::Item;
  };

  struct U : virtual Item {
    public:
      virtual ~U() = default;
  };
  struct S : virtual Item {
    public:
      virtual ~S() = default;
  };
  struct T : virtual S {
    public:
      virtual ~T() = default;
  };
  struct X: virtual T, U {
    public:
      virtual ~X() = default;
  };

  // x is all register + variables
  // var -> sx -> a -> w -> x
  // put everything down as X, then check to exclude rsp as a register for W

  struct Name : Item {
    std::string name;

    Name(std::string _name) : name(_name) {type = ItemType::Name;}

    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  struct Label : S {
    std::string label_name;

    Label(std::string _label_name) : label_name(_label_name) {type = ItemType::Label;}
    
    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  struct FunctionName: S, U {
    std::string function_name;

    FunctionName(std::string _function_name) : function_name(_function_name) {type = ItemType::FunctionName;}
    
    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  struct Number : T {
    int64_t number;

    Number(int64_t _number) : number(_number) {type = ItemType::Number;}
    
    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  struct Variable : X {
    std::string var_name;

    Variable(std::string _var_name) : var_name(_var_name) {type = ItemType::Variable;};

    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  enum class EComparator {LT,LTE,EQ};

  struct Comparator : Item {
    EComparator cmp;

    Comparator(EComparator _cmp) : cmp(_cmp) {type = ItemType::Comparator;}
    
    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  enum class EShiftOperator {RIGHT,LEFT};

  struct ShiftOp : Item {
    enum EShiftOperator shift_op;

    ShiftOp(EShiftOperator _shift_op) : shift_op(_shift_op) {type = ItemType::ShiftOp;}
    
    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  enum class EAssignmentOperator {INCREMENT,DECREMENT,MULTIPLY,AND};

  struct AssignmentOp : Item {
    EAssignmentOperator assign_op;

    AssignmentOp(EAssignmentOperator _assign_op) : assign_op(_assign_op) {type = ItemType::AssignmentOp;}
    
    void generate_code(std::ofstream& stream) const override;
    std::string to_string() const override;
  };

  enum class ERegister {rdi,rsi,rdx,r8,r9,rax,rsp,rcx};
  struct Register : X {
    ERegister reg;

    Register(ERegister _reg) : reg(_reg) {type = ItemType::Register;}
    
    void generate_code(std::ofstream& stream) const override;
    void generate_code_b(std::ofstream& stream) const;
    std::string to_string() const override;
  };

  // Common intermediate items
  class MemoryAccess : public Item{
    public: 
      MemoryAccess(std::unique_ptr<X> x, std::unique_ptr<Number> M);
      
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      X* get_X() const {
        return x.get();
      }

    private:
      std::unique_ptr<X> x;
      std::unique_ptr<Number> offset;
  };

  /*
   * Instruction interface.
   */
  class Instruction {
    public:
      virtual ~Instruction() = default;
      virtual std::string to_string() const = 0;
      virtual void generate_code(std::ofstream& stream) const = 0;
      virtual void set_gen_set() = 0;
      virtual void set_kill_set() = 0;

      virtual std::vector<int> get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const {
        return {instruction_idx+1};
      }

      std::set<std::string> gen_set;
      std::set<std::string> kill_set;
      std::set<std::string> in_set;
      std::set<std::string> out_set;
  };

  /*
   * Instructions.
   */
  class Instruction_ret : public Instruction{
    public:
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
      std::vector<int> get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const override;
  

      int64_t arguments;
      int64_t locals;

  };

  class Instruction_S_to_W_assignment : public Instruction{
    public:
      Instruction_S_to_W_assignment (std::unique_ptr<S> s, std::unique_ptr<X> w);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<S> s;
      std::unique_ptr<X> w;
  };

  class Instruction_Mem_to_W_assignment : public Instruction{
    public:
      Instruction_Mem_to_W_assignment (std::unique_ptr<MemoryAccess> memory_access, std::unique_ptr<X> w);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<MemoryAccess> memory_access;
      std::unique_ptr<X> w;
  };

  class Instruction_S_to_Mem_assignment : public Instruction{
    public:
      Instruction_S_to_Mem_assignment (std::unique_ptr<S> s, std::unique_ptr<MemoryAccess> memory_access);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<S> s;
      std::unique_ptr<MemoryAccess> memory_access;
  };

  class Instruction_Stack_Arg_M_to_W : public Instruction{
    public:
      Instruction_Stack_Arg_M_to_W (std::unique_ptr<Number> m, std::unique_ptr<X> w);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<Number> m;
      std::unique_ptr<X> w;
  };

  class Instruction_W_aop_T : public Instruction {
    public:
      Instruction_W_aop_T (std::unique_ptr<X> w, std::unique_ptr<AssignmentOp> aop, std::unique_ptr<T> t);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<X> w;
      std::unique_ptr<AssignmentOp> aop;
      std::unique_ptr<T> t;
  };

  class Instruction_W_sop_SX : public Instruction {
    public:
      Instruction_W_sop_SX (std::unique_ptr<X> w, std::unique_ptr<ShiftOp> sop, std::unique_ptr<X> sx);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<X> w;
      std::unique_ptr<ShiftOp> sop;
      std::unique_ptr<X> sx;
  };

  class Instruction_W_sop_N : public Instruction {
    public:
      Instruction_W_sop_N (std::unique_ptr<X> w, std::unique_ptr<ShiftOp> sop, std::unique_ptr<Number> number);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<X> w;
      std::unique_ptr<ShiftOp> sop;
      std::unique_ptr<Number> num;
  };

  class Instruction_Mem_increment_T : public Instruction {
    public:
      Instruction_Mem_increment_T(std::unique_ptr<MemoryAccess> mem, std::unique_ptr<T> t);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<MemoryAccess> memory_access;
      std::unique_ptr<T> t;
  };

  class Instruction_Mem_decrement_T : public Instruction {
    public:
      Instruction_Mem_decrement_T(std::unique_ptr<MemoryAccess> mem, std::unique_ptr<T> t);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<MemoryAccess> memory_access;
      std::unique_ptr<T> t;
  };

  class Instruction_W_increment_Mem : public Instruction {
    public:
      Instruction_W_increment_Mem(std::unique_ptr<X> w, std::unique_ptr<MemoryAccess> mem);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<MemoryAccess> memory_access;
      std::unique_ptr<X> w;
  };

  class Instruction_W_decrement_Mem : public Instruction {
    public:
      Instruction_W_decrement_Mem(std::unique_ptr<X> w, std::unique_ptr<MemoryAccess> mem);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<MemoryAccess> memory_access;
      std::unique_ptr<X> w;
  };

  class Instruction_label : public Instruction {
    public:
      Instruction_label (std::string _label_name) : label_name(_label_name) {};
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::string label_name;
  };

  class Instruction_function_call : public Instruction {
    public:
      Instruction_function_call(std::unique_ptr<U> call_target, int64_t arg_count);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;

    private:
      std::unique_ptr<U> call_target;
      int64_t arg_count;
  };

  class Instruction_print_call : public Instruction {
    public:
      Instruction_print_call() {}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
  };

  class Instruction_W_T_cmp_T : public Instruction {
    public:
      Instruction_W_T_cmp_T(std::unique_ptr<X> w, std::unique_ptr<T> operand1, std::unique_ptr<Comparator> cmp, std::unique_ptr<T> operand2);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
    
    private:
      std::unique_ptr<X> w;
      std::unique_ptr<T> operand1;
      std::unique_ptr<Comparator> cmp;
      std::unique_ptr<T> operand2;
  };

  class Instruction_cjump_T_cmp_T_label: public Instruction{
    public:
      Instruction_cjump_T_cmp_T_label(std::unique_ptr<T> operand1, std::unique_ptr<Comparator> cmp, std::unique_ptr<T> operand2, std::unique_ptr<Label> label);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
      std::vector<int> get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const override;
  
    private:
      std::unique_ptr<T> operand1;
      std::unique_ptr<Comparator> cmp;
      std::unique_ptr<T> operand2;
      std::unique_ptr<Label> label;
  };

  class Instruction_goto_label: public Instruction {
    public:
      Instruction_goto_label(std::unique_ptr<Label> label);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
      std::vector<int> get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const override;
    
    private:
      std::unique_ptr<Label> label;
  };

  class Instruction_input: public Instruction {
    public:
      Instruction_input(){}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
  };
  class Instruction_allocate: public Instruction {
    public:
      Instruction_allocate(){}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
  };
  class Instruction_tuple_error: public Instruction {
    public:
      Instruction_tuple_error(){}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
      std::vector<int> get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const override;
  };
  class Instruction_tensor_error: public Instruction {
    public:
      Instruction_tensor_error(std::unique_ptr<Number> F);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
      std::vector<int> get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const override;
    
    private:
      std::unique_ptr<Number> f;
  };
  class Instruction_W_increment: public Instruction {
    public:
      Instruction_W_increment(std::unique_ptr<X> w);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
    
    private:
      std::unique_ptr<X> w;
  };
  class Instruction_W_decrement: public Instruction {
    public:
      Instruction_W_decrement(std::unique_ptr<X> w);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
    
    private:
      std::unique_ptr<X> w;
  };
  class Instruction_address_calculation: public Instruction {
    public:
      Instruction_address_calculation(std::unique_ptr<X> w1, std::unique_ptr<X> w2, std::unique_ptr<X> w3, std::unique_ptr<Number> num);
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      void set_gen_set() override;
      void set_kill_set() override;
    
    private:
      std::unique_ptr<X> w1;
      std::unique_ptr<X> w2;
      std::unique_ptr<X> w3;
      std::unique_ptr<Number> num;
  };

  /*
   * Function.
   */
  class Function{
    public:
      std::string name;
      int64_t arguments;
      std::vector<std::unique_ptr<Instruction>> instructions;
      std::unordered_map<std::string, Node> graph;

      std::unordered_map<std::string, int> labels_index;
      std::string to_string() const;
      void generate_code(std::ofstream& stream) const;
      void determine_liveness(bool verbose);
      void construct_graph(bool verbose);
      void color_graph();
  };

  class Program{
    public:
      std::string entryPointLabel;
      std::vector<std::unique_ptr<Function>> functions;

      std::string to_string() const;
      void generate_code(std::ofstream& stream) const;
      void determine_liveness(bool verbose);
      void construct_graphs(bool verbose);

  };

}
