#include <string>
#include <iostream>
#include <fstream>

#include "code_generator.h"

namespace LA
{

  

  void generate_code(Program &p)
  {

    /*
     * Open the output file.
     */
    std::ofstream outputFile;
    outputFile.open("prog.IR");

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

  void Name::generate_code(std::ofstream& stream, Function& function_scope) const
  { 
    if (function_scope.name_types.find(name) != function_scope.name_types.end()){
      stream << "%";
    } else if (function_scope.function_names->find(name) != function_scope.function_names->end()){
      stream << "@";
    }
 
    stream << name;
  }

  void Label::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << ":" + label_name;
  }

  void Number::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    if (function_scope.encode)
      stream << (number << 1) + 1;
    else
      stream << number;
  }

  void Operator::generate_code(std::ofstream& stream, Function& function_scope) const
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

  void Type::generate_code(std::ofstream& stream, Function& function_scope) const {

    switch(name_type){
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

  void TypeDef::generate_code(std::ofstream& stream, Function& function_scope) const {
    type->generate_code(stream, function_scope);
    stream << " ";
    name->generate_code(stream, function_scope);

    if (type->name_type == EType::TUPLE || type->name_type == EType::ARRAY){
      stream << "\n";
      name->generate_code(stream, function_scope);
      stream << " <- 0";
    } else if (type->name_type == EType::INT){
      stream << "\n";
      name->generate_code(stream, function_scope);
      stream << " <- 1";
    }
  }

  void Instruction_Name_Def::generate_code(std::ofstream& stream, Function& function_scope) const {
    type_def->generate_code(stream, function_scope);
  }

  void Instruction_Name_T_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    name->generate_code(stream, function_scope);
    stream << " <- ";
    t->generate_code(stream, function_scope);
  }
  void Instruction_Name_T_Op_T_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    
    stream << "%" << "DecodedVar0 <- ";
    t1->generate_code(stream, function_scope);
    stream << " >> 1\n";
    stream << "%" << "DecodedVar1 <- ";
    t2->generate_code(stream, function_scope);
    stream << " >> 1\n";
    
    name->generate_code(stream, function_scope);
    stream << " <- %" << "DecodedVar0 ";
    op->generate_code(stream, function_scope);
    stream << " %" << "DecodedVar1\n";

    name->generate_code(stream, function_scope);
    stream << " <- ";
    name->generate_code(stream, function_scope);
    stream << " << 1\n";
    name->generate_code(stream, function_scope);
    stream << " <- ";
    name->generate_code(stream, function_scope);
    stream << " + 1";
  }

  void Instruction_Label::generate_code(std::ofstream& stream, Function& function_scope) const {
    label->generate_code(stream, function_scope);
  }

  void Instruction_Name_Array_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    //allocation check
    stream << "%" << "LineNumber <- " << line_number << "\n";
    stream << "%" << "ErrorBool <- ";
    arr_name->generate_code(stream, function_scope);
    stream << " = 0\n";
    
    std::string error_label = temp_label();
    std::string correct_label = temp_label();
    stream << "br %" << "ErrorBool " + error_label + " " + correct_label << "\n";
    stream << error_label << "\n";
    if (function_scope.name_types[arr_name->to_string()] == EType::TUPLE)
      stream << "tuple-error(%" << "LineNumber)\n";
    else
      stream << "tensor-error(%" << "LineNumber)\n";
    stream << correct_label << "\n";

    // decode accesses (only generate when its vars?)
    for (int i = 0; i < idxs.size(); i++){
      stream << "%" << "DecodedVar" << i << " <- ";
      idxs[i]->generate_code(stream, function_scope);
      stream << " >> 1\n";
    }

    //access check
    for (int i = 0; i < idxs.size(); i++){
      stream << "%" << "ErrorBool <- %" << "DecodedVar" << i << " < 0\n";
      stream << "%" << "LengthVar <- length ";
      name->generate_code(stream, function_scope);
      // tuple calls without dim
      if (function_scope.name_types[arr_name->to_string()] == EType::ARRAY)
        stream << " " << i;
      stream << "\n";

      std::string error_access_label = temp_label();
      std::string correct_access_label = temp_label();

      stream << "br %" << "ErrorBool " + error_access_label + " " + correct_access_label << "\n";
      stream << error_access_label << "\n";

      if (function_scope.name_types[arr_name->to_string()] == EType::TUPLE)
        stream << "tuple-error(%" << "LineNumber)\n";
      else if (idxs.size() > 1)
        stream << "tensor-error(%" << "LineNumber, " << i << ", %" << "LengthVar, %" << "DecodedVar" << i << ")\n";
      else
        stream << "tensor-error(%" << "LineNumber, %" << "LengthVar, %" << "DecodedVar" << i << ")\n";
      stream << correct_access_label << "\n";

      stream << "%" << "ErrorBool <- %" << "DecodedVar" << i << " < %" << "LengthVar\n";

      std::string error_access_label2 = temp_label();
      std::string correct_access_label2 = temp_label();

      stream << "br %" << "ErrorBool " + error_access_label2 + " " + correct_access_label2 << "\n";
      stream << error_access_label2 << "\n";

      if (function_scope.name_types[arr_name->to_string()] == EType::TUPLE)
        stream << "tuple-error(%" << "LineNumber)\n";
      else if (idxs.size() > 1)
        stream << "tensor-error(%" << "LineNumber, " << i << ", %" << "LengthVar, %" << "DecodedVar" << i << ")\n";
      else
        stream << "tensor-error(%" << "LineNumber, %" << "LengthVar, %" << "DecodedVar" << i << ")\n";

      stream << correct_access_label2 << "\n";
    }


    name->generate_code(stream, function_scope);
    stream << " <- ";
    arr_name->generate_code(stream, function_scope);
    stream << "[";
    for (int i = 0; i < idxs.size(); i++){
      stream << "%" << "DecodedVar" << i;
      if (i != idxs.size()-1)
        stream << ", ";
    }
    stream << "]";
  }
  void Instruction_Array_T_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    // decode accesses (only generate when its vars?)
    for (int i = 0; i < idxs.size(); i++){
      stream << "%" << "DecodedVar" << i << " <- ";
      idxs[i]->generate_code(stream, function_scope);
      stream << " >> 1\n";
    }
    
    arr_name->generate_code(stream, function_scope);
    stream << "[";
    for (int i = 0; i < idxs.size(); i++){
      stream << "%" << "DecodedVar" << i;
      if (i != idxs.size()-1)
        stream << ", ";
    }
    stream << "] <- ";
    t->generate_code(stream, function_scope);
  }
  void Instruction_Name_Tuple_Init::generate_code(std::ofstream& stream, Function& function_scope) const {
    name->generate_code(stream, function_scope);
    stream << " <- new Tuple(";
    t->generate_code(stream, function_scope);
    stream << ")";
  }
  void Instruction_Name_Array_Init::generate_code(std::ofstream& stream, Function& function_scope) const {
    name->generate_code(stream, function_scope);
    stream << " <- new Array(";

    for (int i = 0; i < dims.size(); i++){
      dims[i]->generate_code(stream, function_scope);
      if (i != dims.size()-1)
        stream << ", ";
    }
    stream << ")";
  }
  void Instruction_Name_Length_Name_T_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    stream << "%" << "DecodedVar0 <- ";
    t->generate_code(stream, function_scope);
    stream << " >> 1\n";
    
    name1->generate_code(stream, function_scope);
    stream << " <- length ";
    name2->generate_code(stream, function_scope);
    stream << " %" << "DecodedVar0";
  }
  void Instruction_Name_Length_Name_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    name1->generate_code(stream, function_scope);
    stream << " <- length ";
    name2->generate_code(stream, function_scope);
  }
  void Instruction_Call_Function::generate_code(std::ofstream& stream, Function& function_scope) const {
    stream << "call ";
    name->generate_code(stream, function_scope);
    stream << "(";

    for (int i = 0; i < args.size(); i++){
      args[i]->generate_code(stream, function_scope);

      if (i != args.size()-1)
        stream << ", ";
    }
    stream << ")";
  }
  void Instruction_Name_Function_Assignment::generate_code(std::ofstream& stream, Function& function_scope) const {
    name->generate_code(stream, function_scope);
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
    t->generate_code(stream, function_scope);
  }
  void Instruction_Br_Label::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << "br ";
    label->generate_code(stream, function_scope);
  }
  void Instruction_Br_T_Label_Label::generate_code(std::ofstream& stream, Function& function_scope) const
  {
    stream << "%" << "DecodedVar0 <- ";
    t->generate_code(stream, function_scope);
    stream << " >> 1\n";

    stream << "br %" << "DecodedVar0 ";
    label1->generate_code(stream, function_scope);
    stream << " ";
    label2->generate_code(stream, function_scope);
  }

  void Function::generate_code(std::ofstream &stream)
  {
    stream << "define ";
    return_type->generate_code(stream, *this);
    stream << " ";
    function_name->generate_code(stream, *this);
    stream << " (";

    for (int i = 0; i < params.size(); i++){
      params[i]->generate_code(stream, *this);

      if (i != params.size()-1)
        stream << ", ";
    }
    stream << ") {\n";

    instructions[0]->generate_code(stream, *this);

    stream << "\nint64 %" << "LineNumber\n";
    stream << "int64 %" << "ErrorBool\n";
    stream << "int64 %" << "LengthVar\n";

    for (int i = 0; i <= max_length_access; i++){
      stream << "int64 %" << "DecodedVar" << i << "\n";
    }

    for (int i = 1; i < instructions.size(); i++){
      instructions[i]->generate_code(stream, *this);
      stream << "\n";
    }

    stream << "}";

  }
}
