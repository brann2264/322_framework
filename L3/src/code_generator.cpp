#include <string>
#include <iostream>
#include <fstream>

#include "code_generator.h"

namespace L3
{
  void generate_code(Program &p)
  {

    /*
     * Open the output file.
     */
    std::ofstream outputFile;
    outputFile.open("prog.L2");

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

  void Callee::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    if (name == "%")
      u->generate_code(stream, function_scope, global_scope);
    else
      stream << name;
  }

  void Name::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << name;
  }

  void Label::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << ":" + global_scope.label_to_global(label_name);
  }

  void FunctionName::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "@" << function_name;
  }

  void Number::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << std::to_string(number);
  }

  void Variable::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "%" + var_name;
  }

  void Comparator::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const
  {
    switch (cmp){
      case EComparator::GT:
      case EComparator::GTE:
        throw std::runtime_error("UNREACHABLE");
      case EComparator::LT:
        stream << "<";
        return;
      case EComparator::LTE:
        stream << "<=";
        return;
      case EComparator::EQ:
        stream << "=";
        return;
      default:
        throw std::runtime_error("UNREACHABLE");
    }
  }

  void Operator::generate_code(std::ofstream& stream, Function& function_scope, Program& global_scope) const
  {
    switch (op)
    {
    case EOperator::ADD: stream << "+"; return;
    case EOperator::SUB: stream << "-"; return;
    case EOperator::MULT: stream << "*"; return;
    case EOperator::AND: stream << "&"; return;
    case EOperator::LEFT_SHIFT: stream << "<<"; return;
    case EOperator::RIGHT_SHIFT: stream << ">>"; return;    
    default:
      throw std::runtime_error("UNREACHABLE");
    }
  }

  //

  void Instruction_Return::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "return";
  }

  void Instruction_Return_T::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "rax <- ";
    t->generate_code(stream, function_scope, global_scope);
    stream << "\nreturn";
  }

  void Instruction_Call_Function::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    std::string return_label = global_scope.generate_global_label() + "_ret";

    stream << "mem rsp -8 <- :" + return_label + "\n";

    for (int i = 0; i < args.size(); i++)
    {
      if (i < 6){
        stream << ARGUMENT_REGISTERS[i] + " <- ";
        args[i]->generate_code(stream, function_scope, global_scope);
        stream << "\n";
      } else {
        // -8 used by return label, so start at -16
        stream << "mem rsp -" << (i-6)*8 + 16 << " <- ";
        args[i]->generate_code(stream, function_scope, global_scope);
        stream << "\n";
      }
    }

    stream << "call ";
    callee->generate_code(stream, function_scope, global_scope);
    stream << std::to_string(args.size());

    stream << ":" + return_label;
  }

  void Instruction_Var_Function_Assignment::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    function_call_instruction->generate_code(stream, function_scope, global_scope);
    stream << "\n";
    var->generate_code(stream, function_scope, global_scope);
    stream << " <- rax";
  }

  void W_Assign_S_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " <- ";
    s->generate_code(stream, function_scope, global_scope);
  }
  void W_Assign_Mem_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " <- mem ";
    x->generate_code(stream, function_scope, global_scope);
    stream << " 0";
  }
  void Mem_Assign_S_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "mem ";
    x->generate_code(stream, function_scope, global_scope);
    stream << " 0 <- ";
    s->generate_code(stream, function_scope, global_scope);
  }
  void W_Aop_T_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " ";
    op->generate_code(stream, function_scope, global_scope);
    stream << "= ";
    t->generate_code(stream, function_scope, global_scope);
  }
  void W_Sop_Sx_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " ";
    sop->generate_code(stream, function_scope, global_scope);
    stream << "= ";
    sx->generate_code(stream, function_scope, global_scope);
  }
  void W_Sop_N_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " ";
    sop->generate_code(stream, function_scope, global_scope);
    stream << "= ";
    n->generate_code(stream, function_scope, global_scope);
  }
  void Mem_Increment_T_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "mem ";
    x->generate_code(stream, function_scope, global_scope);
    stream << " 0 += ";
    t->generate_code(stream, function_scope, global_scope);
  }
  void Mem_Decrement_T_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "mem ";
    x->generate_code(stream, function_scope, global_scope);
    stream << " 0 -= ";
    t->generate_code(stream, function_scope, global_scope);
  }
  void W_Increment_Mem_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " += mem ";
    x->generate_code(stream, function_scope, global_scope);
    stream << " 0";
  }
  void W_Decrement_Mem_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " -= mem ";
    x->generate_code(stream, function_scope, global_scope);
    stream << " 0";
  }
  void W_Assign_T_Cmp_T_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << " <- ";

    switch (std::dynamic_pointer_cast<Comparator>(cmp)->cmp)
    {
    case EComparator::GT:
      t2->generate_code(stream, function_scope, global_scope);
      stream << " < ";
      t1->generate_code(stream, function_scope, global_scope);
      return;
    case EComparator::GTE:
      t2->generate_code(stream, function_scope, global_scope);
      stream << " < ";
      t1->generate_code(stream, function_scope, global_scope);
      return;
    default:
      break;
    }

    t1->generate_code(stream, function_scope, global_scope);
    stream << " ";
    cmp->generate_code(stream, function_scope, global_scope);
    stream << " ";
    t2->generate_code(stream, function_scope, global_scope);
  }
  void Cjump_T_Label_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "cjump ";
    t->generate_code(stream, function_scope, global_scope);
    stream << " = 1";
    label->generate_code(stream, function_scope, global_scope);
  }
  void Goto_Label_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    stream << "goto ";
    label->generate_code(stream, function_scope, global_scope);
  }
  void Return_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    if (t != nullptr){
      stream << "rax <- ";
      t->generate_code(stream, function_scope, global_scope);
      stream << "\n";
    }
    stream << "return";
  }
  void W_Increment_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << "++";
  }
  void W_Decrement_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w->generate_code(stream, function_scope, global_scope);
    stream << "--";
  }
  void Address_Calculation_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    w1->generate_code(stream, function_scope, global_scope);
    stream << " @ ";
    w2->generate_code(stream, function_scope, global_scope);
    stream << " ";
    w3->generate_code(stream, function_scope, global_scope);
    stream << " ";
    E->generate_code(stream, function_scope, global_scope);
  }
  void W_Assign_T_Aop_T_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
    // w <- t1
    w->generate_code(stream, function_scope, global_scope);
    stream << " <- ";
    t1->generate_code(stream, function_scope, global_scope);
    stream << "\n";

    // w += t2
    w->generate_code(stream, function_scope, global_scope);
    stream << " ";
    aop->generate_code(stream, function_scope, global_scope);
    stream << "= ";
    t2->generate_code(stream, function_scope, global_scope);
  }

  void W_Assign_T_Sop_T_Tile::generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const
  {
      w->generate_code(stream, function_scope, global_scope);
      stream << " <- ";
      t1->generate_code(stream, function_scope, global_scope);
      stream << "\n";

      w->generate_code(stream, function_scope, global_scope);
      stream << " ";
      sop->generate_code(stream, function_scope, global_scope);
      stream << "= ";
      t2->generate_code(stream, function_scope, global_scope);
  }

  std::string Program::generate_global_label()
  {
    label_count += 1;
    return longest_label + "_global_" + std::to_string(label_count);
  }

  std::string Program::label_to_global(std::string label_name)
  {

    if (local_to_global_label_map.find(label_name) != local_to_global_label_map.end())
      return local_to_global_label_map.at(label_name);

    label_count += 1;
    std::string new_global_label_name = longest_label + "_global_" + std::to_string(label_count);

    local_to_global_label_map[label_name] = new_global_label_name;
    return new_global_label_name;
  }

  void Program::generate_code(std::ofstream &stream)
  {
    stream << "(@main\n";

    for (auto &f : functions)
    {
      f->generate_code(stream, *this);
    }

    stream << ")\n";
  }

  void Function::generate_code(std::ofstream &stream, Program &global_scope)
  {

    stream << "(" + function_name->to_string() + "\n" + std::to_string(vars.size()) + "\n";

    for (int i = 0; i < vars.size(); i++)
    {
      if (i < 6)
        stream << vars[i]->to_string() + " <- " + ARGUMENT_REGISTERS[i] + "\n";
      else 
        stream << vars[i]->to_string() << " <- stack-arg " << (i-6)*8 << "\n";
    }

    create_contexts();

    int i = 0;
    for (auto &context : contexts)
    {

      while (i < context->start_idx)
      {
        // labels or function calls in between contexts
        instructions[i]->generate_code(stream, *this, global_scope);
        i++;
      }

      context->generate_trees(instructions);

      for (auto &tree : context->trees)
      {
        tree->tile_tree();

        for (auto &tile : tree->tiles)
        {
          tile->generate_code(stream, *this, global_scope);
          stream << "\n";
        }
      }

      i = context->end_idx + 1;
    }

    stream << ")\n";
  }
}
