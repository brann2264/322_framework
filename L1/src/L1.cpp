#include "L1.h"

namespace L1 {

// Register::Register (RegisterID r)
//   : ID {r}{
//   return ;
// }

Instruction_S_to_reg_assignment::Instruction_S_to_reg_assignment (std::unique_ptr<Register> dst, std::unique_ptr<S> src){
  this->d = std::move(dst);
  this->s = std::move(src);
}

Instruction_mem_to_reg_assignment::Instruction_mem_to_reg_assignment (std::unique_ptr<Register> dst, std::unique_ptr<MemoryAccess> src){
  this->d = std::move(dst);
  this->s = std::move(src);
}

Instruction_S_to_mem_assignment::Instruction_S_to_mem_assignment (std::unique_ptr<MemoryAccess> dst, std::unique_ptr<S> src){
  this->s = std::move(src);
  this->d = std::move(dst);
}

Instruction_aop::Instruction_aop (std::unique_ptr<Register> w, std::unique_ptr<AssignmentOp> aop, std::unique_ptr<T> t){
  this->w = std::move(w);
  this->aop = std::move(aop);
  this->t = std::move(t);
}

Instruction_sop_sx::Instruction_sop_sx (std::unique_ptr<Register> w, std::unique_ptr<ShiftOp> sop, std::unique_ptr<Register> sx){
  this->w = std::move(w);
  this->sop = std::move(sop);
  this->sx = std::move(sx);
}

Instruction_sop_n::Instruction_sop_n (std::unique_ptr<Register> w, std::unique_ptr<ShiftOp> sop, std::unique_ptr<Number> number){
  this->w = std::move(w);
  this->sop = std::move(sop);
  this->num = std::move(number);
}

Instruction_memory_access_increment_t::Instruction_memory_access_increment_t(std::unique_ptr<MemoryAccess> mem, std::unique_ptr<T> t){
  this->t = std::move(t);
  this->memory_access = std::move(mem);
}

Instruction_memory_access_decrement_t::Instruction_memory_access_decrement_t(std::unique_ptr<MemoryAccess> mem, std::unique_ptr<T> t){
  this->t = std::move(t);
  this->memory_access = std::move(mem);
}

Instruction_reg_mem_increment::Instruction_reg_mem_increment( std::unique_ptr<Register> reg, std::unique_ptr<MemoryAccess> mem){
  this->memory_access = std::move(mem);
  this->reg = std::move(reg);  
}

Instruction_reg_mem_decrement::Instruction_reg_mem_decrement(std::unique_ptr<Register> reg, std::unique_ptr<MemoryAccess> mem){
  this->memory_access = std::move(mem);
  this->reg = std::move(reg);  
}

MemoryAccess::MemoryAccess (std::unique_ptr<Register> x, std::unique_ptr<Number> M) {
  reg = std::move(x);
  offset = std::move(M);
  type = ItemType::MemoryAccess;
}

Instruction_function_call::Instruction_function_call (std::unique_ptr<U> call_target, int64_t arg_count){
  this->arg_count = arg_count;
  this->call_target = std::move(call_target);
}

Instruction_reg_cmp::Instruction_reg_cmp(std::unique_ptr<Register> reg, std::unique_ptr<T> operand1, std::unique_ptr<Comparator> cmp, std::unique_ptr<T> operand2){
  this->cmp = std::move(cmp);
  this->operand1 = std::move(operand1);
  this->operand2 = std::move(operand2);
  this->reg = std::move(reg);
}

Instruction_cjump::Instruction_cjump(std::unique_ptr<T> operand1, std::unique_ptr<Comparator> cmp, std::unique_ptr<T> operand2, std::unique_ptr<Label> label){
  this->cmp = std::move(cmp);
  this->operand1 = std::move(operand1);
  this->operand2 = std::move(operand2);
  this->label = std::move(label);
}

Instruction_goto_label::Instruction_goto_label(std::unique_ptr<Label> label){
  this->label = std::move(label);
}

Instruction_tensor_error::Instruction_tensor_error(std::unique_ptr<Number> F){
  this->f = std::move(F);
}

Instruction_increment::Instruction_increment(std::unique_ptr<Register> w){
  reg = std::move(w);
}
Instruction_decrement::Instruction_decrement(std::unique_ptr<Register> w){
  reg = std::move(w);
}
Instruction_address_calculation::Instruction_address_calculation(std::unique_ptr<Register> reg1, std::unique_ptr<Register> reg2, std::unique_ptr<Register> reg3, std::unique_ptr<Number> num){
  this->reg1 = std::move(reg1);
  this->reg2 = std::move(reg2);
  this->reg3 = std::move(reg3);
  this->num = std::move(num);
}

// to_string() implementations

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
std::string Comparator::to_string() const {
  switch (cmp) {
    case EComparator::LT: return "<";
    case EComparator::LTE: return "<=";
    case EComparator::EQ: return "=";
    default: throw std::runtime_error("Unkown comparator");
  }
}
std::string ShiftOp::to_string() const {
  if (shift_op == EShiftOperator::RIGHT) return ">>=";
  return "<<=";
}
std::string AssignmentOp::to_string() const {
  switch (assign_op){
    case EAssignmentOperator::INCREMENT: return "+=";
    case EAssignmentOperator::DECREMENT: return "-=";
    case EAssignmentOperator::MULTIPLY: return "*=";
    case EAssignmentOperator::AND: return "&=";
    default: throw std::runtime_error("Unkown Assignment Operator");
  }
}
std::string Register::to_string() const {
  switch (reg) {
      case ERegister::rdi: return "rdi";
      case ERegister::rsi: return "rsi";
      case ERegister::rdx: return "rdx";
      case ERegister::r8:  return "r8";
      case ERegister::r9:  return "r9";
      case ERegister::rax: return "rax";
      case ERegister::rbx: return "rbx";
      case ERegister::rbp: return "rbp";
      case ERegister::r10: return "r10";
      case ERegister::r11: return "r11";
      case ERegister::r12: return "r12";
      case ERegister::r13: return "r13";
      case ERegister::r14: return "r14";
      case ERegister::r15: return "r15";
      case ERegister::rsp: return "rsp";
      case ERegister::rcx: return "rcx";
      default:             throw std::runtime_error("Unknown register");
  }
}
std::string MemoryAccess::to_string() const {
  return "mem " + reg->to_string() + " " + offset->to_string();
}
std::string Instruction_ret::to_string() const {
  return "return";
}
std::string Instruction_S_to_reg_assignment::to_string() const {
  return d->to_string() + " <- " + s->to_string();
}
std::string Instruction_mem_to_reg_assignment::to_string() const {
  return d->to_string() + " <- " + s->to_string();
}
std::string Instruction_S_to_mem_assignment::to_string() const {
  return d->to_string() + " <- " + s->to_string();
}
std::string Instruction_aop::to_string() const{
  return w->to_string() + aop->to_string() + t->to_string();
}
std::string Instruction_sop_sx::to_string() const{
  return w->to_string() + sop->to_string() + sx->to_string();
}
std::string Instruction_sop_n::to_string() const{
  return w->to_string() + sop->to_string() + num->to_string();
}
std::string Instruction_memory_access_increment_t::to_string() const{
  return memory_access->to_string() + " += " + t->to_string();
}
std::string Instruction_memory_access_decrement_t::to_string() const{
  return memory_access->to_string() + " -= " + t->to_string();
}
std::string Instruction_reg_mem_increment::to_string() const{
  return reg->to_string() + " += " + memory_access->to_string();
}
std::string Instruction_reg_mem_decrement::to_string() const{
  return reg->to_string() + " -= " + memory_access->to_string();
}
std::string Instruction_label::to_string() const {
  return ":" + label_name;
}
std::string Instruction_function_call::to_string() const {
  return "call " + call_target->to_string() + " " + std::to_string(arg_count);
}
std::string Instruction_print_call::to_string() const {
  return "call print 1";
}
std::string Instruction_reg_cmp::to_string() const {
  return reg->to_string() + " <- " + operand1->to_string() + cmp->to_string() + operand2->to_string();
}

std::string Instruction_cjump::to_string() const {
  return "cjump " + operand1->to_string() + cmp->to_string() + operand2->to_string() + label->to_string();
}

std::string Function::to_string() const {
  std::string str_rep = name + "\n" + std::to_string(arguments) + " " + std::to_string(locals) + "\n";
  for (std::unique_ptr<Instruction> const& i : instructions){
    str_rep += i->to_string() + "\n";
  }

  str_rep += ")";

  return str_rep;
}

std::string Instruction_goto_label::to_string() const {
  return "goto " + label->to_string();
}
std::string Instruction_input::to_string() const {
  return "call input 0";
}
std::string Instruction_allocate::to_string() const {
  return "call allocate 2";
}
std::string Instruction_tuple_error::to_string() const {
  return "call tuple-error 3";
}
std::string Instruction_tensor_error::to_string() const {
  return "call tensor-error " + f->to_string();
}
std::string Instruction_increment::to_string() const {
  return reg->to_string() + "++";
}
std::string Instruction_decrement::to_string() const {
  return reg->to_string() + "--";
}
std::string Instruction_address_calculation::to_string() const {
  return reg1->to_string() + " @ " + reg2->to_string() + reg3->to_string() + num->to_string();
}

std::string Program::to_string() const {
  std::string str_rep = "(" + entryPointLabel + "\n";
  for (std::unique_ptr<Function> const& f : functions){
    str_rep += f->to_string() + "\n";
  }
  str_rep += ")";

  return str_rep;
}


//
// generate_code
//

void Name::generate_code(std::ofstream& stream) const {
  throw std::runtime_error("TODO");
}
void Label::generate_code(std::ofstream& stream) const {
  stream << "_" << label_name;
}
void FunctionName::generate_code(std::ofstream& stream) const {
  stream << "_" << function_name;
}
void Number::generate_code(std::ofstream& stream) const {
  stream << "$" << number;
}
void Comparator::generate_code(std::ofstream& stream) const {
  throw std::runtime_error("TODO");
}
void ShiftOp::generate_code(std::ofstream& stream) const {
  throw std::runtime_error("TODO");
}
void AssignmentOp::generate_code(std::ofstream& stream) const {
  throw std::runtime_error("TODO");
}
void Register::generate_code(std::ofstream& stream) const {
  switch (reg) {
      case ERegister::rdi: 
        stream << "%rdi";
        break;
      case ERegister::rsi: 
        stream << "%rsi";
        break;
      case ERegister::rdx: 
        stream << "%rdx";
        break;
      case ERegister::r8:  
        stream << "%r8";
        break;        
      case ERegister::r9:  
        stream << "%r9";
        break;        
      case ERegister::rax: 
        stream << "%rax";
        break;
      case ERegister::rbx: 
        stream << "%rbx";
        break;
      case ERegister::rbp: 
        stream << "%rbp";
        break;
      case ERegister::r10: 
        stream << "%r10";
        break;
      case ERegister::r11: 
        stream << "%r11";
        break;
      case ERegister::r12: 
        stream << "%r12";
        break;
      case ERegister::r13: 
        stream << "%r13";
        break;
      case ERegister::r14: 
        stream << "%r14";
        break;
      case ERegister::r15: 
        stream << "%r15";
        break;
      case ERegister::rsp: 
        stream << "%rsp";
        break;
      case ERegister::rcx: 
        stream << "%rcx";
        break;
      default:             
        throw std::runtime_error("Unknown register");
  }
}

void Register::generate_code_b(std::ofstream& stream) const {
  switch (reg) {
      case ERegister::rax: stream << "%al";   break;
      case ERegister::rbx: stream << "%bl";   break;
      case ERegister::rcx: stream << "%cl";   break;
      case ERegister::rdx: stream << "%dl";   break;
      case ERegister::rsi: stream << "%sil";  break;
      case ERegister::rdi: stream << "%dil";  break;
      case ERegister::rbp: stream << "%bpl";  break;
      case ERegister::rsp: stream << "%spl";  break;

      case ERegister::r8:  stream << "%r8b";  break;
      case ERegister::r9:  stream << "%r9b";  break;
      case ERegister::r10: stream << "%r10b"; break;
      case ERegister::r11: stream << "%r11b"; break;
      case ERegister::r12: stream << "%r12b"; break;
      case ERegister::r13: stream << "%r13b"; break;
      case ERegister::r14: stream << "%r14b"; break;
      case ERegister::r15: stream << "%r15b"; break;

      default:
        throw std::runtime_error("Unknown register for 8-bit conversion");
  }
}

void MemoryAccess::generate_code(std::ofstream& stream) const {
  stream << offset->number;
  stream << "(";
  reg->generate_code(stream);
  stream << ")";
}
void Instruction_ret::generate_code(std::ofstream& stream) const {

  if (std::max<int64_t>(0, arguments-6) + locals > 0){
    stream << "addq $" << (std::max<int64_t>(0, arguments-6) + locals)*8 << ", %rsp" << "\n\t";
  }
  stream << "retq";
}
void Instruction_S_to_reg_assignment::generate_code(std::ofstream& stream) const {
  stream << "movq ";

  if (s->type == ItemType::Label || s->type == ItemType::FunctionName){
    stream << "$";
  }
  s->generate_code(stream);
  stream << ", ";
  d->generate_code(stream);
}
void Instruction_mem_to_reg_assignment::generate_code(std::ofstream& stream) const {
  stream << "movq ";

  if (s->type == ItemType::Label || s->type == ItemType::FunctionName){
    stream << "$";
  }
  s->generate_code(stream);
  stream << ", ";
  d->generate_code(stream);
}
void Instruction_S_to_mem_assignment::generate_code(std::ofstream& stream) const {
  stream << "movq ";
  
  if (s->type == ItemType::Label || s->type == ItemType::FunctionName){
    stream << "$";
  }
  s->generate_code(stream);
  stream << ", ";
  d->generate_code(stream);
}

void Instruction_aop::generate_code(std::ofstream& stream) const {
  switch (aop->assign_op){
    case EAssignmentOperator::AND:
      stream << "andq ";
      break;
    case EAssignmentOperator::INCREMENT:
      stream << "addq ";
      break;
    case EAssignmentOperator::DECREMENT:
      stream << "subq ";
      break;
    case EAssignmentOperator::MULTIPLY:
      stream << "imulq ";
      break;
    default:
    ;
  }
  t->generate_code(stream);
  stream << ", ";
  w->generate_code(stream);
}
void Instruction_sop_sx::generate_code(std::ofstream& stream) const {
  if (sop->shift_op == EShiftOperator::LEFT){
    stream << "salq ";
  } else {
    stream << "sarq ";
  }

  sx->generate_code_b(stream);
  stream << ", ";
  w->generate_code(stream);
}
void Instruction_sop_n::generate_code(std::ofstream& stream) const {
  if (sop->shift_op == EShiftOperator::LEFT){
    stream << "salq ";
  } else {
    stream << "sarq ";
  }

  num->generate_code(stream);
  stream << ", ";
  w->generate_code(stream);
}
void Instruction_memory_access_increment_t::generate_code(std::ofstream& stream) const {
  stream << "addq ";
  t->generate_code(stream);
  stream << ", ";
  memory_access->generate_code(stream);
}
void Instruction_memory_access_decrement_t::generate_code(std::ofstream& stream) const {
  stream << "subq ";
  t->generate_code(stream);
  stream << ", ";
  memory_access->generate_code(stream);
}
void Instruction_reg_mem_increment::generate_code(std::ofstream& stream) const {
  stream << "addq ";
  memory_access->generate_code(stream);
  stream << ", ";
  reg->generate_code(stream);
}
void Instruction_reg_mem_decrement::generate_code(std::ofstream& stream) const {
  stream << "subq ";
  memory_access->generate_code(stream);
  stream << ", ";
  reg->generate_code(stream);
}
void Instruction_reg_cmp::generate_code(std::ofstream& stream) const {

  if (operand1->type == ItemType::Number && operand2->type == ItemType::Number){
    bool cmp_bool;
    int64_t constant1 = dynamic_cast<Number*>(operand1.get())->number;
    int64_t constant2 = dynamic_cast<Number*>(operand2.get())->number;

    switch (cmp->cmp){
      case EComparator::EQ:
        cmp_bool = constant1 == constant2;
        break;
      case EComparator::LT:
        cmp_bool = constant1 < constant2;
        break;
      case EComparator::LTE:
        cmp_bool = constant1 <= constant2;
        break;
      default:
      ;
    }
    stream << "movq $";

    if (cmp_bool){
      stream << "1, ";
    } else {
      stream << "0, ";
    }
    reg->generate_code(stream);
    return;
  }

  bool flip = operand1->type == ItemType::Number;

  stream << "cmpq ";
  if (flip){
    operand1->generate_code(stream);
    stream << ", ";
    operand2->generate_code(stream);
  } else {
    operand2->generate_code(stream);
    stream << ", ";
    operand1->generate_code(stream);
  }
  stream << "\n\t";

  switch (cmp->cmp){
    case EComparator::EQ: 
      stream << "sete ";
      break;
    case EComparator::LT:
      if (flip){
        stream << "setg ";
      } else {
        stream << "setl ";
      } 
      break;
    case EComparator::LTE:
      if (flip){
        stream << "setge ";
      } else {
        stream << "setle ";
      }
      break;
    default:
    ;
  }
  reg->generate_code_b(stream);
  stream << "\n\t";
  stream << "movzbq ";
  reg->generate_code_b(stream);
  stream << ", ";
  reg->generate_code(stream);
}
void Instruction_cjump::generate_code(std::ofstream& stream) const {

  if (operand1->type == ItemType::Number && operand2->type == ItemType::Number){
    bool jmp_bool;
    int64_t constant1 = dynamic_cast<Number*>(operand1.get())->number;
    int64_t constant2 = dynamic_cast<Number*>(operand2.get())->number;

    switch (cmp->cmp){
      case EComparator::EQ:
        jmp_bool = constant1 == constant2;
        break;
      case EComparator::LT:
        jmp_bool = constant1 < constant2;
        break;
      case EComparator::LTE:
        jmp_bool = constant1 <= constant2;
        break;
      default:
      ;
    }

    if (jmp_bool){
      stream << "jmp ";
      label->generate_code(stream);
    }

    return;
  }

  bool flip = operand1->type == ItemType::Number;

  stream << "cmpq ";
  if (flip){
    operand1->generate_code(stream);
    stream << ", ";
    operand2->generate_code(stream);
  } else {
    operand2->generate_code(stream);
    stream << ", ";
    operand1->generate_code(stream);
  }
  stream << "\n\t";

  switch (cmp->cmp){
    case EComparator::EQ:
      stream << "je ";
      break;
    case EComparator::LT:
      if (flip) {
        stream << "jg ";
      } else {
        stream << "jl ";
      }
      break;
    case EComparator::LTE:
      if (flip) {
        stream << "jge ";
      } else {
        stream << "jle ";
      }
      break;
    default:
    ;
  }

  label->generate_code(stream);
}
void Instruction_input::generate_code(std::ofstream& stream) const {
  stream << "call input";
}
void Instruction_allocate::generate_code(std::ofstream& stream) const {
  stream << "call allocate";
}
void Instruction_tuple_error::generate_code(std::ofstream& stream) const {
  stream << "call tuple_error";
}
void Instruction_tensor_error::generate_code(std::ofstream& stream) const {
  switch (f->number){
    case 1:
      stream << "call array_tensor_error_null";
      return;
    case 3:
      stream << "call array_error";
      return;
    case 4:
      stream << "call tensor_error";
      return;
    default:
      throw std::runtime_error("Syntax Error: Invalid F for Tensor Error");
  }
}
void Instruction_goto_label::generate_code(std::ofstream& stream) const {
  stream << "jmp ";
  label->generate_code(stream);
}
void Instruction_increment::generate_code(std::ofstream& stream) const {
  stream << "inc ";
  reg->generate_code(stream);
}
void Instruction_decrement::generate_code(std::ofstream& stream) const {
  stream << "dec ";
  reg->generate_code(stream);
}
void Instruction_address_calculation::generate_code(std::ofstream& stream) const {
  stream << "lea (";
  reg2->generate_code(stream);
  stream <<", ";
  reg3->generate_code(stream);
  stream << ", " << num->number << "), ";
  reg1->generate_code(stream);
}

void Instruction_label::generate_code(std::ofstream& stream) const {
  stream << "_" << label_name << ":";
}
void Instruction_function_call::generate_code(std::ofstream& stream) const {
  
  stream << "subq $" << std::max<int64_t>(arg_count-6, 0)*8 + 8 << ", %rsp";
  stream << "\n\t";
  stream << "jmp ";

  if (call_target->type == ItemType::Register) stream << "*";
  call_target->generate_code(stream);
}
void Instruction_print_call::generate_code(std::ofstream& stream) const {
  stream << "call print";
}
void Function::generate_code(std::ofstream& stream) const {
  stream << "_" << name << ":";

  // if (arguments > 6) stream << "\n\t" << "subq $" << 8*(arguments-6) << ", %rsp";
  if (locals > 0) stream << "\n\t" << "subq $" << 8*locals << ", %rsp";
  
  for (auto& i: instructions){
    stream << "\n\t";
    i->generate_code(stream);
  }
}
void Program::generate_code(std::ofstream& stream) const {
  stream << ".text\n";
  stream << "\t.globl go\n";
  stream << "go:\n";
  stream << "\tpushq %rbx\n";
  stream << "\tpushq %rbp\n";
  stream << "\tpushq %r12\n";
  stream << "\tpushq %r13\n";
  stream << "\tpushq %r14\n";
  stream << "\tpushq %r15\n";
  stream << "\tcall _" << entryPointLabel << "\n";
  stream << "\tpopq %r15\n";
  stream << "\tpopq %r14\n";
  stream << "\tpopq %r13\n";
  stream << "\tpopq %r12\n";
  stream << "\tpopq %rbp\n";
  stream << "\tpopq %rbx\n";
  stream << "\tretq";

  for (auto& f: functions){
    stream << "\n";
    f->generate_code(stream);
  }
  stream << "\n";
}

}
