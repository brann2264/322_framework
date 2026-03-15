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

namespace LA {

  class Program;
  class Function;

  inline std::string LA_LABEL = "AAAA";
  inline int LA_LABEL_COUNT = 0;

  inline std::string LA_VAR = "ASDASDSAD";
  inline int LA_NAME_COUNT = 0;

  inline std::string temp_var() {
    return "%" + LA_VAR + std::to_string(LA_NAME_COUNT++);
  }

  inline std::string temp_label(){
    return ":" + LA_LABEL + std::to_string(LA_LABEL_COUNT++);
  }

  enum class ItemType{Item, Name, Label, Number, Operator, Type};
  inline int64_t itemTypeToInt(ItemType type) {
    switch (type) {
        case ItemType::Item:           return 0;
        case ItemType::Name:           return 1;
        case ItemType::Label:          return 2;
        case ItemType::Number:         return 3;
        case ItemType::Operator:         return 4;
        case ItemType::Type:            return 5; 
        default:
            throw std::invalid_argument("Unknown ItemType");
    }
  }

  class Item {
    public:
      virtual ~Item() = default;
      virtual std::string to_string() const = 0;
      ItemType type = ItemType::Item;

      virtual void generate_code(std::ofstream& stream, Function& function_scope) const = 0;
  };

  struct T : virtual Item {
    public:
      virtual ~T() = default;
  };

  struct Name : T {
    std::string name;

    Name(std::string _name) : name(_name) {type = ItemType::Name;}

    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  struct Label : Item {
    std::string label_name;

    Label(std::string _label_name) : label_name(_label_name) {type = ItemType::Label;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  struct Number : T {
    int64_t number;

    Number(int64_t _number) : number(_number) {type = ItemType::Number;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  enum class EOperator {ADD,SUB,MULT,AND,LEFT_SHIFT,RIGHT_SHIFT,LT,LTE,EQ,GT,GTE};

  struct Operator : Item {
    EOperator op;

    Operator(EOperator _op) : op(_op) {type = ItemType::Operator;}
    
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  enum class EType {INT, TUPLE, CODE, ARRAY, VOID};

  struct Type : Item {
    EType name_type;
    int array_dims;

    Type() {type = ItemType::Type;}
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  struct TypeDef : Item {
    std::shared_ptr<Name> name;
    std::shared_ptr<Type> type;

    TypeDef(std::shared_ptr<Type> _type, std::shared_ptr<Name> _name) : type(std::move(_type)), name(std::move(_name)) {}
    std::string to_string() const override;
    void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  /*
   * Instruction interface.
   */
  class Instruction {
    public:
      virtual ~Instruction() = default;
      virtual std::string to_string() const = 0;
      virtual void generate_code(std::ofstream& stream, Function& function_scope) const {
        throw std::runtime_error("UNREACHABLE");
      };

      int line_number;
  };

  /*
   * Instructions.
   */

  class Instruction_Name_Def: public Instruction {
    public:
      std::shared_ptr<TypeDef> type_def;

      Instruction_Name_Def(std::shared_ptr<TypeDef> _type_def) : type_def(std::move(_type_def)){}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_T_Assignment: public Instruction{
    public:
      std::shared_ptr<Name> name;
      std::shared_ptr<T> t;

      Instruction_Name_T_Assignment(std::shared_ptr<Name> _name, std::shared_ptr<T> _t) : name(std::move(_name)), t(std::move(_t)){}
      
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_T_Op_T_Assignment: public Instruction {
    public:
      std::shared_ptr<Name> name;
      std::shared_ptr<T> t1;
      std::shared_ptr<Operator> op;
      std::shared_ptr<T> t2;

      Instruction_Name_T_Op_T_Assignment(std::shared_ptr<Name> _name, std::shared_ptr<T> _t1, 
        std::shared_ptr<Operator> _op, std::shared_ptr<T> _t2) : name(std::move(_name)), t1(std::move(_t1)), 
                                                                op(std::move(_op)), t2(std::move(_t2)){}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
      
  };

  class Instruction_Label: public Instruction {
    public:
      std::shared_ptr<Label> label;

      Instruction_Label(std::shared_ptr<Label> _label) : label(std::move(_label)) {}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Return : public Instruction{
    public:
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;

  };

  class Instruction_Return_T: public Instruction{
    public:
      std::shared_ptr<T> t;

      Instruction_Return_T(std::shared_ptr<T> _t) : t(std::move(_t)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Br_Label: public Instruction{
    public:
      std::shared_ptr<Label> label;

      Instruction_Br_Label(std::shared_ptr<Label> _label) : label(std::move(_label)){}
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Br_T_Label_Label: public Instruction{
    public:
      std::shared_ptr<Label> label1;
      std::shared_ptr<Label> label2;
      std::shared_ptr<T> t;

      Instruction_Br_T_Label_Label(std::shared_ptr<T> _t, std::shared_ptr<Label> _label1, std::shared_ptr<Label> _label2)
        : label1(std::move(_label1)), label2(std::move(_label2)), t(std::move(_t)){}
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_Array_Assignment: public Instruction{
    public:
      std::shared_ptr<Name> name;
      std::shared_ptr<Name> arr_name;
      std::vector<std::shared_ptr<T>> idxs;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Array_T_Assignment: public Instruction{
    public:
      std::shared_ptr<T> t;
      std::shared_ptr<Name> arr_name;
      std::vector<std::shared_ptr<T>> idxs;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_Length_Name_T_Assignment: public Instruction{
    public:
      std::shared_ptr<Name> name1;
      std::shared_ptr<Name> name2;
      std::shared_ptr<T> t;

      Instruction_Name_Length_Name_T_Assignment(std::shared_ptr<Name> _name1, std::shared_ptr<Name> _name2, std::shared_ptr<T> _t)
      : name1(std::move(_name1)), name2(std::move(_name2)), t(std::move(_t)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_Length_Name_Assignment: public Instruction{
    public:
      std::shared_ptr<Name> name1;
      std::shared_ptr<Name> name2;

      Instruction_Name_Length_Name_Assignment(std::shared_ptr<Name> _name1, std::shared_ptr<Name> _name2)
      : name1(std::move(_name1)), name2(std::move(_name2)){}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_Array_Init: public Instruction{
    public:
      std::shared_ptr<Name> name;
      std::vector<std::shared_ptr<T>> dims;

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_Tuple_Init: public Instruction{
    public:
      std::shared_ptr<Name> name;
      std::shared_ptr<T> t;

      Instruction_Name_Tuple_Init(std::shared_ptr<Name> _name, std::shared_ptr<T> _t) : name(std::move(_name)), t(std::move(_t)) {}
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Call_Function: public Instruction{
    public:
      std::shared_ptr<Name> name;
      std::vector<std::shared_ptr<T>> args;
    
      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  class Instruction_Name_Function_Assignment: public Instruction{
    public:
      std::shared_ptr<Name> name;
      std::unique_ptr<Instruction_Call_Function> function_call_instruction;

      Instruction_Name_Function_Assignment(std::shared_ptr<Name> _name, std::unique_ptr<Instruction_Call_Function> _function_call_instruction) : name(std::move(_name)), 
        function_call_instruction(std::move(_function_call_instruction)) {}

      std::string to_string() const override;
      void generate_code(std::ofstream& stream, Function& function_scope) const override;
  };

  /*
    Block.
  */
  class Block{
    public:
      std::shared_ptr<Label> label;
      
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
      std::shared_ptr<Name> function_name;
      std::shared_ptr<Type> return_type;
      std::vector<std::shared_ptr<TypeDef>> params;
      std::vector<std::unique_ptr<Instruction>> instructions;
      std::shared_ptr<std::unordered_set<std::string>> function_names;

      std::unordered_map<std::string, EType> name_types;
      int max_length_access = 0;
      bool encode = true;

      std::string to_string() const;
      void create_blocks();

      void generate_code(std::ofstream& stream);
  };

  class Program{
    public:
      std::vector<std::unique_ptr<Function>> functions;
      std::unordered_set<std::string> function_names;

      std::string to_string() const;

      void generate_code(std::ofstream& stream) {

        for (auto& f: functions){
          function_names.insert(f->function_name->name);
        }

        for (auto& f: functions){
          f->function_names = std::make_shared<std::unordered_set<std::string>>(function_names);
          f->create_blocks();
          f->generate_code(stream);
          stream << "\n";
        }
      }
  };

}
