#include <string>
#include <iostream>
#include <fstream>

#include "code_generator.h"

namespace L3{
  void generate_code(Program& p){

    /* 
     * Open the output file.
     */ 
    std::ofstream outputFile;
    outputFile.open("prog.L2");
   
    /* 
     * Generate target code
     */ 
    //TODO
    p.generate_code(outputFile);
    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return ;
  }

  void Callee::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    if (name == "%")
      u->generate_code(stream, function_scope, global_scope);
    else
      stream << name;
  }

  void Name::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << name;
  }

  void Label::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << ":" + global_scope.label_to_global(label_name);
  }

  void FunctionName::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << "@" << function_name;
  }
  
  void Number::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << std::to_string(number);
  }

  void Variable::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << "%" + var_name;
  }

  //

  void Instruction_Return::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << "return";
  }

  void Instruction_Return_T::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    stream << "rax <- ";
    t->generate_code(stream, function_scope, global_scope);
    stream << "\nreturn";
  }

  void Instruction_Call_Function::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    std::string return_label = global_scope.generate_global_label() + "_ret";
    
    stream << "mem rsp -8 <- :" + return_label + "\n";

    for (int i = 0; i < args.size(); i++){
      if (i >= 6) throw std::runtime_error("Not Implemented: >6 Function Arguments");
      stream << ARGUMENT_REGISTERS[i] + " <- ";
      args[i]->generate_code(stream, function_scope, global_scope);
      stream << "\n";
    }

    stream << "call ";
    callee->generate_code(stream, function_scope, global_scope);
    stream << std::to_string(args.size());

    stream << ":" + return_label;
  }

  void Instruction_Var_Function_Assignment::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const {
    function_call_instruction->generate_code(stream, function_scope, global_scope);
    stream << "\n";
    var->generate_code(stream, function_scope, global_scope);
    stream << " <- rax";
  }

  std::string Program::generate_global_label(){
    label_count += 1;
    return longest_label + "_global_" + std::to_string(label_count);
  }

  std::string Program::label_to_global(std::string label_name) {

    if (local_to_global_label_map.find(label_name) != local_to_global_label_map.end())
      return local_to_global_label_map.at(label_name);
    
    label_count += 1;
    std::string new_global_label_name = longest_label + "_global_" + std::to_string(label_count);

    local_to_global_label_map[label_name] = new_global_label_name;
    return new_global_label_name;
  }

  void Program::generate_code(std::ofstream& stream) {
    stream << "(@main\n";

    for (auto& f : functions){
      f->generate_code(stream, *this);
    }

    stream << ")\n";
  }

  void Function::generate_code(std::ofstream& stream, Program& global_scope) {
    
    stream << "(" + function_name->to_string() + "\n" + std::to_string(vars.size()) + "\n"; 

    for (int i = 0; i < vars.size(); i++){
      if (i >= 6) throw std::runtime_error("Not Implemented: >6 Function Arguments");
      stream << vars[i]->to_string() + " <- " + ARGUMENT_REGISTERS[i] + "\n";
    }
    
    create_contexts();
    // need to include function calls and labels
    for (auto& context: contexts){
      context->generate_trees(instructions);
      
      for (auto& tree: context->trees){
        tree->tile_tree();

        for (auto& tile: tree->tiles){
          stream << tile->instruction_translation << "\n";
        }
      }
    }

    stream << ")\n";
  }
}
