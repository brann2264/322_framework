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
#include "instruction_selection.h"

namespace L3 {

  class Program;
  class Function;

  enum class ItemType{Item, Name, Label, FunctionName, Number, Comparator, Operator, Variable, Other};
  inline int64_t itemTypeToInt(ItemType type) {
    switch (type) {
        case ItemType::Item:           return 0;
        case ItemType::Name:           return 1;
        case ItemType::Label:          return 2;
        case ItemType::FunctionName:   return 3;
        case ItemType::Number:         return 4;
        case ItemType::Comparator:     return 5;
        case ItemType::Variable:       return 6; 
        default:
            throw std::invalid_argument("Unknown ItemType");
    }
  }
  inline std::vector<std::string> ARGUMENT_REGISTERS = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
  class Item {
    public:
      virtual ~Item() = default;
      virtual std::string to_string() const = 0;
      ItemType type = ItemType::Item;

      virtual void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const = 0;
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

  struct Callee : Item {
    public:
      std::string name; // name will be % if there is a u present
      std::shared_ptr<U> u;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  struct Name : Item {
    std::string name;

    Name(std::string _name) : name(_name) {type = ItemType::Name;}

    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  struct Label : S {
    std::string label_name;

    Label(std::string _label_name) : label_name(_label_name) {type = ItemType::Label;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  struct FunctionName: S, U {
    std::string function_name;

    FunctionName(std::string _function_name) : function_name(_function_name) {type = ItemType::FunctionName;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  struct Number : T {
    int64_t number;

    Number(int64_t _number) : number(_number) {type = ItemType::Number;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  struct Variable : T, U {
    std::string var_name;

    Variable(std::string _var_name) : var_name(_var_name) {type = ItemType::Variable;};

    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  enum class EComparator {LT,LTE,EQ,GT,GTE};

  struct Comparator : Item {
    EComparator cmp;

    Comparator(EComparator _cmp) : cmp(_cmp) {type = ItemType::Comparator;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  enum class EOperator {ADD,SUB,MULT,AND,LEFT_SHIFT,RIGHT_SHIFT};

  struct Operator : Item {
    EOperator op;

    Operator(EOperator _op) : op(_op) {type = ItemType::Operator;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  /*
   * Instruction interface.
   */
  class Instruction {
    public:
      virtual ~Instruction() = default;
      virtual std::string to_string() const = 0;
      virtual std::unique_ptr<InstructionTree> generate_tree() const {
        throw std::runtime_error("UNREACHABLE");
      }
      virtual void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
        throw std::runtime_error("UNREACHABLE");
      };
  };

  /*
   * Instructions.
   */

  class Instruction_Var_S_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<S> s;

      Instruction_Var_S_Assignment(std::shared_ptr<Variable> _var, std::shared_ptr<S> _s) : var(std::move(_var)), s(std::move(_s)){}
      std::unique_ptr<InstructionTree> generate_tree() const override;
      std::string to_string() const override;
  };

  class Instruction_Var_T_Op_T_Assignment: public Instruction {
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<T> t1;
      std::shared_ptr<Operator> op;
      std::shared_ptr<T> t2;

      Instruction_Var_T_Op_T_Assignment(std::shared_ptr<Variable> _var, std::shared_ptr<T> _t1, 
        std::shared_ptr<Operator> _op, std::shared_ptr<T> _t2) : var(std::move(_var)), t1(std::move(_t1)), 
                                                                op(std::move(_op)), t2(std::move(_t2)){}
      std::unique_ptr<InstructionTree> generate_tree() const override;
      std::string to_string() const override;
  };

  class Instruction_Var_T_Cmp_T_Assignment: public Instruction {
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<T> t1;
      std::shared_ptr<Comparator> cmp;
      std::shared_ptr<T> t2;

      Instruction_Var_T_Cmp_T_Assignment(std::shared_ptr<Variable> _var, std::shared_ptr<T> _t1, 
        std::shared_ptr<Comparator> _cmp, std::shared_ptr<T> _t2): var(std::move(_var)), t1(std::move(_t1)), 
                                                                cmp(std::move(_cmp)), t2(std::move(_t2)){}
      std::unique_ptr<InstructionTree> generate_tree() const override;
      std::string to_string() const override;
  };

  class Instruction_Var_Load_Var_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var1;
      std::shared_ptr<Variable> var2;

      Instruction_Var_Load_Var_Assignment(std::shared_ptr<Variable> _var1, 
        std::shared_ptr<Variable> _var2) : var1(std::move(_var1)), var2(std::move(_var2)){}
      std::unique_ptr<InstructionTree> generate_tree() const override;
      std::string to_string() const override;
  };

  class Instruction_Store_Var_S_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<S> s;

      Instruction_Store_Var_S_Assignment(std::shared_ptr<Variable> _var, std::shared_ptr<S> _s)
        : var(std::move(_var)), s(std::move(_s)){}
      std::unique_ptr<InstructionTree> generate_tree() const override;
      std::string to_string() const override;
  };

  class Instruction_Return : public Instruction{
    public:
      std::string to_string() const override;
      // void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
      std::unique_ptr<InstructionTree> generate_tree() const override;
  };

  class Instruction_Return_T: public Instruction{
    public:
      std::shared_ptr<T> t;

      Instruction_Return_T(std::shared_ptr<T> _t) : t(std::move(_t)){}

      std::string to_string() const override;
      // void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
      std::unique_ptr<InstructionTree> generate_tree() const override;
  };

  class Instruction_Label: public Instruction{
    public:
      std::shared_ptr<Label> label;

      Instruction_Label(std::shared_ptr<Label> _label) : label(std::move(_label)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  class Instruction_Br_Label: public Instruction{
    public:
      std::shared_ptr<Label> label;

      Instruction_Br_Label(std::shared_ptr<Label> _label) : label(std::move(_label)){}
    
      std::string to_string() const override;
      std::unique_ptr<InstructionTree> generate_tree() const override;
  };

  class Instruction_Br_T_Label: public Instruction{
    public:
      std::shared_ptr<Label> label;
      std::shared_ptr<T> t;

      Instruction_Br_T_Label(std::shared_ptr<T> _t, std::shared_ptr<Label> _label)
        : label(std::move(_label)), t(std::move(_t)){}
    
      std::string to_string() const override;
      std::unique_ptr<InstructionTree> generate_tree() const override;
  };

  class Instruction_Call_Function: public Instruction{
    public:
      std::shared_ptr<Callee> callee;
      std::vector<std::shared_ptr<T>> args;
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  class Instruction_Var_Function_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::unique_ptr<Instruction_Call_Function> function_call_instruction;

      Instruction_Var_Function_Assignment(std::shared_ptr<Variable> _var, std::unique_ptr<Instruction_Call_Function> _function_call_instruction) : var(std::move(_var)), 
        function_call_instruction(std::move(_function_call_instruction)) {}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const override;
  };

  /*
   * Function.
   */
  class Function{
    public:
      std::shared_ptr<FunctionName> function_name;
      std::vector<std::shared_ptr<Variable>> vars;
      std::vector<std::unique_ptr<Instruction>> instructions;
      std::vector<std::unique_ptr<Context>> contexts;
      std::unordered_map<std::string, std::string> local_to_global_label_map;

      std::string to_string() const;
      void create_contexts();
      void generate_code(std::ofstream& stream, Program& global_scope);
  };

  class Program{
    public:
      std::vector<std::unique_ptr<Function>> functions;
      std::string longest_label;
      int label_count = 0;

      std::string to_string() const;
      std::string label_to_global(std::string label_name, Function& function);
      std::string generate_global_label();
      void generate_code(std::ofstream& stream);
  };

}
