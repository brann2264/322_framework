#include "LA.h"

namespace LA {
  
  void pass() {
    throw std::runtime_error("Not implemented");
  }
  
  std::string Name::to_string() const {
    return name;
  }
  std::string Label::to_string() const {
    return ":" + label_name;
  }
  std::string Number::to_string() const {
    return std::to_string(number);
  }
  std::string Operator::to_string() const {
    switch (op){
      case EOperator::ADD: return "+";
      case EOperator::SUB: return "-";
      case EOperator::MULT: return "*";
      case EOperator::AND: return "&";
      case EOperator::LEFT_SHIFT: return "<<";
      case EOperator::RIGHT_SHIFT: return ">>";
      case EOperator::EQ: return "=";
      case EOperator::LT: return "<";
      case EOperator::LTE: return "<=";
      case EOperator::GT: return ">";
      case EOperator::GTE: return ">=";
      default:
        throw std::runtime_error("");
    }
  }
  std::string Type::to_string() const {
    switch(name_type){
      case EType::INT: return "int64";
      case EType::TUPLE: return "tuple";
      case EType::CODE: return "code";
      case EType::ARRAY: {
        std::string rep = "int64";
        for (int i = 0; i < array_dims; i++){
          rep += "[]";
        }
        return rep;
      }
      case EType::VOID: return "void";
      default:
        throw std::runtime_error("");
    }
  }
  std::string TypeDef::to_string() const {
    return type->to_string() + " " + name->to_string();
  }
  std::string Instruction_Name_T_Assignment::to_string() const {
    return name->to_string() + " <- " + t->to_string();
  }
  std::string Instruction_Name_T_Op_T_Assignment::to_string() const {
    return name->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string();
  }
  std::string Instruction_Return::to_string() const {
    return "return";
  }
  std::string Instruction_Return_T::to_string() const {
    return "return " + t->to_string();
  }
  std::string Instruction_Label::to_string() const {
    return label->to_string();
  }
  std::string Instruction_Br_Label::to_string() const {
    return "br " + label->to_string();
  }
  std::string Instruction_Br_T_Label_Label::to_string() const {
    return "br " + t->to_string() + " " + label1->to_string() + " " + label2->to_string();
  }
  std::string Instruction_Call_Function::to_string() const {
    std::string str_rep = name->to_string() + " (";

    for (auto& arg: args){
      str_rep += arg->to_string() + ", ";
    }
    str_rep += ")";
    return str_rep;
  }
  std::string Instruction_Name_Function_Assignment::to_string() const {
    return name->to_string() + " <- " + function_call_instruction->to_string();
  }
  std::string Instruction_Name_Def::to_string() const {
    return type_def->to_string();
  }
  std::string Instruction_Name_Array_Assignment::to_string() const {
    std::string str_rep = name->to_string() + " <- " + arr_name->to_string();
    for (auto& idx : idxs){
      str_rep += "[" + idx->to_string() + "]";
    }
    return str_rep;
  }
  std::string Instruction_Array_T_Assignment::to_string() const {
    std::string str_rep = arr_name->to_string();
    for (auto& idx : idxs){
      str_rep += "[" + idx->to_string() + "]";
    }
    return str_rep + " <- " + t->to_string();
  }
  std::string Instruction_Name_Length_Name_T_Assignment::to_string() const {
    return name1->to_string() + " <- length " + name2->to_string() + " " + t->to_string();
  }
  std::string Instruction_Name_Length_Name_Assignment::to_string() const {
    return name1->to_string() + " <- length " + name2->to_string();
  }

  std::string Instruction_Name_Array_Init::to_string() const {
    std::string str_rep = name->to_string() + " <- new Array(";
    for (auto& dim : dims){
      str_rep += dim->to_string() + ",";
    }
    return str_rep + ")";
  }
  std::string Instruction_Name_Tuple_Init::to_string() const {
    return name->to_string() + " <- new Tuple(" + t->to_string() + ")";
  }

  std::string Function::to_string() const {
    std::string str_rep = "define " + return_type->to_string() + " " + function_name->to_string() + "( ";
    
    for (auto& param : params){
      str_rep += param->to_string() + ", ";
    }
    str_rep += ") {\n";
    
    for (auto& instruction: instructions){
      str_rep += instruction->to_string() + "\n";
      std::cout << str_rep << std::endl;
    }
    return str_rep + "}";
  }

  std::string Program::to_string() const {
    std::string str_rep = "";

    for (auto& f : functions){
      str_rep += f->to_string() + "\n";
    }
    return str_rep;
  }

  void Function::create_blocks() {

    std::vector<std::unique_ptr<Instruction>> blocked_instructions;
    std::vector<std::unique_ptr<Instruction>> type_def_instructions;

    bool startBB = true;

    for (int i = 0; i < instructions.size(); i++){

      Instruction_Label* label_inst = dynamic_cast<Instruction_Label*>(instructions[i].get());

      if (startBB){
        if (label_inst == nullptr){
          blocked_instructions.push_back(std::make_unique<Instruction_Label>(std::make_shared<Label>(temp_label())));
        }
        startBB = false;
      } else if (label_inst != nullptr){
        blocked_instructions.push_back(std::make_unique<Instruction_Br_Label>(label_inst->label));
      }

      // gather definitions to place in first bb
      Instruction_Name_Def* name_inst = dynamic_cast<Instruction_Name_Def*>(instructions[i].get()) ;
      if (name_inst != nullptr){
        type_def_instructions.push_back(std::move(instructions[i]));
        name_types[name_inst->type_def->name->to_string()] = name_inst->type_def->type->name_type;
        continue;
      }

      // bb terminates on labels, branches, and returns
      // label case already handled
      if (dynamic_cast<Instruction_Return*>(instructions[i].get()) ||
        dynamic_cast<Instruction_Return_T*>(instructions[i].get()) ||
        dynamic_cast<Instruction_Br_Label*>(instructions[i].get()) || 
        dynamic_cast<Instruction_Br_T_Label_Label*>(instructions[i].get())){
          startBB = true;
      }

      blocked_instructions.push_back(std::move(instructions[i]));
    }

    // last block not terminated, add return
    if (!startBB){
      if (return_type->name_type == EType::VOID){
        blocked_instructions.push_back(std::make_unique<Instruction_Return>());
      } else {
        blocked_instructions.push_back(std::make_unique<Instruction_Return_T>(std::make_shared<Number>(0)));
      }
    }

    std::vector<std::unique_ptr<Instruction>> newInstructions;

    newInstructions.push_back(std::move(blocked_instructions[0]));

    for (int i = 0; i < type_def_instructions.size(); i++){
      newInstructions.push_back(std::move(type_def_instructions[i]));
    }

    for (int i = 1; i < blocked_instructions.size(); i++){
      newInstructions.push_back(std::move(blocked_instructions[i]));
    }

    instructions = std::move(newInstructions);
  }

}