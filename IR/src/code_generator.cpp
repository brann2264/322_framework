#include <string>
#include <iostream>
#include <fstream>

#include "code_generator.h"

namespace IR
{

  std::string IR_VAR = "ASDASDSAD";
  int ir_var_count = 0;

  std::string temp_var() {
    return "%" + IR_VAR + std::to_string(ir_var_count++);
  }

  void generate_code(Program &p)
  {

    /*
     * Open the output file.
     */
    std::ofstream outputFile;
    outputFile.open("prog.L3");

    /*
     * Generate target code
     */
    // TODO
    p.generate_code(outputFile);
    /*
     * Close the output file.
     */
    outputFile.close();

    return;
  }

  void Callee::generate_code(std::ofstream &stream) const
  {
    if (name == "%")
      u->generate_code(stream);
    else
      stream << name;
  }

  void Name::generate_code(std::ofstream &stream) const
  {
    stream << name;
  }

  void Label::generate_code(std::ofstream &stream) const
  {
    stream << ":" + label_name;
  }

  void FunctionName::generate_code(std::ofstream &stream) const
  {
    stream << "@" << function_name;
  }

  void Number::generate_code(std::ofstream &stream) const
  {
    stream << std::to_string(number);
  }

  void Variable::generate_code(std::ofstream &stream) const
  {
    stream << "%" + var_name;
  }

  void Operator::generate_code(std::ofstream& stream) const
  {
    switch (op)
    {
    case EOperator::ADD: stream << "+"; return;
    case EOperator::SUB: stream << "-"; return;
    case EOperator::MULT: stream << "*"; return;
    case EOperator::AND: stream << "&"; return;
    case EOperator::LEFT_SHIFT: stream << "<<"; return;
    case EOperator::RIGHT_SHIFT: stream << ">>"; return;    
    case EOperator::LT: stream << "<"; return;    
    case EOperator::LTE: stream << "<="; return;    
    case EOperator::EQ: stream << "="; return;    
    case EOperator::GT: stream << ">"; return;    
    case EOperator::GTE: stream << ">="; return;    
    default:
      throw std::runtime_error("UNREACHABLE");
    }
  }

  void Type::generate_code(std::ofstream& stream) const {

    switch(var_type){
      case EType::INT: stream << "int64"; return;
      case EType::TUPLE: stream << "tuple"; return;
      case EType::CODE: stream << "code"; return;
      case EType::ARRAY: 
        stream << "int64";
        for (int i = 0; i < array_dims; i++){
          stream << "[]";
        }
        return;
      case EType::VOID: stream << "void"; return;
      default:
        throw std::runtime_error("");
    }
  }

  void TypeDef::generate_code(std::ofstream& stream) const {
    var->generate_code(stream);
  }

  void Instruction_Var_Def::generate_code(std::ofstream& stream, Function& function_scope) const {
   // dont emit
    function_scope.var_types[type_def->var->to_string()] = type_def->type->var_type;
  }

  void Instruction_Var_S_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    var->generate_code(stream);
    stream << " <- ";
    s->generate_code(stream);
  }
  void Instruction_Var_T_Op_T_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    var->generate_code(stream);
    stream << " <- ";
    t1->generate_code(stream);
    stream << " ";
    op->generate_code(stream);
    stream << " ";
    t2->generate_code(stream);
  }
  void Instruction_Var_Array_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {

    if (function_scope.var_types[arr_var->to_string()] == EType::TUPLE){

      if (idxs.size() != 1) throw std::runtime_error("Tuple cant be multi-dimensional");

      std::string address = temp_var();
      stream << address << " <- 8 * ";
      idxs[0]->generate_code(stream);
      stream << "\n";

      stream << address << " <- " << address << " + 8\n"; 

      stream << address << " <- " << address << " + ";
      arr_var->generate_code(stream);
      stream << "\n";

      var->generate_code(stream);
      stream << " <- load " << address;
      return;
    }

    std::string base = temp_var();
    stream << base << " <- " << 8*(idxs.size() + 1) << "\n";

    std::string multiplier = temp_var();
    stream << multiplier << " <- 1\n";

    std::string bodyOffset = temp_var();
    stream << bodyOffset << " <- 0\n";
    

    for (int i = idxs.size()-1; i >= 0; i--){

      std::string dim_offset = temp_var();

      stream << dim_offset << " <- " << multiplier << " * ";
      idxs[i]->generate_code(stream);
      stream << "\n";

      stream << bodyOffset << " <- " << bodyOffset << " + " << dim_offset << "\n";

      std::string header_offset = temp_var();
      
      stream << header_offset << " <- " << 8*(i+1) << " + ";
      arr_var->generate_code(stream);
      stream << "\n";

      std::string dim_size = temp_var();
      stream << dim_size << " <- load " << header_offset << "\n";
      stream << dim_size << " <- " << dim_size << " >> 1\n";
      stream << multiplier << " <- " << multiplier << " * " << dim_size << "\n";
    }

    stream << bodyOffset << " <- " << bodyOffset << " * 8\n";

    std::string total_offset = temp_var();
    stream << total_offset << " <- " << bodyOffset << " + " << base << "\n";

    std::string address = temp_var();
    stream << address << " <- " << total_offset << " + ";
    arr_var->generate_code(stream);
    stream << "\n";

    var->generate_code(stream);
    stream << " <- load " << address << "\n";
  }
  void Instruction_Array_S_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    
    if (function_scope.var_types[arr_var->to_string()] == EType::TUPLE){

      if (idxs.size() != 1) throw std::runtime_error("Tuple cant be multi-dimensional");

      std::string address = temp_var();
      stream << address << " <- 8 * ";
      idxs[0]->generate_code(stream);
      stream << "\n";

      stream << address << " <- " << address << " + 8\n"; 

      stream << address << " <- " << address << " + ";
      arr_var->generate_code(stream);
      stream << "\n";

      stream << "store " << address << " <- ";
      s->generate_code(stream);
      return;
    }
    
    std::string base = temp_var();
    stream << base << " <- " << 8*(idxs.size() + 1) << "\n";

    std::string multiplier = temp_var();
    stream << multiplier << " <- 1\n";

    std::string bodyOffset = temp_var();
    stream << bodyOffset << " <- 0\n";
    

    for (int i = idxs.size()-1; i >= 0; i--){

      std::string dim_offset = temp_var();

      stream << dim_offset << " <- " << multiplier << " * ";
      idxs[i]->generate_code(stream);
      stream << "\n";

      stream << bodyOffset << " <- " << bodyOffset << " + " << dim_offset << "\n";

      std::string header_offset = temp_var();
      
      stream << header_offset << " <- " << 8*(i+1) << " + ";
      arr_var->generate_code(stream);
      stream << "\n";

      std::string dim_size = temp_var();
      stream << dim_size << " <- load " << header_offset << "\n";
      stream << dim_size << " <- " << dim_size << " >> 1\n";
      stream << multiplier << " <- " << multiplier << " * " << dim_size << "\n";
    }

    stream << bodyOffset << " <- " << bodyOffset << " * 8\n";

    std::string total_offset = temp_var();
    stream << total_offset << " <- " << bodyOffset << " + " << base << "\n";

    std::string address = temp_var();
    stream << address << " <- " << total_offset << " + ";
    arr_var->generate_code(stream);
    stream << "\n";

    stream << "store " << address << " <- ";
    s->generate_code(stream);
    stream << "\n";
  }
  void Instruction_Var_Length_Var_T_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    std::string offset = temp_var();
    
    stream << offset << " <- 8 * ";
    t->generate_code(stream);
    stream << "\n";

    stream << offset << " <- 8 + " << offset << "\n";

    std::string address = temp_var();
    stream << address << " <- ";
    var2->generate_code(stream);
    stream << " + " << offset << "\n";

    var1->generate_code(stream);
    stream << " <- load " << address;
  }
  void Instruction_Var_Length_Var_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    var1->generate_code(stream);
    stream << " <- load";
    var2->generate_code(stream);
    stream << "\n";

    var1->generate_code(stream);
    stream << " <- ";
    var1->generate_code(stream);
    stream << " << 1\n";

    var1->generate_code(stream);
    stream << " <- ";
    var1->generate_code(stream);
    stream << " + 1";
  }
  void Instruction_Var_Array_Init::generate_code(std::ofstream& stream, Function& function_scope) const {
    // shift args by one

    std::string allocate_arg = temp_var();

    std::string temp1 = temp_var();
    stream << temp1 << " <- ";
    dims[0]->generate_code(stream);
    stream << " >> 1\n";

    if (dims.size() == 1){
      stream << allocate_arg << " <- " << temp1 << " + " << dims.size() << "\n";
    } else {

      std::string temp2 = temp_var();
      stream << temp2 << " <- ";
      dims[1]->generate_code(stream);
      stream << " >> 1\n";

      stream << allocate_arg << " <- " << temp1 << " * " << temp2 << "\n";

      for (int i = 2; i < dims.size(); i++){
        std::string temp = temp_var();
        stream << temp << " <- ";
        dims[i]->generate_code(stream);
        stream << " >> 1\n";

        stream << allocate_arg << " <- " << allocate_arg << " * " << temp << "\n";
      }

      stream << allocate_arg << " <- " << allocate_arg << " + " << dims.size() << "\n";
    }

    stream << allocate_arg << " <- " << allocate_arg << " << 1\n";
    stream << allocate_arg << " <- " << allocate_arg << " + 1\n";

    var->generate_code(stream);
    stream << " <- call allocate(" << allocate_arg << ", 1)\n";
    
    // encode dimensions

    for (int i = 0; i < dims.size(); i++){
      std::string temp = temp_var();
      stream << temp << " <- ";
      var->generate_code(stream);
      stream << " + " << 8*(i+1) << "\n";

      stream << "store " << temp << " <- ";
      dims[i]->generate_code(stream);
      stream << "\n";
    }
  }
  void Instruction_Var_Tuple_Init::generate_code(std::ofstream& stream, Function& function_scope) const {
    var->generate_code(stream);
    stream << " <- call allocate(";
    t->generate_code(stream);
    stream << ", 1)";
  }
  void Instruction_Call_Function::generate_code(std::ofstream& stream, Function& function_scope) const {
    stream << "call ";
    callee->generate_code(stream);
    stream << "(";

    for (int i = 0; i < args.size(); i++){
      args[i]->generate_code(stream);

      if (i != args.size()-1)
        stream << ", ";
    }
    stream << ")";
  }
  void Instruction_Var_Function_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    var->generate_code(stream);
    stream << " <- ";
    function_call_instruction->generate_code(stream, function_scope);
  }

  void Instruction_Return::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << "return";
  }
  void Instruction_Return_T::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << "return ";
    t->generate_code(stream);
  }
  void Instruction_Br_Label::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << "br ";
    label->generate_code(stream);
  }
  void Instruction_Br_T_Label_Label::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << "br ";
    t->generate_code(stream);
    stream << " ";
    label1->generate_code(stream);
    stream << "\n";

    stream << "br ";
    label2->generate_code(stream);
  }

  void Function::generate_code(std::ofstream &stream)
  {
    stream << "define ";
    function_name->generate_code(stream);
    stream << " (";

    for (int i = 0; i < params.size(); i++){
      params[i]->generate_code(stream);
      var_types[params[i]->var->to_string()] = params[i]->type->var_type;

      if (i != params.size()-1)
        stream << ", ";
    }
    stream << ") {\n";

    for (auto& block : blocks){

      if (block->predecessors.size() > 0){
        block->label->generate_code(stream);
        stream << "\n";
      }
      
      for (auto& instruction : block->instructions){
        instruction->generate_code(stream, *this);
        stream << "\n";
      }

      Instruction_Br_Label* br_label = dynamic_cast<Instruction_Br_Label*>(block->end_instruction.get());
      Instruction_Br_T_Label_Label* br_t_label_label = dynamic_cast<Instruction_Br_T_Label_Label*>(block->end_instruction.get());

      if (br_label != nullptr){
        // generate br if label1 is not next
        if (block->successors.find(br_label->label->label_name) != block->successors.end()){
          br_label->generate_code(stream, *this);
          stream << "\n";
        }
          
      } else if (br_t_label_label != nullptr){

        if (block->successors.size() == 2){
          br_t_label_label->generate_code(stream, *this);
          stream << "\n";
        } else if (block->successors.size() == 1){
          // successor only gets deleted if it is label2
          stream << "br ";
          br_t_label_label->t->generate_code(stream);
          stream << " ";
          br_t_label_label->label1->generate_code(stream);   
          stream << "\n"; 
        }
      } else {
        // return instruction
        block->end_instruction->generate_code(stream, *this);
        stream << "\n";
      }
    }

    stream << "}";

  }
}
