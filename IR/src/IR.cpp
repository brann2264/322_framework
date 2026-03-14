#include "IR.h"

namespace IR {

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
    switch(var_type){
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
    return type->to_string() + " " + var->to_string();
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
  std::string Instruction_Return::to_string() const {
    return "return";
  }
  std::string Instruction_Return_T::to_string() const {
    return "return " + t->to_string();
  }
  std::string Instruction_Br_Label::to_string() const {
    return "br " + label->to_string();
  }
  std::string Instruction_Br_T_Label_Label::to_string() const {
    return "br " + t->to_string() + " " + label1->to_string() + " " + label2->to_string();
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
  std::string Instruction_Var_Def::to_string() const {
    return type_def->to_string();
  }
  std::string Instruction_Var_Array_Assignment::to_string() const {
    std::string str_rep = var->to_string() + " <- " + arr_var->to_string();
    for (auto& idx : idxs){
      str_rep += "[" + idx->to_string() + "]";
    }
    return str_rep;
  }
  std::string Instruction_Array_S_Assignment::to_string() const {
    std::string str_rep = arr_var->to_string();
    for (auto& idx : idxs){
      str_rep += "[" + idx->to_string() + "]";
    }
    return str_rep + " <- " + s->to_string();
  }
  std::string Instruction_Var_Length_Var_T_Assignment::to_string() const {
    return var1->to_string() + " <- length " + var2->to_string() + " " + t->to_string();
  }
  std::string Instruction_Var_Length_Var_Assignment::to_string() const {
    return var1->to_string() + " <- length " + var2->to_string();
  }

  std::string Instruction_Var_Array_Init::to_string() const {
    std::string str_rep = var->to_string() + " <- new Array(";
    for (auto& dim : dims){
      str_rep += dim->to_string() + ",";
    }
    return str_rep + ")";
  }
  std::string Instruction_Var_Tuple_Init::to_string() const {
    return var->to_string() + " <- new Tuple(" + t->to_string() + ")";
  }

  std::string Block::to_string() const {
    std::string str_rep = label->to_string() + "\n";

    for (auto& i: instructions){
      str_rep += i->to_string() + "\n";
    }
    str_rep += end_instruction->to_string();
    return str_rep;
  }

  std::string Function::to_string() const {
    std::string str_rep = "define " + return_type->to_string() + " " + function_name->to_string() + "( ";
    
    for (auto& param : params){
      str_rep += param->to_string() + ", ";
    }
    str_rep += ") {\n";
    
    for (auto& block: blocks){
      str_rep += block->to_string() + "\n";
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

  bool all_true(std::vector<bool>& vec){
    for (bool element : vec){
      if (!element) return false;
    }
    return true;
  }

  int first_block(std::vector<bool>& vec){
    for (int i = 0; i < vec.size(); i++){
      if (!vec[i]) return i;
    }
    return -1;
  }

  void Function::linearize() {

    // populate successors of blocks
    std::unordered_map<std::string, int> block_label_mapping;
    for (int i = 0; i < blocks.size(); i++){
      block_label_mapping.insert({blocks[i]->label->label_name, i});
    }

    for (auto& block : blocks){
      Instruction_Br_Label* br_label = dynamic_cast<Instruction_Br_Label*>(block->end_instruction.get());
      Instruction_Br_T_Label_Label* br_t_label_label = dynamic_cast<Instruction_Br_T_Label_Label*>(block->end_instruction.get());

      if (br_label != nullptr){        
        block->successors.insert(br_label->label->label_name);
        blocks[block_label_mapping[br_label->label->label_name]]->predecessors.insert(block->label->label_name);
      } else if (br_t_label_label != nullptr){
        block->successors.insert(br_t_label_label->label1->label_name);
        block->successors.insert(br_t_label_label->label2->label_name);
        blocks[block_label_mapping[br_t_label_label->label1->label_name]]->predecessors.insert(block->label->label_name);
        blocks[block_label_mapping[br_t_label_label->label2->label_name]]->predecessors.insert(block->label->label_name);
      }
    }

    // start tracing
    std::vector<bool> isTraced(blocks.size(), false);
    std::vector<std::vector<int>> traces;

    while (!all_true(isTraced)) {
      std::vector<int> trace;
      int block_id = first_block(isTraced);

      while (!isTraced[block_id]){
        isTraced[block_id] = true;
        trace.push_back(block_id);

        for (std::string successor_label : blocks[block_id]->successors){
          int successor_id = block_label_mapping[successor_label];
          if (!isTraced[successor_id]){
            block_id = successor_id;
            break;
          }
        }
      }
      traces.push_back(std::vector<int>(trace));
    }

    // put in linear order - entry block is guranteed to be in first trace since we start tracing from that
    
    std::vector<std::unique_ptr<Block>> linearized_blocks;

    for (auto& trace: traces){
      for (int block_id : trace){
        linearized_blocks.push_back(std::move(blocks[block_id]));
      }
    }

    blocks = std::move(linearized_blocks);

    for (int i = 0; i < blocks.size()-1; i++){
      // if last block, skip

      Instruction_Br_T_Label_Label* br_t_label_label = dynamic_cast<Instruction_Br_T_Label_Label*>(blocks[i]->end_instruction.get());
      Instruction_Br_Label* br_label = dynamic_cast<Instruction_Br_Label*>(blocks[i]->end_instruction.get());
        
      if (br_t_label_label != nullptr){
        // if first label is right after
        // br t :label1
        // br :label2
        // cant remove label1

        // if second label is right after
        // br t :label1
        // remove label2

        if (br_t_label_label->label2->label_name == blocks[i+1]->label->label_name){
          blocks[i]->successors.erase(br_t_label_label->label2->label_name);
          blocks[i+1]->predecessors.erase(blocks[i]->label->label_name);
        }

      } else if (br_label != nullptr){
        if (br_label->label->label_name == blocks[i+1]->label->label_name){
          blocks[i]->successors.erase(br_label->label->label_name);
          blocks[i+1]->predecessors.erase(blocks[i]->label->label_name);
        }
      }

    }
  }
}