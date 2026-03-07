#include "L3.h"

namespace L3 {

  void pass() {
    throw std::runtime_error("Not implemented");
  }
  
  std::string Name::to_string() const {
    return name;
  }
  std::string Label::to_string() const {
    return ":" + label_name;
  }
  std::string FunctionName::to_string() const {
    return "@" + function_name;
  }
  std::string Number::to_string() const {
    return std::to_string(number);
  }
  std::string Variable::to_string() const {
    return "%" + var_name;
  }
  std::string Comparator::to_string() const {
    switch (cmp){
      case EComparator::EQ: return "=";
      case EComparator::LT: return "<";
      case EComparator::LTE: return "<=";
      case EComparator::GT: return ">";
      case EComparator::GTE: return ">=";
      default:
        throw std::runtime_error("Unrecognized cmp");
    }
  }
  std::string Operator::to_string() const {
    switch (op){
      case EOperator::ADD: return "+";
      case EOperator::SUB: return "-";
      case EOperator::MULT: return "*";
      case EOperator::AND: return "&";
      case EOperator::LEFT_SHIFT: return "<<";
      case EOperator::RIGHT_SHIFT: return ">>";
    }
  }

  std::string Callee::to_string() const {
    if (name == "%"){
      return u->to_string();
    }
    return name;
  }
  std::string Instruction_Var_S_Assignment::to_string() const {
    return var->to_string() + " <- " + s->to_string();
  }
  std::string Instruction_Var_T_Op_T_Assignment::to_string() const {
    return var->to_string() + " <- " + t1->to_string() + " " + op->to_string() + " " + t2->to_string();
  }
  std::string Instruction_Var_T_Cmp_T_Assignment::to_string() const {
    return var->to_string() + " <- " + t1->to_string() + " " + cmp->to_string() + " " + t2->to_string();
  }
  std::string Instruction_Var_Load_Var_Assignment::to_string() const {
    return var1->to_string() + " <- load " + var2->to_string(); 
  }
  std::string Instruction_Store_Var_S_Assignment::to_string() const {
    return "store " + var->to_string() + " <- " + s->to_string();
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
  std::string Instruction_Br_T_Label::to_string() const {
    return "br " + t->to_string() + " " + label->to_string();
  }
  std::string Instruction_Call_Function::to_string() const {
    std::string str_rep = "call " + callee->to_string() + " (";

    for (auto& arg: args){
      str_rep += arg->to_string() + ", ";
    }
    str_rep += ")";
    return str_rep;
  }
  std::string Instruction_Var_Function_Assignment::to_string() const {
    return var->to_string() + " <- " + function_call_instruction->to_string();
  }
  std::string Function::to_string() const {
    std::string str_rep = "define " + function_name->to_string() + "( ";

    for (auto& var : vars){
      str_rep += var->to_string() + ", ";
    }
    str_rep += ") {\n";
    for (auto& i: instructions){
      str_rep += i->to_string() + "\n";
    }
    str_rep += "}";
    return str_rep;
  }
  std::string Program::to_string() const {
    std::string str_rep = "";

    for (auto& f : functions){
      str_rep += f->to_string() + "\n";
    }
    return str_rep;
  }
}