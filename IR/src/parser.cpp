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

#include "parser.h"

namespace pegtl = TAO_PEGTL_NAMESPACE;

using namespace pegtl;

namespace IR
{

    /*
     * Tokens parsed
     */
    std::vector<std::shared_ptr<Item>> parsed_items;
    // std::vector<std::unique_ptr<std::vector<std::shared_ptr<Item>>>> parsed_item_collections;

    /*
     * Grammar rules from now on.
     */
    struct name : pegtl::seq<
                      pegtl::plus<
                          pegtl::sor<
                              pegtl::alpha,
                              pegtl::one<'_'>>>,
                      pegtl::star<
                          pegtl::sor<
                              pegtl::alpha,
                              pegtl::one<'_'>,
                              pegtl::digit>>>
    {
    };

    struct op : pegtl::sor<
                    TAO_PEGTL_STRING("+"),
                    TAO_PEGTL_STRING("-"),
                    TAO_PEGTL_STRING("*"),
                    TAO_PEGTL_STRING("<<"),
                    TAO_PEGTL_STRING(">>"),
                    TAO_PEGTL_STRING("&"),
                    TAO_PEGTL_STRING("<="),
                     TAO_PEGTL_STRING(">="),
                     TAO_PEGTL_STRING("<"),
                     TAO_PEGTL_STRING(">"),
                     TAO_PEGTL_STRING("=")>
    {
    };

    /*
     * Keywords.
     */
    struct str_return : TAO_PEGTL_STRING("return")
    {
    };
    // struct str_arrow;
    struct str_arrow : TAO_PEGTL_STRING("<-")
    {
    };
    struct str_call : TAO_PEGTL_STRING("call")
    {
    };

    struct label : pegtl::seq<
                       pegtl::one<':'>,
                       name>
    {
    };

    struct var : pegtl::seq<
                     pegtl::one<'%'>,
                     name>
    {
    };

    struct function_name : pegtl::seq<
                               pegtl::one<'@'>,
                               name>
    {
    };

    struct number : pegtl::seq<
                        pegtl::opt<
                            pegtl::sor<
                                pegtl::one<'-'>,
                                pegtl::one<'+'>>>,
                        pegtl::plus<
                            pegtl::digit>>
    {
    };

    struct type : pegtl::sor<
                        TAO_PEGTL_STRING("tuple"),
                        TAO_PEGTL_STRING("code"),
                        pegtl::seq<TAO_PEGTL_STRING("int64"), pegtl::plus<TAO_PEGTL_STRING("[]")>>,
                        TAO_PEGTL_STRING("int64"),
                        TAO_PEGTL_STRING("void")
    >
    {
    };

    struct t_rule : pegtl::sor<
                        var,
                        number>
    {
    };

    struct u_rule : pegtl::sor<
                        var,
                        function_name>
    {
    };

    struct s_rule : pegtl::sor<
                        t_rule,
                        label,
                        function_name>
    {
    };

    struct callee_rule : pegtl::sor<
                             u_rule,
                             TAO_PEGTL_STRING("print"),
                             TAO_PEGTL_STRING("allocate"),
                             TAO_PEGTL_STRING("input"),
                             TAO_PEGTL_STRING("tuple-error"),
                             TAO_PEGTL_STRING("tensor-error")>
    {
    };

    struct comment : pegtl::disable<
                         TAO_PEGTL_STRING("//"),
                         pegtl::until<pegtl::eolf>>
    {
    };

    /*
     * Separators.
     */
    struct spaces : pegtl::star<
                        pegtl::sor<
                            pegtl::one<' '>,
                            pegtl::one<'\t'>>>
    {
    };

    struct seps : pegtl::star<
                      pegtl::seq<
                          spaces,
                          pegtl::eol>>
    {
    };
    struct seps_with_comments : pegtl::star<
                                    pegtl::seq<
                                        spaces,
                                        pegtl::sor<
                                            pegtl::eol,
                                            comment>>>
    {
    };

    struct type_def : pegtl::seq<type,
    spaces,
    var>{};

    // instructions

    struct Instruction_Var_Def_rule : pegtl::seq<type_def>
    {};

    struct Instruction_Var_S_Assignment_rule : pegtl::seq<
                                                   var,
                                                   spaces,
                                                   str_arrow,
                                                   spaces,
                                                   s_rule>
    {
    };

    struct Instruction_Var_T_Op_T_Assignment_rule : pegtl::seq<
                                                        var,
                                                        spaces,
                                                        str_arrow,
                                                        spaces,
                                                        t_rule,
                                                        spaces,
                                                        op,
                                                        spaces,
                                                        t_rule>
    {
    };


    struct Instruction_Return_rule : str_return
    {
    };

    struct Instruction_Return_T_rule : pegtl::seq<
                                           str_return,
                                           spaces,
                                           t_rule>
    {
    };


    struct Instruction_Br_Label_rule : pegtl::seq<TAO_PEGTL_STRING("br"), spaces, label>
    {
    };

    struct Instruction_Br_T_Label_Label_rule : pegtl::seq<TAO_PEGTL_STRING("br"), spaces, t_rule, spaces, label, spaces, label>
    {
    };

    struct Instruction_Var_Array_Assignment_rule : pegtl::seq<
        var,
        spaces,
        str_arrow,
        spaces,
        var,
        pegtl::plus<
            pegtl::seq<
            TAO_PEGTL_STRING("["),
            t_rule,
            TAO_PEGTL_STRING("]")>
            >
    >{};

    struct Instruction_Array_S_Assignment_rule : pegtl::seq<
        var,
        pegtl::plus<
            pegtl::seq<
            TAO_PEGTL_STRING("["),
            t_rule,
            TAO_PEGTL_STRING("]")>
            >,
        spaces,
        str_arrow,
        spaces,
        s_rule
    >{};

    struct Instruction_Var_Length_Var_T_Assignment_rule : pegtl::seq<
        var,
        spaces,
        str_arrow,
        spaces,
        TAO_PEGTL_STRING("length"),
        spaces,
        var,
        spaces,
        t_rule
    >{};

    struct Instruction_Var_Length_Var_Assignment_rule : pegtl::seq<
        var,
        spaces,
        str_arrow,
        spaces,
        TAO_PEGTL_STRING("length"),
        spaces,
        var
    >{};

    struct args_rule : pegtl::opt<
                           pegtl::seq<
                               t_rule,
                               pegtl::star<
                                   pegtl::seq<
                                       spaces,
                                       pegtl::one<','>,
                                       spaces,
                                       t_rule>>>>
    {
    };

    struct Instruction_Call_Function_rule : pegtl::seq<
                                                str_call,
                                                spaces,
                                                callee_rule,
                                                spaces,
                                                pegtl::one<'('>,
                                                spaces,
                                                args_rule,
                                                spaces,
                                                pegtl::one<')'>>
    {
    };

    struct Instruction_Var_Function_Assignment_rule : pegtl::seq<
                                                          var,
                                                          spaces,
                                                          str_arrow,
                                                          spaces,
                                                          Instruction_Call_Function_rule>
    {
    };

    struct Instruction_Var_Array_Init_rule : pegtl::seq <
        var,
        spaces,
        str_arrow,
        spaces,
        TAO_PEGTL_STRING("new Array("),
        spaces,
        args_rule,
        spaces,
        TAO_PEGTL_STRING(")")
    >
    {};

    struct Instruction_Var_Tuple_Init_rule : pegtl::seq <
        var,
        spaces,
        str_arrow,
        spaces,
        TAO_PEGTL_STRING("new Tuple("),
        spaces,
        t_rule,
        spaces,
        TAO_PEGTL_STRING(")")
    >
    {};

    struct End_Instruction_rule : pegtl::sor<
                                Instruction_Br_Label_rule,
                                Instruction_Br_T_Label_Label_rule,
                                Instruction_Return_T_rule,
                                Instruction_Return_rule
    >{};

    struct Instruction_rule : pegtl::sor<
                                    pegtl::seq<pegtl::at<Instruction_Var_Def_rule>, Instruction_Var_Def_rule>,

                                    pegtl::seq<pegtl::at<Instruction_Var_Array_Assignment_rule>, Instruction_Var_Array_Assignment_rule>,
                                    pegtl::seq<pegtl::at<Instruction_Array_S_Assignment_rule>, Instruction_Array_S_Assignment_rule>,

                                    pegtl::seq<pegtl::at<Instruction_Var_Length_Var_T_Assignment_rule>, Instruction_Var_Length_Var_T_Assignment_rule>,
                                    pegtl::seq<pegtl::at<Instruction_Var_Length_Var_Assignment_rule>, Instruction_Var_Length_Var_Assignment_rule>,

                                    pegtl::seq<pegtl::at<Instruction_Var_Array_Init_rule>, Instruction_Var_Array_Init_rule>,
                                    pegtl::seq<pegtl::at<Instruction_Var_Tuple_Init_rule>, Instruction_Var_Tuple_Init_rule>,

                                  pegtl::seq<pegtl::at<Instruction_Var_Function_Assignment_rule>, Instruction_Var_Function_Assignment_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Call_Function_rule>, Instruction_Call_Function_rule>,

                                  pegtl::seq<pegtl::at<Instruction_Var_T_Op_T_Assignment_rule>, Instruction_Var_T_Op_T_Assignment_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Var_S_Assignment_rule>, Instruction_Var_S_Assignment_rule>,

                                  pegtl::seq<pegtl::at<comment>, comment>>
    {
    };

    struct Instructions_rule : pegtl::plus<
                                   pegtl::seq<
                                       seps,
                                       pegtl::bol,
                                       spaces,
                                       Instruction_rule,
                                       seps>>
    {
    };

    struct Block_Label : pegtl::seq<label> {};

    struct BasicBlock : pegtl::seq<
        spaces,
        Block_Label,
        seps_with_comments,             
        pegtl::opt<Instructions_rule>,
        seps_with_comments,             
        spaces,                         
        End_Instruction_rule
    >{};

    struct params_rule : pegtl::opt<
                           pegtl::seq<
                                type_def,
                               pegtl::star<
                                   pegtl::seq<
                                       spaces,
                                       pegtl::one<','>,
                                       spaces,
                                       type_def>>>>
    {
    };

    struct Function_Def_rule : pegtl::seq<
                                   TAO_PEGTL_STRING("define"), 
                                   spaces,
                                   type,
                                   spaces,
                                   function_name, 
                                   spaces,
                                   pegtl::one<'('>, spaces, params_rule, spaces, pegtl::one<')'>, spaces>
    {
    };

    struct Function_rule : pegtl::seq<
                               Function_Def_rule,
                               seps_with_comments,
                               pegtl::one<'{'>,
                               seps_with_comments,
                               pegtl::plus<
                                pegtl::seq<
                                    seps_with_comments,
                                    BasicBlock,
                                    seps_with_comments
                                >
                               >,
                               seps_with_comments,
                               spaces,
                               pegtl::one<'}'>>
    {
    };

    struct Functions_rule : pegtl::plus<
                                seps_with_comments,
                                Function_rule,
                                seps_with_comments>
    {
    };

    struct grammar : pegtl::seq<
                         seps_with_comments,
                         pegtl::plus<Functions_rule>,
                         seps_with_comments,
                         pegtl::eof>
    {
    };

    /*
     * Actions attached to grammar rules.
     */
    template <typename Rule>
    struct action : pegtl::nothing<Rule>
    {
    };

    template <>
    struct action<name>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            parsed_items.push_back(std::make_shared<Name>(in.string()));
        }
    };

    template <>
    struct action<label>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            if (name == nullptr)
                throw std::runtime_error("Name not parsed (label)");
            parsed_items.pop_back();
            parsed_items.push_back(std::make_shared<Label>(name->name));
        }
    };

    template <>
    struct action<function_name>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            if (name == nullptr)
                throw std::runtime_error("Name not parsed (function_name)");
            parsed_items.pop_back();
            parsed_items.push_back(std::make_shared<FunctionName>(name->name));
        }
    };

    template <>
    struct action<number>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            parsed_items.push_back(std::make_shared<Number>(std::stoll(in.string())));
        }
    };

    template <>
    struct action<var>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            if (name == nullptr)
                throw std::runtime_error("Name not parsed (var)");
            parsed_items.pop_back();
            parsed_items.push_back(std::make_shared<Variable>(name->name));
        }
    };

    template <>
    struct action<type>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {   
            std::string in_string = in.string();
            std::shared_ptr<Type> type = std::make_shared<Type>();

            if (in_string == "tuple"){
                type->var_type = EType::TUPLE;
            } else if (in_string == "code"){
                type->var_type = EType::CODE;
            } else if (in_string == "void"){
                type->var_type = EType::VOID;
            } else if (in_string == "int64"){
                type->var_type = EType::INT;
            } else {
                type->var_type = EType::ARRAY;
                type->array_dims = (in_string.length()-5)/2;
            }

            parsed_items.push_back(std::move(type));
        }
    };

    template <>
    struct action<type_def>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Type> type = std::dynamic_pointer_cast<Type>(parsed_items.back());
            parsed_items.pop_back();
            parsed_items.push_back(std::make_shared<TypeDef>(type, var));
        }
    };

    template <>
    struct action<op>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            EOperator op_enum;
            std::string op_str = in.string();

            if (op_str == "+"){
                op_enum = EOperator::ADD;
            } else if (op_str == "-"){
                op_enum = EOperator::SUB;
            } else if (op_str == "*") {
                op_enum = EOperator::MULT;
            } else if (op_str == "&") {
                op_enum = EOperator::AND;
            } else if (op_str == "<<") {
                op_enum = EOperator::LEFT_SHIFT;
            } else if (op_str == ">>") {
                op_enum = EOperator::RIGHT_SHIFT;
            } else if (op_str == "<") {
                op_enum = EOperator::LT;
            } else if (op_str == "<=") {
                op_enum = EOperator::LTE;
            } else if (op_str == "=") {
                op_enum = EOperator::EQ;
            } else if (op_str == ">") {
                op_enum = EOperator::GT;
            } else if (op_str == ">=") {
                op_enum = EOperator::GTE;
            } else {
                throw std::runtime_error("Unknown Operator: " + op_str);
            }

            parsed_items.push_back(std::make_shared<Operator>(op_enum));
        }
    };

    template <>
    struct action<Function_Def_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::unique_ptr<Function> f = std::make_unique<Function>();

            // based on our grammar, there can't be nested function definitions, so all parsed objects must belong to the current function
            while (parsed_items.size() != 0)
            {
                // should be return type
                if (parsed_items.size() == 1)
                {
                    std::shared_ptr<Type> return_type = std::dynamic_pointer_cast<Type>(parsed_items.back());
                    parsed_items.pop_back();
                    if (return_type == nullptr)
                        throw std::runtime_error("Return type not parsed properly");
                    f->return_type = std::move(return_type);
                } else if (parsed_items.size() == 2)
                {
                    std::shared_ptr<FunctionName> function_name = std::dynamic_pointer_cast<FunctionName>(parsed_items.back());
                    parsed_items.pop_back();
                    if (function_name == nullptr)
                        throw std::runtime_error("Function Name not parsed properly");
                    f->function_name = std::move(function_name);
                }
                else
                {
                    std::shared_ptr<TypeDef> type_def = std::dynamic_pointer_cast<TypeDef>(parsed_items.back());
                    parsed_items.pop_back();
                    if (type_def == nullptr)
                        throw std::runtime_error("Function Parameter Vars not parsed properly");
                    f->params.push_back(std::move(type_def));
                }
            }

            // reverse bc var order was reversed
            std::reverse(f->params.begin(), f->params.end());

            p.functions.push_back(std::move(f));
        }
    };

    template <>
    struct action<Block_Label>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(parsed_items.back());
            if (label == nullptr)
                throw std::runtime_error("Label not casted properly in Block label");
            parsed_items.pop_back();

            std::unique_ptr<Block> block = std::make_unique<Block>();
            block->label = std::move(label);

            p.functions.back()->blocks.push_back(std::move(block));
        }
    };

    template <>
    struct action<Instruction_Var_Def_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<TypeDef> type_def = std::dynamic_pointer_cast<TypeDef>(parsed_items.back());
            parsed_items.pop_back();
            if (type_def == nullptr)
                throw std::runtime_error("Type Def not casted properly");
            p.functions.back()->blocks.back()->instructions.push_back(std::make_unique<Instruction_Var_Def>(std::move(type_def)));
        }
    };

    template <>
    struct action<Instruction_Var_S_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<S> s = std::dynamic_pointer_cast<S>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->blocks.back()->instructions.push_back(std::make_unique<Instruction_Var_S_Assignment>(std::move(var), std::move(s)));
        }
    };

    template <>
    struct action<Instruction_Var_T_Op_T_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<T> t2 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Operator> op = std::dynamic_pointer_cast<Operator>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t1 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->blocks.back()->instructions.push_back(std::make_unique<Instruction_Var_T_Op_T_Assignment>(std::move(var), std::move(t1), std::move(op), std::move(t2)));
        }
    };

    template <>
    struct action<Instruction_Var_Array_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            // when constructing instructions, all parsed items belong to the current instruction
            std::unique_ptr<Instruction_Var_Array_Assignment> instruction = std::make_unique<Instruction_Var_Array_Assignment>();

            while (parsed_items.size() > 2){
                std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
                parsed_items.pop_back();
                instruction->idxs.push_back(std::move(t));
            }
            std::reverse(instruction->idxs.begin(), instruction->idxs.end());

            std::shared_ptr<Variable> arr_var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            instruction->arr_var = std::move(arr_var);
            instruction->var = std::move(var);

            p.functions.back()->blocks.back()->instructions.push_back(std::move(instruction));
        }
    };

    template <>
    struct action<Instruction_Array_S_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<S> s = std::dynamic_pointer_cast<S>(parsed_items.back());
            parsed_items.pop_back();

            // when constructing instructions, all parsed items belong to the current instruction
            std::unique_ptr<Instruction_Array_S_Assignment> instruction = std::make_unique<Instruction_Array_S_Assignment>();
            instruction->s = std::move(s);

            while (parsed_items.size() > 1){
                std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
                parsed_items.pop_back();
                instruction->idxs.push_back(std::move(t));
            }
            std::reverse(instruction->idxs.begin(), instruction->idxs.end());

            std::shared_ptr<Variable> arr_var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            instruction->arr_var = std::move(arr_var);

            p.functions.back()->blocks.back()->instructions.push_back(std::move(instruction));
        }
    };

    template <>
    struct action<Instruction_Var_Length_Var_T_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var2 = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var1 = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->blocks.back()->instructions.push_back(std::make_unique<Instruction_Var_Length_Var_T_Assignment>(std::move(var1), std::move(var2), std::move(t)));
        }
    };

    template <>
    struct action<Instruction_Var_Length_Var_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Variable> var2 = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var1 = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->blocks.back()->instructions.push_back(std::make_unique<Instruction_Var_Length_Var_Assignment>(std::move(var1), std::move(var2)));
        }
    };

    template <>
    struct action<Instruction_Var_Array_Init_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            // when constructing instructions, all parsed items belong to the current instruction
            std::unique_ptr<Instruction_Var_Array_Init> instruction = std::make_unique<Instruction_Var_Array_Init>();

            while (parsed_items.size() > 1){
                std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
                parsed_items.pop_back();
                instruction->dims.push_back(std::move(t));
            }
            std::reverse(instruction->dims.begin(), instruction->dims.end());

            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();
            instruction->var = std::move(var);

            p.functions.back()->blocks.back()->instructions.push_back(std::move(instruction));
        }
    };

    template <>
    struct action<Instruction_Var_Tuple_Init_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->blocks.back()->instructions.push_back(std::make_unique<Instruction_Var_Tuple_Init>(std::move(var), std::move(t)));
        }
    };

    template <>
    struct action<Instruction_Return_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {   
            p.functions.back()->blocks.back()->end_instruction = std::make_unique<Instruction_Return>();
        }
    };

    template <>
    struct action<Instruction_Return_T_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            p.functions.back()->blocks.back()->end_instruction = std::make_unique<Instruction_Return_T>(std::move(t));
        }
    };

    template <>
    struct action<Instruction_Br_Label_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            p.functions.back()->blocks.back()->end_instruction = std::make_unique<Instruction_Br_Label>(std::move(label));
        }
    };

    template <>
    struct action<Instruction_Br_T_Label_Label_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Label> label2 = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Label> label1 = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->blocks.back()->end_instruction = std::make_unique<Instruction_Br_T_Label_Label>(std::move(t), std::move(label1), std::move(label2));
        }
    };

    template <>
    struct action<callee_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {   
            std::string in_str = in.string();
            std::shared_ptr<Callee> callee = std::make_shared<Callee>();
            
            if (in_str == "print"){
                callee->name = "print";
            } else if (in_str == "allocate"){
                callee->name = "allocate";
            } else if (in_str == "input"){
                callee->name = "input";
            } else if (in_str == "tuple-error"){
                callee->name = "tuple-error";
            } else if (in_str == "tensor-error"){
                callee->name = "tensor-error";
            } else {
                std::shared_ptr<U> u = std::dynamic_pointer_cast<U>(parsed_items.back());
                parsed_items.pop_back();
                callee->name = "%";
                callee->u = std::move(u);
            }
            parsed_items.push_back(std::move(callee));
        }
    };

    template <>
    struct action<Instruction_Call_Function_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::unique_ptr<Instruction_Call_Function> function_call = std::make_unique<Instruction_Call_Function>();

            while (parsed_items.size() != 0)
            {
                std::shared_ptr<Callee> callee = std::dynamic_pointer_cast<Callee>(parsed_items.back());

                // callee reached
                if (callee != nullptr){
                    function_call->callee = std::move(callee);
                    parsed_items.pop_back();
                    break;
                }

                std::shared_ptr<T> arg = std::dynamic_pointer_cast<T>(parsed_items.back());
                function_call->args.push_back(std::move(arg));
                parsed_items.pop_back();
            }

            // reverse bc var order was reversed
            std::reverse(function_call->args.begin(), function_call->args.end());
            
            p.functions.back()->blocks.back()->instructions.push_back(std::move(function_call));
        }
    };

    template <>
    struct action<Instruction_Var_Function_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::unique_ptr<Instruction_Call_Function> function_call = std::unique_ptr<Instruction_Call_Function>(dynamic_cast<Instruction_Call_Function *>(p.functions.back()->blocks.back()->instructions.back().release()));
            p.functions.back()->blocks.back()->instructions.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            if (function_call == nullptr) throw std::runtime_error("Function Call cast");

            std::unique_ptr<Instruction_Var_Function_Assignment> function_assignment = std::make_unique<Instruction_Var_Function_Assignment>(std::move(var), std::move(function_call));
            p.functions.back()->blocks.back()->instructions.push_back(std::move(function_assignment));
        }
    };

    Program parse_file(char *fileName)
    {

        /*
         * Check the grammar for some possible issues.
         */
        if (pegtl::analyze<grammar>() != 0)
        {
            std::cerr << "There are problems with the grammar" << std::endl;
            exit(1);
        }

        /*
         * Parse.
         */
        file_input<> fileInput(fileName);
        Program p;
        bool success = parse<grammar, action>(fileInput, p);
        if (!success) {
            std::cerr << "Parser failed before reaching the end of the file!" << std::endl;
            exit(1);
        }

        return p;
    }

}
