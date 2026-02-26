#include "L2.h"

namespace L2 {

  void pass() {
    throw std::runtime_error("Not implemented");
  }
  
  void Name::generate_code(std::ofstream& stream, const Function& parent_function) const {
    stream << name;
  }
  std::string Name::to_string() const {
    return name;
  }

  void Label::generate_code(std::ofstream& stream, const Function& parent_function) const {
    stream << ":" << label_name;
  }
  std::string Label::to_string() const {
    return ":" + label_name;
  }

  void FunctionName::generate_code(std::ofstream& stream, const Function& parent_function) const {
    stream << "@" << function_name;
  }
  std::string FunctionName::to_string() const {
    return "@" + function_name;
  }

  void Number::generate_code(std::ofstream& stream, const Function& parent_function) const {
    stream << std::to_string(number);
  }
  std::string Number::to_string() const {
    return std::to_string(number);
  }

  void Variable::generate_code(std::ofstream& stream, const Function& parent_function) const {
    int color_id = parent_function.graph.at(var_name).color;

    if (color_id == -1) {
        throw std::runtime_error("CRITICAL ERROR: Tried to generate code for uncolored variable: %" + var_name);
    }

    std::string allocated_register = COLOR_TO_REG[color_id];

    stream << allocated_register;
  }
  std::string Variable::to_string() const {
    return "%" + var_name;
  }

  void Comparator::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    switch (cmp) {
      case EComparator::LT: stream << "<"; break;
      case EComparator::LTE: stream << "<="; break;
      case EComparator::EQ: stream << "="; break;
      default: throw std::runtime_error("Unkown comparator");
    }
  }
  std::string Comparator::to_string() const {
    switch (cmp) {
      case EComparator::LT: return "<";
      case EComparator::LTE: return "<=";
      case EComparator::EQ: return "=";
      default: throw std::runtime_error("Unkown comparator");
    }
  }

  void ShiftOp::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    if (shift_op == EShiftOperator::RIGHT) {
      stream << ">>=";
    } else {
      stream << "<<=";
    }
  }
  std::string ShiftOp::to_string() const {
    if (shift_op == EShiftOperator::RIGHT) return ">>=";
    return "<<=";
  }

  void AssignmentOp::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    switch (assign_op){
      case EAssignmentOperator::INCREMENT: stream << "+="; break;
      case EAssignmentOperator::DECREMENT: stream << "-="; break;
      case EAssignmentOperator::MULTIPLY: stream << "*="; break;
      case EAssignmentOperator::AND: stream << "&="; break;
      default: throw std::runtime_error("Unkown Assignment Operator");
    }
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

    
  void Register::generate_code(std::ofstream& stream, const Function& parent_function) const {
    switch (reg){
      case ERegister::r8:
        stream << "r8";
        break;
      case ERegister::r9:
        stream << "r9";
        break;
      case ERegister::rax:
        stream << "rax";
        break;
      case ERegister::rcx:
        stream << "rcx";
        break;
      case ERegister::rdi:
        stream << "rdi";
        break;
      case ERegister::rdx:
        stream << "rdx";
        break;
      case ERegister::rsi:
        stream << "rsi";
        break;
      case ERegister::rsp:
        stream << "rsp";
        break;
      default:
        throw std::runtime_error("Unknown Register");
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
        case ERegister::rsp: return "rsp";
        case ERegister::rcx: return "rcx";
        default:             throw std::runtime_error("Unknown register");
    }
  }

  MemoryAccess::MemoryAccess (std::unique_ptr<X> x, std::unique_ptr<Number> M) {
    this->x = std::move(x);
    offset = std::move(M);
    type = ItemType::MemoryAccess;
  }
  std::string MemoryAccess::to_string() const {
    return "mem " + x->to_string() + " " + offset->to_string();
  }
  void MemoryAccess::generate_code(std::ofstream& stream, const Function& parent_function) const {
    stream << "mem ";
    x->generate_code(stream, parent_function);
    stream << " ";
    offset->generate_code(stream, parent_function);
  }

  std::string Instruction_ret::to_string() const {
    return "return";
  }

  void Instruction_ret::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "return";
  }
  std::vector<int> Instruction_ret::get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const {
    return {};
  }

  Instruction_S_to_W_assignment::Instruction_S_to_W_assignment (std::unique_ptr<S> s, std::unique_ptr<X> w) {
    this->w = std::move(w);
    this->s = std::move(s);
  }
  std::string Instruction_S_to_W_assignment::to_string() const {
    return w->to_string() + " <- " + s->to_string();
  }
  void Instruction_S_to_W_assignment::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " <- ";
    s->generate_code(stream, parent_function);
  }

  Instruction_Mem_to_W_assignment::Instruction_Mem_to_W_assignment (std::unique_ptr<MemoryAccess> memory_access, std::unique_ptr<X> w) {
    this->w = std::move(w);
    this->memory_access = std::move(memory_access);
  }
  std::string Instruction_Mem_to_W_assignment::to_string() const {
    return w->to_string() + " <- " + memory_access->to_string();
  }
  void Instruction_Mem_to_W_assignment::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " <- ";
    memory_access->generate_code(stream, parent_function);
  }

  Instruction_S_to_Mem_assignment::Instruction_S_to_Mem_assignment (std::unique_ptr<S> s, std::unique_ptr<MemoryAccess> memory_access) {
    this->memory_access = std::move(memory_access);
    this->s = std::move(s);
  }
  std::string Instruction_S_to_Mem_assignment::to_string() const {
    return this->memory_access->to_string() + " <- " + this->s->to_string();
  }
  void Instruction_S_to_Mem_assignment::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    memory_access->generate_code(stream, parent_function);
    stream << " <- ";
    s->generate_code(stream, parent_function);
  }

  Instruction_Stack_Arg_M_to_W::Instruction_Stack_Arg_M_to_W (std::unique_ptr<Number> m, std::unique_ptr<X> w){
    this->m = std::move(m);
    this->w = std::move(w);
  }
  std::string Instruction_Stack_Arg_M_to_W::to_string() const {
    return w->to_string() + " <- stack-arg " + m->to_string(); 
  }
  void Instruction_Stack_Arg_M_to_W::generate_code(std::ofstream& stream, const Function& parent_function) const {
    w->generate_code(stream, parent_function);
    stream << " <- mem rsp " << m->number + (parent_function.locals * 8);
  }

  Instruction_W_aop_T::Instruction_W_aop_T (std::unique_ptr<X> w, std::unique_ptr<AssignmentOp> aop, std::unique_ptr<T> t){
    this->w = std::move(w);
    this->aop = std::move(aop);
    this->t = std::move(t);
  }
  std::string Instruction_W_aop_T::to_string() const {
    return w->to_string() + " " + aop->to_string() + " " + t->to_string();
  }
  void Instruction_W_aop_T::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " ";
    aop->generate_code(stream, parent_function);
    stream << " ";
    t->generate_code(stream, parent_function);
  }

  Instruction_W_sop_SX::Instruction_W_sop_SX (std::unique_ptr<X> w, std::unique_ptr<ShiftOp> sop, std::unique_ptr<X> sx) {
    this->w = std::move(w);
    this->sop = std::move(sop);
    this->sx = std::move(sx);
  }
  std::string Instruction_W_sop_SX::to_string() const {
    return w->to_string() + " " + sop->to_string() + " " + sx->to_string();
  }
  void Instruction_W_sop_SX::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " ";
    sop->generate_code(stream, parent_function);
    stream << " ";
    sx->generate_code(stream, parent_function);
  }

  Instruction_W_sop_N::Instruction_W_sop_N (std::unique_ptr<X> w, std::unique_ptr<ShiftOp> sop, std::unique_ptr<Number> number) {
    this->w = std::move(w);
    this->sop = std::move(sop);
    this->num = std::move(number);
  }
  std::string Instruction_W_sop_N::to_string() const {
    return w->to_string() + " " + sop->to_string() + " " + num->to_string();
  }
  void Instruction_W_sop_N::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " ";
    sop->generate_code(stream, parent_function);
    stream << " ";
    num->generate_code(stream, parent_function);
  }

  Instruction_Mem_increment_T::Instruction_Mem_increment_T(std::unique_ptr<MemoryAccess> mem, std::unique_ptr<T> t) {
    this->memory_access = std::move(mem);
    this->t = std::move(t);
  }
  std::string Instruction_Mem_increment_T::to_string() const {
    return memory_access->to_string() + " += " + this->t->to_string();
  }
  void Instruction_Mem_increment_T::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    memory_access->generate_code(stream, parent_function);
    stream << " += ";
    t->generate_code(stream, parent_function);
  }

  Instruction_Mem_decrement_T::Instruction_Mem_decrement_T(std::unique_ptr<MemoryAccess> mem, std::unique_ptr<T> t) {
    this->memory_access = std::move(mem);
    this->t = std::move(t);
  }
  std::string Instruction_Mem_decrement_T::to_string() const {
    return this->memory_access->to_string() + " -= " + this->t->to_string();
  }
  void Instruction_Mem_decrement_T::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    memory_access->generate_code(stream, parent_function);
    stream << " -= ";
    t->generate_code(stream, parent_function);
  }

  Instruction_W_increment_Mem::Instruction_W_increment_Mem(std::unique_ptr<X> w, std::unique_ptr<MemoryAccess> mem) {
    this->w = std::move(w);
    this->memory_access = std::move(mem);
  }
  std::string Instruction_W_increment_Mem::to_string() const {
    return this->w->to_string() + " += " + this->memory_access->to_string();
  }
  void Instruction_W_increment_Mem::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " += ";
    memory_access->generate_code(stream, parent_function);
  }

  Instruction_W_decrement_Mem::Instruction_W_decrement_Mem(std::unique_ptr<X> w, std::unique_ptr<MemoryAccess> mem) {
    this->w = std::move(w);
    this->memory_access = std::move(mem);
  }
  std::string Instruction_W_decrement_Mem::to_string() const {
    return this->w->to_string() + " -= " + this->memory_access->to_string();
  }
  void Instruction_W_decrement_Mem::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);;
    stream << " -= ";
    memory_access->generate_code(stream, parent_function);
  }

  std::string Instruction_label::to_string() const {
    return ":" + label_name;
  }
  void Instruction_label::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << ":" << label_name;
  }

  Instruction_function_call::Instruction_function_call(std::unique_ptr<U> call_target, int64_t arg_count) {
    this->call_target = std::move(call_target);
    this->arg_count = arg_count;
  }
  std::string Instruction_function_call::to_string() const {
    return "call " + this->call_target->to_string() + " " + std::to_string(arg_count);
  }
  void Instruction_function_call::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "call ";
    call_target->generate_code(stream, parent_function);
    stream << " " << arg_count;
  }

  std::string Instruction_print_call::to_string() const {
    return "call print 1";
  }
  void Instruction_print_call::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "call print 1";
  }

  Instruction_W_T_cmp_T::Instruction_W_T_cmp_T(std::unique_ptr<X> w, std::unique_ptr<T> operand1, std::unique_ptr<Comparator> cmp, std::unique_ptr<T> operand2){
    this->w = std::move(w);
    this->operand1 = std::move(operand1);
    this->cmp = std::move(cmp);
    this->operand2 = std::move(operand2);
  }
  std::string Instruction_W_T_cmp_T::to_string() const {
    return this->w->to_string() + " <- " + operand1->to_string() + " " + this->cmp->to_string() + " " + operand2->to_string();
  }
  void Instruction_W_T_cmp_T::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << " <- ";
    operand1->generate_code(stream, parent_function);
    stream << " ";
    cmp->generate_code(stream, parent_function);
    stream << " ";
    operand2->generate_code(stream, parent_function);
  }
    

  Instruction_cjump_T_cmp_T_label::Instruction_cjump_T_cmp_T_label(std::unique_ptr<T> operand1, std::unique_ptr<Comparator> cmp, std::unique_ptr<T> operand2, std::unique_ptr<Label> label){
    this->operand1 = std::move(operand1);
    this->cmp = std::move(cmp);
    this->operand2 = std::move(operand2);
    this->label = std::move(label);
  }
  std::string Instruction_cjump_T_cmp_T_label::to_string() const {
    return "cjump " + this->operand1->to_string() + " " + cmp->to_string() + " " + this->operand2->to_string() + " " + label->to_string();
  }
  void Instruction_cjump_T_cmp_T_label::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "cjump ";
    operand1->generate_code(stream, parent_function);
    stream << " ";
    cmp->generate_code(stream, parent_function);
    stream << " ";
    operand2->generate_code(stream, parent_function);
    stream << " ";
    label->generate_code(stream, parent_function);
  }
  std::vector<int> Instruction_cjump_T_cmp_T_label::get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const {
    auto it = label_mapping.find(this->label->label_name);
    if (it == label_mapping.end()) throw std::runtime_error("No such label " + label->label_name + " found.");

    return {it->second, instruction_idx+1};
  }


  Instruction_goto_label::Instruction_goto_label(std::unique_ptr<Label> label) {
    this->label = std::move(label);
  }
  std::string Instruction_goto_label::to_string() const {
    return "goto " + this->label->to_string();
  }
  void Instruction_goto_label::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "goto ";
    label->generate_code(stream, parent_function);
  }
  std::vector<int> Instruction_goto_label::get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const {
    auto it = label_mapping.find(this->label->label_name);
    if (it == label_mapping.end()) throw std::runtime_error("Error: Label " + this->label->label_name + " not found.");
    return {it->second};
  }


  std::string Instruction_input::to_string() const {
    return "call input 0";
  }
  void Instruction_input::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "call input 0";
  }

  std::string Instruction_allocate::to_string() const {
    return "call allocate 2";
  }
  void Instruction_allocate::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "call allocate 2";
  }

  std::string Instruction_tuple_error::to_string() const {
    return "call tuple-error 3";
  }
  void Instruction_tuple_error::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "call tuple-error 3";
  }
  std::vector<int> Instruction_tuple_error::get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const {
    return {};
  }

  Instruction_tensor_error::Instruction_tensor_error(std::unique_ptr<Number> F) {
    f = std::move(F);
  }
  std::string Instruction_tensor_error::to_string() const {
    return "call tensor-error " + f->to_string();
  }
  void Instruction_tensor_error::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    stream << "call tensor-error ";
    f->generate_code(stream, parent_function);
  }
  std::vector<int> Instruction_tensor_error::get_successor(const std::unordered_map<std::string, int>& label_mapping, int instruction_idx) const {
    return {};
  }

  Instruction_W_increment::Instruction_W_increment(std::unique_ptr<X> w) {
    this->w = std::move(w);
  }
  std::string Instruction_W_increment::to_string() const {
    return w->to_string() + "++";
  }
  void Instruction_W_increment::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << "++";
  }

    
  Instruction_W_decrement::Instruction_W_decrement(std::unique_ptr<X> w) {
    this->w = std::move(w);
  }
  std::string Instruction_W_decrement::to_string() const {
    return w->to_string() + "--";
  }
  void Instruction_W_decrement::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w->generate_code(stream, parent_function);
    stream << "--";
  }
    
  Instruction_address_calculation::Instruction_address_calculation(std::unique_ptr<X> w1, std::unique_ptr<X> w2, std::unique_ptr<X> w3, std::unique_ptr<Number> num) {
    this->w1 = std::move(w1);
    this->w2 = std::move(w2);
    this->w3 = std::move(w3);
    this->num = std::move(num);
  }
  std::string Instruction_address_calculation::to_string() const {
    return w1->to_string() + " @ " + w2->to_string() + " " + w3->to_string() + " " + num->to_string();
  }
  void Instruction_address_calculation::generate_code(std::ofstream& stream, const Function& parent_function) const   {
    w1->generate_code(stream, parent_function);
    stream << " @ ";
    w2->generate_code(stream, parent_function);
    stream << " ";
    w3->generate_code(stream, parent_function);
    stream << " ";
    num->generate_code(stream, parent_function);
  }

  std::string Function::to_string() const {
    std::string str_rep = "(@" + name + "\n\t" + std::to_string(arguments) + " " + std::to_string(locals) + "\n";
    for (std::unique_ptr<Instruction> const& i : instructions){
      str_rep += "\t" + i->to_string() + "\n";
    }

    str_rep += ")";

    return str_rep;
  }
  void Function::generate_code(std::ofstream& stream) const   {
    stream << "(@" << name << "\n\t" << arguments << " " << locals << "\n";
    for (std::unique_ptr<Instruction> const& i : instructions){
      stream << "\t";
      i->generate_code(stream, *this);
      stream << "\n";
    }
    stream << ")";
  }

  std::string Program::to_string() const {
    std::string str_rep = "(" + entryPointLabel + "\n";
    for (std::unique_ptr<Function> const& f : functions){
      str_rep += f->to_string() + "\n";
    }
    str_rep += ")";

    return str_rep;
  }
  void Program::generate_code(std::ofstream& stream) const   {
    stream << "(@" << entryPointLabel << "\n";

    for (std::unique_ptr<Function> const& f : functions){
      f->generate_code(stream);
      stream << "\n";
    }
    stream << ")";
  }
}

