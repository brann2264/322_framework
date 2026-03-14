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
#include <unordered_set>

namespace IR {

  class Program;
  class Function;

  enum class ItemType{Item, Name, Label, FunctionName, Number, Operator, Variable, Type};
  inline int64_t itemTypeToInt(ItemType type) {
    switch (type) {
        case ItemType::Item:           return 0;
        case ItemType::Name:           return 1;
        case ItemType::Label:          return 2;
        case ItemType::FunctionName:   return 3;
        case ItemType::Number:         return 4;
        case ItemType::Variable:       return 5; 
        case ItemType::Type:            return 6; 
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

      virtual void generate_code(std::ofstream& stream) const = 0;
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
      void generate_code(std::ofstream& stream) const override;
  };

  struct Name : Item {
    std::string name;

    Name(std::string _name) : name(_name) {type = ItemType::Name;}

    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  struct Label : S {
    std::string label_name;

    Label(std::string _label_name) : label_name(_label_name) {type = ItemType::Label;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  struct FunctionName: S, U {
    std::string function_name;

    FunctionName(std::string _function_name) : function_name(_function_name) {type = ItemType::FunctionName;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  struct Number : T {
    int64_t number;

    Number(int64_t _number) : number(_number) {type = ItemType::Number;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  struct Variable : T, U {
    std::string var_name;

    Variable(std::string _var_name) : var_name(_var_name) {type = ItemType::Variable;};

    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  enum class EOperator {ADD,SUB,MULT,AND,LEFT_SHIFT,RIGHT_SHIFT,LT,LTE,EQ,GT,GTE};

  struct Operator : Item {
    EOperator op;

    Operator(EOperator _op) : op(_op) {type = ItemType::Operator;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  enum class EType {INT, TUPLE, CODE, ARRAY, VOID};

  struct Type : Item {
    EType var_type;
    int array_dims;

    Type() {type = ItemType::Type;}
    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  struct TypeDef : Item {
    std::shared_ptr<Variable> var;
    std::shared_ptr<Type> type;

    TypeDef(std::shared_ptr<Type> _type, std::shared_ptr<Variable> _var) : type(std::move(_type)), var(std::move(_var)) {}
    std::string to_string() const override;
    void generate_code(std::ofstream& stream) const override;
  };

  /*
   * Instruction interface.
   */
  class Instruction {
    public:
      virtual ~Instruction() = default;
      virtual std::string to_string() const = 0;
      virtual void generate_code(std::ofstream& stream) const {
        throw std::runtime_error("UNREACHABLE");
      };
  };

  /*
   * Instructions.
   */

  class Instruction_Var_Def: public Instruction {
    public:
      std::shared_ptr<TypeDef> type_def;

      Instruction_Var_Def(std::shared_ptr<TypeDef> _type_def) : type_def(std::move(_type_def)){}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Var_S_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<S> s;

      Instruction_Var_S_Assignment(std::shared_ptr<Variable> _var, std::shared_ptr<S> _s) : var(std::move(_var)), s(std::move(_s)){}
      
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
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
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
      
  };

  class Instruction_Var_Array_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<Variable> arr_var;
      std::vector<std::shared_ptr<T>> idxs;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Array_S_Assignment: public Instruction{
    public:
      std::shared_ptr<S> s;
      std::shared_ptr<Variable> arr_var;
      std::vector<std::shared_ptr<T>> idxs;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Var_Length_Var_T_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var1;
      std::shared_ptr<Variable> var2;
      std::shared_ptr<T> t;

      Instruction_Var_Length_Var_T_Assignment(std::shared_ptr<Variable> _var1, std::shared_ptr<Variable> _var2, std::shared_ptr<T> _t)
      : var1(std::move(_var1)), var2(std::move(_var2)), t(std::move(_t)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Var_Length_Var_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var1;
      std::shared_ptr<Variable> var2;

      Instruction_Var_Length_Var_Assignment(std::shared_ptr<Variable> _var1, std::shared_ptr<Variable> _var2)
      : var1(std::move(_var1)), var2(std::move(_var2)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Var_Array_Init: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::vector<std::shared_ptr<T>> dims;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Var_Tuple_Init: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::shared_ptr<T> t;

      Instruction_Var_Tuple_Init(std::shared_ptr<Variable> _var, std::shared_ptr<T> _t) : var(std::move(_var)), t(std::move(_t)) {}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Call_Function: public Instruction{
    public:
      std::shared_ptr<Callee> callee;
      std::vector<std::shared_ptr<T>> args;
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Var_Function_Assignment: public Instruction{
    public:
      std::shared_ptr<Variable> var;
      std::unique_ptr<Instruction_Call_Function> function_call_instruction;

      Instruction_Var_Function_Assignment(std::shared_ptr<Variable> _var, std::unique_ptr<Instruction_Call_Function> _function_call_instruction) : var(std::move(_var)), 
        function_call_instruction(std::move(_function_call_instruction)) {}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  /*
  Block Enders
  */

  class Instruction_Return : public Instruction{
    public:
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Return_T: public Instruction{
    public:
      std::shared_ptr<T> t;

      Instruction_Return_T(std::shared_ptr<T> _t) : t(std::move(_t)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Br_Label: public Instruction{
    public:
      std::shared_ptr<Label> label;

      Instruction_Br_Label(std::shared_ptr<Label> _label) : label(std::move(_label)){}
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  class Instruction_Br_T_Label_Label: public Instruction{
    public:
      std::shared_ptr<Label> label1;
      std::shared_ptr<Label> label2;
      std::shared_ptr<T> t;

      Instruction_Br_T_Label_Label(std::shared_ptr<T> _t, std::shared_ptr<Label> _label1, std::shared_ptr<Label> _label2)
        : label1(std::move(_label1)), label2(std::move(_label2)), t(std::move(_t)){}
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream) const override;
  };

  /*
    Block.
  */
  class Block{
    public:
      std::shared_ptr<Label> label;
      std::vector<std::unique_ptr<Instruction>> instructions;
      std::unique_ptr<Instruction> end_instruction;
      std::unordered_set<std::string> successors;
      std::unordered_set<std::string> predecessors;

      std::string to_string() const;
  };

  /*
   * Function.
   */
  class Function{
    public:
      std::shared_ptr<FunctionName> function_name;
      std::shared_ptr<Type> return_type;
      std::vector<std::shared_ptr<TypeDef>> params;
      std::vector<std::unique_ptr<Block>> blocks;

      std::string to_string() const;
      void linearize();
      void generate_code(std::ofstream& stream) const;
  };

  class Program{
    public:
      std::vector<std::unique_ptr<Function>> functions;

      std::string to_string() const;
      
      void generate_code(std::ofstream& stream) {
        for (auto& f: functions){
          f->linearize();
          f->generate_code(stream);
        }
      }
  };

}
