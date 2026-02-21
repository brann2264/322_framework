/*
 * SUGGESTION FROM THE CC TEAM:
 * double check the order of actions that are fired.
 * You can do this in (at least) two ways:
 * 1) by using gdb adding breakpoints to actions
 * 2) by adding printing statements in each action
 *
 * For 2), we suggest writing the code to make it straightforward to enable/disable all of them 
 * (e.g., assuming shouldIPrint is a global variable
 *    if (shouldIPrint) std::cerr << "MY OUTPUT" << std::endl;
 * )
 */
#include <sched.h>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <assert.h>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>
#include <tao/pegtl/contrib/raw_string.hpp>

#include <L1.h>
#include <parser.h>

namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace L1 {

  /* 
   * Tokens parsed
   */ 
  std::vector<std::unique_ptr<Item>> parsed_items;

  /* 
   * Grammar rules from now on.
   */
  struct name:
    pegtl::seq<
      pegtl::plus< 
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >
        >
      >,
      pegtl::star<
        pegtl::sor<
          pegtl::alpha,
          pegtl::one< '_' >,
          pegtl::digit
        >
      >
    > {};

  struct aop_rule : 
  pegtl::sor<
      TAO_PEGTL_STRING("+="),
      TAO_PEGTL_STRING("-="),
      TAO_PEGTL_STRING("*="),
      TAO_PEGTL_STRING("&=")
  > {};
  
  struct sop_rule : 
  pegtl::sor<
    TAO_PEGTL_STRING("<<="),
    TAO_PEGTL_STRING(">>=")
  > {};

  struct cmp_rule : 
  pegtl::sor<
      TAO_PEGTL_STRING("<="), 
      pegtl::one<'<'>,
      pegtl::one<'='>
  > {};

  /* 
   * Keywords.
   */
  struct str_return : TAO_PEGTL_STRING( "return" ) {};
  struct str_arrow : TAO_PEGTL_STRING( "<-" ) {};
  struct str_call : TAO_PEGTL_STRING( "call") {};
  struct str_mem : TAO_PEGTL_STRING("mem") {};

  struct str_rdi : TAO_PEGTL_STRING( "rdi" ) {};
  struct str_rax : TAO_PEGTL_STRING( "rax" ) {};
  struct str_rbx : TAO_PEGTL_STRING( "rbx" ) {};
  struct str_rcx : TAO_PEGTL_STRING( "rcx" ) {};
  struct str_rsi : TAO_PEGTL_STRING( "rsi" ) {};
  struct str_rdx : TAO_PEGTL_STRING( "rdx" ) {};
  struct str_rbp : TAO_PEGTL_STRING( "rbp" ) {};
  struct str_r10 : TAO_PEGTL_STRING( "r10" ) {};
  struct str_r11 : TAO_PEGTL_STRING( "r11" ) {};
  struct str_r12 : TAO_PEGTL_STRING( "r12" ) {};
  struct str_r13 : TAO_PEGTL_STRING( "r13" ) {};
  struct str_r14 : TAO_PEGTL_STRING( "r14" ) {};
  struct str_r15 : TAO_PEGTL_STRING( "r15" ) {};
  struct str_r8 : TAO_PEGTL_STRING( "r8" ) {};
  struct str_r9 : TAO_PEGTL_STRING( "r9" ) {};
  struct str_rsp : TAO_PEGTL_STRING( "rsp" ) {};

  struct register_rdi_rule : str_rdi {};
  struct register_rax_rule : str_rax {};
  struct register_rbx_rule : str_rbx {};
  struct register_rcx_rule : str_rcx {};
  struct register_rsi_rule : str_rsi {};
  struct register_rdx_rule : str_rdx {};
  struct register_rbp_rule : str_rbp {};
  struct register_r10_rule : str_r10 {};
  struct register_r11_rule : str_r11 {};
  struct register_r12_rule : str_r12 {};
  struct register_r13_rule : str_r13 {};
  struct register_r14_rule : str_r14 {};
  struct register_r15_rule : str_r15 {};
  struct register_r8_rule : str_r8  {};
  struct register_r9_rule : str_r9  {};
  struct register_rsp_rule : str_rsp {};
  
  struct argument_register_rule:
    pegtl::sor<
      register_rdi_rule,
      register_rsi_rule,
      register_rdx_rule,
      register_r8_rule,
      register_r9_rule,
      register_rcx_rule
    > {};
  
  struct general_register_rule:
    pegtl::sor<
      argument_register_rule,
      register_rax_rule,
      register_rbx_rule,
      register_rbp_rule,
      register_r10_rule,
      register_r11_rule,
      register_r12_rule,
      register_r13_rule,
      register_r14_rule,
      register_r15_rule
    > {};

  struct register_rule:
    pegtl::sor<
      general_register_rule,
      register_rsp_rule
    > {};

  struct label:
    pegtl::seq<
      pegtl::one<':'>,
      name
    > {};

  struct function_name_rule:
    pegtl::seq<
      pegtl::one<'@'>,
      name
    > {};
   
  struct number:
    pegtl::seq<
      pegtl::opt<
        pegtl::sor<
          pegtl::one< '-' >,
          pegtl::one< '+' >
        >
      >,
      pegtl::plus< 
        pegtl::digit
      >
    >{};

  struct t_rule:
    pegtl::sor<
      number,
      register_rule
    > {};
  
  struct u_rule:
    pegtl::sor<
      general_register_rule,
      function_name_rule
    > {};
  
  struct s_rule:
    pegtl::sor<
      t_rule,
      label,
      function_name_rule
    > {};
  
  // struct m_number:
  //   number {};
  
  // struct m_number:
  //   number {};
  
  // struct f_number:
  //   number {};

  struct argument_number:
    number {};

  struct local_number:
    number {} ;

  struct comment: 
    pegtl::disable< 
      TAO_PEGTL_STRING( "//" ), 
      pegtl::until< pegtl::eolf > 
    > {};

  /*
   * Separators.
   */
  struct spaces :
    pegtl::star< 
      pegtl::sor<
        pegtl::one< ' ' >,
        pegtl::one< '\t'>
      >
    > { };

  struct seps : 
    pegtl::star<
      pegtl::seq<
        spaces,
        pegtl::eol
      >
    > { };
  struct seps_with_comments : 
    pegtl::star< 
      pegtl::seq<
        spaces,
        pegtl::sor<
          pegtl::eol,
          comment
        >
      >
    > { };

  // instructions

  struct memory_access_rule:
    pegtl::seq<
      str_mem,
      spaces,
      register_rule,
      spaces,
      number
    > {};

  struct Instruction_return_rule:
    pegtl::seq<
      str_return
    > { };

  struct Instruction_S_to_reg_assignment_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      str_arrow,
      spaces,
      s_rule
    > {};
  
  struct Instruction_mem_to_reg_assignment_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      str_arrow,
      spaces,
      memory_access_rule
    > {};
  
  struct Instruction_S_to_mem_assignment_rule:
    pegtl::seq<
      memory_access_rule,
      spaces,
      str_arrow,
      spaces,
      s_rule
    > {};
  
  struct Instruction_aop_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      aop_rule,
      spaces,
      t_rule
    > {};
  
  struct Instruction_sop_sx_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      sop_rule,
      spaces,
      register_rcx_rule
    > {};
  
    struct Instruction_sop_n_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      sop_rule,
      spaces,
      number
    > {};
  
  struct Instruction_memory_access_increment_t_rule:
    pegtl::seq<
      memory_access_rule,
      spaces,
      pegtl::string<'+', '='>,
      spaces,
      t_rule
    > {};
  struct Instruction_memory_access_decrement_t_rule:
    pegtl::seq<
      memory_access_rule,
      spaces,
      pegtl::string<'-', '='>,
      spaces,
      t_rule
    > {};
  
  struct Instruction_reg_increment_mem_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      pegtl::string<'+', '='>,
      spaces,
      memory_access_rule
    > {};

  struct Instruction_reg_decrement_mem_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      pegtl::string<'-', '='>,
      spaces,
      memory_access_rule
    > {};
  
  struct Instruction_reg_cmp_rule:
    pegtl::seq<
      general_register_rule,
      spaces,
      str_arrow,
      spaces,
      t_rule,
      spaces,
      cmp_rule,
      spaces,
      t_rule
    > {};
  
  struct Instruction_cjump_rule:
    pegtl::seq<
      TAO_PEGTL_STRING( "cjump" ),
      spaces,
      t_rule,
      spaces,
      cmp_rule,
      spaces,
      t_rule,
      spaces,
      label
    > {};
  
  struct Instruction_label_rule:
    label {};
  
  struct Instruction_goto_label_rule:
    pegtl::seq<
      TAO_PEGTL_STRING( "goto" ),
      spaces,
      label
    >{};

  struct Instruction_function_call_rule:
    pegtl::seq<
      str_call,
      spaces,
      u_rule,
      spaces,
      number
      > {};
  
  struct Instruction_print_rule:
    pegtl::seq<
      str_call,
      spaces,
      TAO_PEGTL_STRING( "print" ),
      spaces,
      pegtl::one<'1'>
    > {};
  
  struct Instruction_input_rule:
    pegtl::seq<
      str_call,
      spaces,
      TAO_PEGTL_STRING( "input" ),
      spaces,
      pegtl::one<'0'>
    > {};
  
  struct Instruction_allocate_rule:
    pegtl::seq<
      str_call,
      spaces,
      TAO_PEGTL_STRING( "allocate" ),
      spaces,
      pegtl::one<'2'>
    > {};

  struct Instruction_tuple_error_rule:
    pegtl::seq<
      str_call,
      spaces,
      TAO_PEGTL_STRING( "tuple-error" ),
      spaces,
      pegtl::one<'3'>
    > {};
  
  struct Instruction_tensor_error_rule:
  pegtl::seq<
      str_call,
      spaces,
      TAO_PEGTL_STRING( "tensor-error" ),
      spaces,
      number
    > {};
  
  struct Instruction_increment_rule:
  pegtl::seq<
    general_register_rule,
    spaces,
    pegtl::two<'+'>
  > {};

  struct Instruction_decrement_rule:
  pegtl::seq<
    general_register_rule,
    spaces,
    pegtl::two<'-'>
  > {};
  
  struct Instruction_address_calculation_rule:
  pegtl::seq<
    general_register_rule,
    spaces,
    pegtl::one<'@'>,
    spaces,
    general_register_rule,
    spaces,
    general_register_rule,
    spaces,
    number
  > {};

  struct Instruction_rule:
    pegtl::sor<
      // 1. High Priority: Unique Keywords & Labels
      pegtl::seq<pegtl::at<Instruction_return_rule>,        Instruction_return_rule>,
      pegtl::seq<pegtl::at<Instruction_cjump_rule>,         Instruction_cjump_rule>,
      pegtl::seq<pegtl::at<Instruction_goto_label_rule>,    Instruction_goto_label_rule>,
      pegtl::seq<pegtl::at<Instruction_label_rule>,         Instruction_label_rule>,

      // 2. Specialized Function Calls
      pegtl::seq<pegtl::at<Instruction_print_rule>,         Instruction_print_rule>,
      pegtl::seq<pegtl::at<Instruction_input_rule>,         Instruction_input_rule>,
      pegtl::seq<pegtl::at<Instruction_allocate_rule>,      Instruction_allocate_rule>,
      pegtl::seq<pegtl::at<Instruction_tuple_error_rule>,   Instruction_tuple_error_rule>,
      pegtl::seq<pegtl::at<Instruction_tensor_error_rule>,  Instruction_tensor_error_rule>,
      pegtl::seq<pegtl::at<Instruction_function_call_rule>, Instruction_function_call_rule>,

      // 3. Complex Operations (Unambiguous prefixes)
      pegtl::seq<pegtl::at<Instruction_address_calculation_rule>,      Instruction_address_calculation_rule>,
      pegtl::seq<pegtl::at<Instruction_memory_access_increment_t_rule>, Instruction_memory_access_increment_t_rule>,
      pegtl::seq<pegtl::at<Instruction_memory_access_decrement_t_rule>, Instruction_memory_access_decrement_t_rule>,
      
      // 4. Arithmetic / Shift / Ops
      pegtl::seq<pegtl::at<Instruction_reg_increment_mem_rule>,  Instruction_reg_increment_mem_rule>,
      pegtl::seq<pegtl::at<Instruction_reg_decrement_mem_rule>,  Instruction_reg_decrement_mem_rule>,
      pegtl::seq<pegtl::at<Instruction_increment_rule>,          Instruction_increment_rule>,
      pegtl::seq<pegtl::at<Instruction_decrement_rule>,          Instruction_decrement_rule>,
      pegtl::seq<pegtl::at<Instruction_sop_sx_rule>,             Instruction_sop_sx_rule>,
      pegtl::seq<pegtl::at<Instruction_sop_n_rule>,              Instruction_sop_n_rule>,
      pegtl::seq<pegtl::at<Instruction_aop_rule>,                Instruction_aop_rule>,

      // 5. Memory Assignments
      pegtl::seq<pegtl::at<Instruction_S_to_mem_assignment_rule>,       Instruction_S_to_mem_assignment_rule>,
      pegtl::seq<pegtl::at<Instruction_mem_to_reg_assignment_rule>, Instruction_mem_to_reg_assignment_rule>,

      // 6. Comparison Assignment (MUST be before generic assignment)
      pegtl::seq<pegtl::at<Instruction_reg_cmp_rule>, Instruction_reg_cmp_rule>,

      // 7. General Assignment (Guarded)
      // If "not_at<reg_cmp>" fails, it means it IS a comparison, so we skip this rule.
      pegtl::seq<
        pegtl::at<Instruction_S_to_reg_assignment_rule>,
        pegtl::not_at<Instruction_reg_cmp_rule>, 
        Instruction_S_to_reg_assignment_rule
      >,

      // 8. Comments
      pegtl::seq<pegtl::at<comment>, comment>
    > { };
// struct Instruction_rule:
//     pegtl::sor<
//       pegtl::seq<pegtl::at<Instruction_return_rule>,        Instruction_return_rule>,
//       pegtl::seq<pegtl::at<Instruction_cjump_rule>,         Instruction_cjump_rule>,
//       pegtl::seq<pegtl::at<Instruction_goto_label_rule>,    Instruction_goto_label_rule>,
//       pegtl::seq<pegtl::at<Instruction_reg_cmp_rule>,       Instruction_reg_cmp_rule>,
      
//       pegtl::seq<pegtl::at<Instruction_print_rule>,         Instruction_print_rule>,
//       pegtl::seq<pegtl::at<Instruction_input_rule>,         Instruction_input_rule>,
//       pegtl::seq<pegtl::at<Instruction_allocate_rule>,      Instruction_allocate_rule>,
//       pegtl::seq<pegtl::at<Instruction_tuple_error_rule>,   Instruction_tuple_error_rule>,
//       pegtl::seq<pegtl::at<Instruction_tensor_error_rule>,  Instruction_tensor_error_rule>,
//       pegtl::seq<pegtl::at<Instruction_function_call_rule>, Instruction_function_call_rule>,

//       pegtl::seq<pegtl::at<Instruction_address_calculation_rule>,      Instruction_address_calculation_rule>,
//       pegtl::seq<pegtl::at<Instruction_memory_access_increment_t_rule>, Instruction_memory_access_increment_t_rule>,
//       pegtl::seq<pegtl::at<Instruction_memory_access_decrement_t_rule>, Instruction_memory_access_decrement_t_rule>,
      
      
//       pegtl::seq<pegtl::at<Instruction_reg_increment_mem_rule>,  Instruction_reg_increment_mem_rule>,
//       pegtl::seq<pegtl::at<Instruction_reg_decrement_mem_rule>,  Instruction_reg_decrement_mem_rule>,
//       pegtl::seq<pegtl::at<Instruction_increment_rule>,          Instruction_increment_rule>,
//       pegtl::seq<pegtl::at<Instruction_decrement_rule>,          Instruction_decrement_rule>,
//       pegtl::seq<pegtl::at<Instruction_sop_sx_rule>,             Instruction_sop_sx_rule>,
//       pegtl::seq<pegtl::at<Instruction_sop_n_rule>,              Instruction_sop_n_rule>,
//       pegtl::seq<pegtl::at<Instruction_aop_rule>,                Instruction_aop_rule>,

//       // pegtl::seq<pegtl::at<Instruction_S_to_reg_assignment_rule>,   Instruction_S_to_reg_assignment_rule>,

//       pegtl::seq<pegtl::at<Instruction_label_rule>, Instruction_label_rule>,

//       pegtl::seq<pegtl::at<Instruction_S_to_mem_assignment_rule>,       Instruction_S_to_mem_assignment_rule>,
//       pegtl::seq<pegtl::at<Instruction_mem_to_reg_assignment_rule>, Instruction_mem_to_reg_assignment_rule>,
//       pegtl::seq<
//         pegtl::at<Instruction_S_to_reg_assignment_rule>,
//         pegtl::not_at<Instruction_reg_cmp_rule>, 
//         Instruction_S_to_reg_assignment_rule
//       >,

//       pegtl::seq<pegtl::at<comment>,                comment>
//     > { };

  struct Instructions_rule:
    pegtl::plus<
      pegtl::seq<
        seps,
        pegtl::bol,
        spaces,
        Instruction_rule,
        seps
      >
    > { };
  
  struct Function_def_rule:
    pegtl::seq<
      pegtl::seq<spaces, pegtl::one< '(' >>,
      seps_with_comments,
      pegtl::seq<spaces, function_name_rule>
      > {};

  struct Function_rule:
    pegtl::seq<
      Function_def_rule,
      seps_with_comments,
      pegtl::seq<spaces, argument_number>,
      seps_with_comments,
      pegtl::seq<spaces, local_number>,
      seps_with_comments,
      Instructions_rule,
      seps_with_comments,
      pegtl::seq<spaces, pegtl::one< ')' >>
    > {};

  struct Functions_rule:
    pegtl::plus<
      seps_with_comments,
      Function_rule,
      seps_with_comments
    > {};

  struct entry_point_rule:
    pegtl::seq<
      seps_with_comments,
      pegtl::seq<spaces, pegtl::one< '(' >>,
      seps_with_comments,
      function_name_rule,
      seps_with_comments,
      pegtl::plus<Functions_rule>,
      seps_with_comments,
      pegtl::seq<spaces, pegtl::one< ')' >>,
      seps
    > { };

  struct grammar : 
    pegtl::must< 
      entry_point_rule
    > {};



  /* 
   * Actions attached to grammar rules.
   */
  template< typename Rule >
  struct action : pegtl::nothing< Rule > {};

  template<> struct action < Function_def_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item_ptr = std::move(parsed_items.back());
      FunctionName* name_ptr = dynamic_cast<FunctionName*>(item_ptr.release());
      parsed_items.pop_back();

      auto newF = std::make_unique<Function>();
      newF->name = name_ptr->function_name;
      p.functions.push_back(std::move(newF));
    }
  };

  template<> struct action < function_name_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item_ptr = std::move(parsed_items.back());
      Name* name_ptr = dynamic_cast<Name*>(item_ptr.release());
      parsed_items.pop_back();

      if (p.entryPointLabel.empty()){
        p.entryPointLabel = name_ptr->name;
      } else {
        auto newFunctionName = std::make_unique<FunctionName>(name_ptr->name);
        parsed_items.push_back(std::move(newFunctionName));
      }
    }
  };

  template<> struct action < argument_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      p.functions.back()->arguments = std::stoll(in.string());
    }
  };

  template<> struct action < local_number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      p.functions.back()->locals = std::stoll(in.string());
    }
  };

  template<> struct action < number > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto newNum = std::make_unique<Number>(std::stoll(in.string()));
      parsed_items.push_back(std::move(newNum));
    }
  };

  template<> struct action < label > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item_ptr = std::move(parsed_items.back());
      Name* name_ptr = dynamic_cast<Name*>(item_ptr.release());
      parsed_items.pop_back();

      auto newLabel = std::make_unique<Label>(name_ptr->name);
      parsed_items.push_back(std::move(newLabel));
    }
  };

  template<> struct action < name > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto newName = std::make_unique<Name>(in.string());
      parsed_items.push_back(std::move(newName));
    }
  };

  // registers

  template<> struct action < register_rdi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rdi);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rax_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rax);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rbx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rbx);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rcx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rcx);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rsi_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rsi);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rdx_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rdx);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rbp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rbp);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r8_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r8);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r9_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r9);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r10_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r10);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r11_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r11);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r12_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r12);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r13_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r13);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r14_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r14);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_r15_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::r15);
      parsed_items.push_back(std::move(r));
    }
  };
  template<> struct action < register_rsp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){
      auto r = std::make_unique<Register>(ERegister::rsp);
      parsed_items.push_back(std::move(r));
    }
  };

  template<> struct action < aop_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){

      std::unique_ptr<AssignmentOp> aop_ptr;
      std::string in_string = in.string();
      if (in_string == "+="){
        aop_ptr = std::make_unique<AssignmentOp>(EAssignmentOperator::INCREMENT);
      } else if (in_string == "-="){
        aop_ptr = std::make_unique<AssignmentOp>(EAssignmentOperator::DECREMENT);
      } else if (in_string == "*="){
        aop_ptr = std::make_unique<AssignmentOp>(EAssignmentOperator::MULTIPLY);
      } else if (in_string == "&="){
        aop_ptr = std::make_unique<AssignmentOp>(EAssignmentOperator::AND);
      } else {
        throw std::runtime_error("Parsing Error: unkown AOP");
      }
      parsed_items.push_back(std::move(aop_ptr));
    }
  };

  template<> struct action < sop_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){

      std::unique_ptr<ShiftOp> sop_ptr;

      if (in.string() == "<<="){
        sop_ptr = std::make_unique<ShiftOp>(EShiftOperator::LEFT);
      } else if (in.string() == ">>="){
        sop_ptr = std::make_unique<ShiftOp>(EShiftOperator::RIGHT);
      } else {
        throw std::runtime_error("Parsing Error: unkown SOP");
      }
      parsed_items.push_back(std::move(sop_ptr));
    }
  };

  template<> struct action < cmp_rule > {
    template< typename Input >
    static void apply( const Input & in, Program & p){

      std::unique_ptr<Comparator> cmp_ptr;
      std::string in_string = in.string();

      if (in_string == "<"){
        cmp_ptr = std::make_unique<Comparator>(EComparator::LT);
      } else if (in_string == "<="){
        cmp_ptr = std::make_unique<Comparator>(EComparator::LTE);
      } else if (in_string == "="){
        cmp_ptr = std::make_unique<Comparator>(EComparator::EQ);
      } else {
        throw std::runtime_error("Parsing Error: unkown CMP");
      }
      parsed_items.push_back(std::move(cmp_ptr));
    }
  };

  template<> struct action < memory_access_rule > {
    template < typename Input >
    static void apply (const Input &in, Program &p){

      auto item_ptr = std::move(parsed_items.back());
      Number* offset_ptr = dynamic_cast<Number*>(item_ptr.release());
      parsed_items.pop_back();

      auto reg = std::move(parsed_items.back());
      Register* reg_ptr = dynamic_cast<Register*>(reg.release());
      parsed_items.pop_back();

      if (!reg_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted Item to Register");
      if (!offset_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted Item to Number");
      if (offset_ptr->number % 8 != 0) throw std::runtime_error("L1 Syntax Error: Memory Offset must be a multiple of 8.");
      
      auto newMemAccess = std::make_unique<MemoryAccess>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Number>(offset_ptr));

      parsed_items.push_back(std::move(newMemAccess));
    }
  };

  
  template<> struct action< Instruction_return_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto i = std::make_unique<Instruction_ret>();

      i->arguments = p.functions.back()->arguments;
      i->locals = p.functions.back()->locals;
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action< Instruction_print_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto i = std::make_unique<Instruction_print_call>();
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };
  template<> struct action< Instruction_input_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto i = std::make_unique<Instruction_input>();
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };
  template<> struct action< Instruction_allocate_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto i = std::make_unique<Instruction_allocate>();
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };
  template<> struct action< Instruction_tuple_error_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto i = std::make_unique<Instruction_tuple_error>();
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };
  template<> struct action< Instruction_tensor_error_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      std::unique_ptr<Number> num_ptr = std::unique_ptr<Number>(dynamic_cast<Number*>(item_ptr.release()));

      auto i = std::make_unique<Instruction_tensor_error>(std::move(num_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action< Instruction_function_call_rule> {
    template< typename Input >
	  static void apply( const Input & in, Program & p){

      //very back item should be N, indicating number of arguments
      auto item_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      
      Number* number_ptr = dynamic_cast<Number*>(item_ptr.release());
      // u object
      item_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      if (!item_ptr) throw std::runtime_error("Parsing Error: incorrectly casted item to number");

      std::unique_ptr<Instruction_function_call> i;
      
      switch (item_ptr->type){
        case ItemType::Register: {
          Register* U_ptr = dynamic_cast<Register*>(item_ptr.release());
          i = std::make_unique<Instruction_function_call>(std::unique_ptr<Register>(U_ptr), number_ptr->number);
          break;
        }
        case ItemType::FunctionName: {
          FunctionName* U_ptr = dynamic_cast<FunctionName*>(item_ptr.release());
          i = std::make_unique<Instruction_function_call>(std::unique_ptr<FunctionName>(U_ptr), number_ptr->number);
          break;
        }
        default:
          throw std::runtime_error("Syntax Error: Must be U type for Function Call");
      }

      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_label_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item_ptr = std::move(parsed_items.back());
      Name* name_ptr = dynamic_cast<Name*>(item_ptr.release());
      parsed_items.pop_back();

      auto i = std::make_unique<Instruction_label>(name_ptr->name);
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_goto_label_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item_ptr = std::move(parsed_items.back());
      Label* label_ptr = dynamic_cast<Label*>(item_ptr.release());
      parsed_items.pop_back();

      auto i = std::make_unique<Instruction_goto_label>(std::unique_ptr<Label>(label_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_S_to_reg_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());

      if (!reg_ptr) throw std::runtime_error("Parsing Error: incorrectly casted item to Register (Instruction_S_to_reg_assignment_rule)");
      
      std::unique_ptr<Instruction_S_to_reg_assignment> i;
      
      switch (src_ptr->type){
        case ItemType::Register: {
          Register* s_ptr = dynamic_cast<Register*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_reg_assignment>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Register>(s_ptr));
          break;
        }
        case ItemType::Number: {
          Number* s_ptr = dynamic_cast<Number*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_reg_assignment>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Number>(s_ptr));
          break;
        }
        case ItemType::Label: {
          Label* s_ptr = dynamic_cast<Label*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_reg_assignment>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Label>(s_ptr));
          break;
        }
        case ItemType::FunctionName: {
          FunctionName* s_ptr = dynamic_cast<FunctionName*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_reg_assignment>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<FunctionName>(s_ptr));
          break;
        }
        default:
          throw std::runtime_error("Syntax Error: Must be a S for S_to_reg Assignment");
      }
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_mem_to_reg_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      MemoryAccess* mem_access_ptr = dynamic_cast<MemoryAccess*>(src_ptr.release());

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());

      if (!mem_access_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to MemoryAccess");

      auto i = std::make_unique<Instruction_mem_to_reg_assignment>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<MemoryAccess>(mem_access_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_S_to_mem_assignment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      MemoryAccess* mem_access_ptr = dynamic_cast<MemoryAccess*>(dst_ptr.release());

      if (!mem_access_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to MemoryAccess");

      std::unique_ptr<Instruction_S_to_mem_assignment> i;

      switch (src_ptr->type){
        case ItemType::Register: {
          Register* s_ptr = dynamic_cast<Register*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_mem_assignment>(std::unique_ptr<MemoryAccess>(mem_access_ptr), std::unique_ptr<Register>(s_ptr));
          break;
        }
        case ItemType::Number: {
          Number* s_ptr = dynamic_cast<Number*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_mem_assignment>(std::unique_ptr<MemoryAccess>(mem_access_ptr), std::unique_ptr<Number>(s_ptr));
          break;
        }
        case ItemType::Label: {
          Label* s_ptr = dynamic_cast<Label*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_mem_assignment>(std::unique_ptr<MemoryAccess>(mem_access_ptr), std::unique_ptr<Label>(s_ptr));
          break;
        }
        case ItemType::FunctionName: {
          FunctionName* s_ptr = dynamic_cast<FunctionName*>(src_ptr.release());
          i = std::make_unique<Instruction_S_to_mem_assignment>(std::unique_ptr<MemoryAccess>(mem_access_ptr), std::unique_ptr<FunctionName>(s_ptr));
          break;
        }
        default:
          throw std::runtime_error("Syntax Error: Must be a S for S_to_mem Assignment");
      }
                                     
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  // Note: need to add error checking for excluded registers

  template<> struct action < Instruction_aop_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto op_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());
      AssignmentOp* aop_ptr = dynamic_cast<AssignmentOp*>(op_ptr.release());
      
      if (!reg_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Register");
      if (!aop_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to AOP");

      std::unique_ptr<Instruction_aop> i;
      switch (src_ptr->type){
        case ItemType::Register: {
          Register* src_reg_ptr = dynamic_cast<Register*>(src_ptr.release());
          i = std::make_unique<Instruction_aop>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<AssignmentOp>(aop_ptr), std::unique_ptr<Register>(src_reg_ptr));
          break;
        }
        case ItemType::Number: {
          Number* src_num_ptr = dynamic_cast<Number*>(src_ptr.release());
          i = std::make_unique<Instruction_aop>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<AssignmentOp>(aop_ptr), std::unique_ptr<Number>(src_num_ptr));
          break;
        }
        default:
          throw std::runtime_error("Parsing Error: Unable to cast Instruction_AOP src type");
      }

      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_sop_sx_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto op_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());
      ShiftOp* sop_ptr = dynamic_cast<ShiftOp*>(op_ptr.release());
      Register* sx_ptr = dynamic_cast<Register*>(src_ptr.release());
      
      if (!reg_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Register");
      if (!sx_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Register");
      if (!sop_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to SOP");
      
      if (sx_ptr->reg != ERegister::rcx) throw std::runtime_error("Syntax Error: Register must be SX for Instruction_SOP_SX");

      std::unique_ptr<Instruction_sop_sx> i = std::make_unique<Instruction_sop_sx>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<ShiftOp>(sop_ptr), std::unique_ptr<Register>(sx_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_sop_n_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto op_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());
      ShiftOp* sop_ptr = dynamic_cast<ShiftOp*>(op_ptr.release());
      Number* num_ptr = dynamic_cast<Number*>(src_ptr.release());
      
      if (!reg_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Register");
      if (!num_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Number");
      if (!sop_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to SOP");
      
      std::unique_ptr<Instruction_sop_n> i = std::make_unique<Instruction_sop_n>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<ShiftOp>(sop_ptr), std::unique_ptr<Number>(num_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_reg_increment_mem_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      MemoryAccess* mem_ptr = dynamic_cast<MemoryAccess*>(src_ptr.release());
      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());

      if (!mem_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to MemoryAccess");
      if (!reg_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Register");
      
      std::unique_ptr<Instruction_reg_mem_increment> i = std::make_unique<Instruction_reg_mem_increment>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<MemoryAccess>(mem_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_reg_decrement_mem_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      MemoryAccess* mem_ptr = dynamic_cast<MemoryAccess*>(src_ptr.release());
      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());

      if (!mem_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to MemoryAccess");
      if (!reg_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to Register");
      
      std::unique_ptr<Instruction_reg_mem_decrement> i = std::make_unique<Instruction_reg_mem_decrement>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<MemoryAccess>(mem_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_memory_access_increment_t_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      MemoryAccess* mem_ptr = dynamic_cast<MemoryAccess*>(dst_ptr.release());

      if (!mem_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to MemoryAccess");
      
      std::unique_ptr<Instruction_memory_access_increment_t> i;
      switch (src_ptr->type){
        case ItemType::Register: {
          Register* src_reg_ptr = dynamic_cast<Register*>(src_ptr.release());
          i = std::make_unique<Instruction_memory_access_increment_t>(std::unique_ptr<MemoryAccess>(mem_ptr), std::unique_ptr<Register>(src_reg_ptr));
          break;
        }
        case ItemType::Number: {
          Number* src_num_ptr = dynamic_cast<Number*>(src_ptr.release());
          i = std::make_unique<Instruction_memory_access_increment_t>(std::unique_ptr<MemoryAccess>(mem_ptr), std::unique_ptr<Number>(src_num_ptr));
          break;
        }
        default:
          throw std::runtime_error("Parsing Error: Unable to cast src type");
      }

      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_memory_access_decrement_t_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto src_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      MemoryAccess* mem_ptr = dynamic_cast<MemoryAccess*>(dst_ptr.release());

      if (!mem_ptr) throw std::runtime_error("Parsing Error: Incorrectly casted to MemoryAccess");
      
      std::unique_ptr<Instruction_memory_access_decrement_t> i;
      switch (src_ptr->type){
        case ItemType::Register: {
          Register* src_reg_ptr = dynamic_cast<Register*>(src_ptr.release());
          i = std::make_unique<Instruction_memory_access_decrement_t>(std::unique_ptr<MemoryAccess>(mem_ptr), std::unique_ptr<Register>(src_reg_ptr));
          break;
        }
        case ItemType::Number: {
          Number* src_num_ptr = dynamic_cast<Number*>(src_ptr.release());
          i = std::make_unique<Instruction_memory_access_decrement_t>(std::unique_ptr<MemoryAccess>(mem_ptr), std::unique_ptr<Number>(src_num_ptr));
          break;
        }
        default:
          throw std::runtime_error("Parsing Error: Unable to cast src type");
      }

      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_reg_cmp_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){

      auto operand2_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto op_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto operand1_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto dst_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Comparator* cmp_ptr = dynamic_cast<Comparator*>(op_ptr.release());
      Register* reg_ptr = dynamic_cast<Register*>(dst_ptr.release());

      std::unique_ptr<Instruction_reg_cmp> i;

      if (operand1_ptr->type == ItemType::Register){
        Register* operand1_reg_ptr = dynamic_cast<Register*>(operand1_ptr.release());
        
        if (operand2_ptr->type == ItemType::Register){
          Register* operand2_reg_ptr = dynamic_cast<Register*>(operand2_ptr.release());
          i = std::make_unique<Instruction_reg_cmp>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Register>(operand1_reg_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Register>(operand2_reg_ptr));
        } else if (operand2_ptr->type == ItemType::Number){
          Number* operand2_num_ptr = dynamic_cast<Number*>(operand2_ptr.release());
          i = std::make_unique<Instruction_reg_cmp>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Register>(operand1_reg_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Number>(operand2_num_ptr));
        } else {
          throw std::runtime_error("Parsing Error: Unable to cast operand2 type");
        }
      
      } else if (operand1_ptr->type == ItemType::Number){
        Number* operand1_num_ptr = dynamic_cast<Number*>(operand1_ptr.release());
      
        if (operand2_ptr->type == ItemType::Register){
          Register* operand2_reg_ptr = dynamic_cast<Register*>(operand2_ptr.release());
          i = std::make_unique<Instruction_reg_cmp>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Number>(operand1_num_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Register>(operand2_reg_ptr));
        } else if (operand2_ptr->type == ItemType::Number){
          Number* operand2_num_ptr = dynamic_cast<Number*>(operand2_ptr.release());
          i = std::make_unique<Instruction_reg_cmp>(std::unique_ptr<Register>(reg_ptr), std::unique_ptr<Number>(operand1_num_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Number>(operand2_num_ptr));
        } else {
          throw std::runtime_error("Parsing Error: Unable to cast operand2 type");
        }
      
      } else {
        throw std::runtime_error("Parsing Error: Unable to cast operand1 type");
      }
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_cjump_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){

      auto target_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      
      auto operand2_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto op_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      auto operand1_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Label* label_ptr = dynamic_cast<Label*>(target_ptr.release());
      Comparator* cmp_ptr = dynamic_cast<Comparator*>(op_ptr.release());
      
      std::unique_ptr<Instruction_cjump> i;

      if (operand1_ptr->type == ItemType::Register){
        Register* operand1_reg_ptr = dynamic_cast<Register*>(operand1_ptr.release());
        
        if (operand2_ptr->type == ItemType::Register){
          Register* operand2_reg_ptr = dynamic_cast<Register*>(operand2_ptr.release());
          i = std::make_unique<Instruction_cjump>(std::unique_ptr<Register>(operand1_reg_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Register>(operand2_reg_ptr), std::unique_ptr<Label>(label_ptr));
        } else if (operand2_ptr->type == ItemType::Number){
          Number* operand2_num_ptr = dynamic_cast<Number*>(operand2_ptr.release());
          i = std::make_unique<Instruction_cjump>(std::unique_ptr<Register>(operand1_reg_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Number>(operand2_num_ptr), std::unique_ptr<Label>(label_ptr));
        } else {
          throw std::runtime_error("Parsing Error: Unable to cast operand2 type");
        }
      
      } else if (operand1_ptr->type == ItemType::Number){
        Number* operand1_num_ptr = dynamic_cast<Number*>(operand1_ptr.release());
      
        if (operand2_ptr->type == ItemType::Register){
          Register* operand2_reg_ptr = dynamic_cast<Register*>(operand2_ptr.release());
          i = std::make_unique<Instruction_cjump>(std::unique_ptr<Number>(operand1_num_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Register>(operand2_reg_ptr), std::unique_ptr<Label>(label_ptr));
        } else if (operand2_ptr->type == ItemType::Number){
          Number* operand2_num_ptr = dynamic_cast<Number*>(operand2_ptr.release());
          i = std::make_unique<Instruction_cjump>(std::unique_ptr<Number>(operand1_num_ptr), std::unique_ptr<Comparator>(cmp_ptr), std::unique_ptr<Number>(operand2_num_ptr), std::unique_ptr<Label>(label_ptr));
        } else {
          throw std::runtime_error("Parsing Error: Unable to cast operand2 type");
        }
      
      } else {
        throw std::runtime_error("Parsing Error: Unable to cast operand1 type");
      }
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_increment_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto item_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg_ptr = dynamic_cast<Register*>(item_ptr.release());

      if (!reg_ptr) throw std::runtime_error("Parsing Error: incorrectly casted item to Register");
      std::unique_ptr<Instruction_increment> i = std::make_unique<Instruction_increment>(std::unique_ptr<Register>(reg_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_decrement_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      
      auto item_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg_ptr = dynamic_cast<Register*>(item_ptr.release());

      if (!reg_ptr) throw std::runtime_error("Parsing Error: incorrectly casted item to Register");
      std::unique_ptr<Instruction_decrement> i = std::make_unique<Instruction_decrement>(std::unique_ptr<Register>(reg_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  template<> struct action < Instruction_address_calculation_rule > {
    template< typename Input >
	  static void apply( const Input & in, Program & p){
      auto item4_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      auto item3_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      auto item2_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();
      auto item1_ptr = std::move(parsed_items.back());
      parsed_items.pop_back();

      Register* reg1_ptr = dynamic_cast<Register*>(item1_ptr.release());
      Register* reg2_ptr = dynamic_cast<Register*>(item2_ptr.release());
      Register* reg3_ptr = dynamic_cast<Register*>(item3_ptr.release());
      Number* num_ptr = dynamic_cast<Number*>(item4_ptr.release());

      std::unique_ptr<Instruction_address_calculation> i = std::make_unique<Instruction_address_calculation>(std::unique_ptr<Register>(reg1_ptr), std::unique_ptr<Register>(reg2_ptr), std::unique_ptr<Register>(reg3_ptr), std::unique_ptr<Number>(num_ptr));
      p.functions.back()->instructions.push_back(std::move(i));
    }
  };

  Program parse_file (char *fileName){

    /* 
     * Check the grammar for some possible issues.
     */
    if (pegtl::analyze< grammar >() != 0){
      std::cerr << "There are problems with the grammar" << std::endl;
      exit(1);
    }

    /*
     * Parse.
     */
    file_input< > fileInput(fileName);
    Program p;
    parse< grammar, action >(fileInput, p);

    return p;
  }

}
