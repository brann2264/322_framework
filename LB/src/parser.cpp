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

namespace LB
{

    /*
     * Tokens parsed
     */
    std::vector<std::shared_ptr<Item>> parsed_items;
    
    // Global stack to handle nested instruction scopes
    std::vector<std::vector<std::unique_ptr<Instruction>>*> instruction_stack;

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

    struct op_math : pegtl::sor<
                    TAO_PEGTL_STRING("+"),
                    TAO_PEGTL_STRING("-"),
                    TAO_PEGTL_STRING("*"),
                    TAO_PEGTL_STRING("<<"),
                    TAO_PEGTL_STRING(">>"),
                    TAO_PEGTL_STRING("&")>
    {
    };

    struct cmp : pegtl::sor<
                     TAO_PEGTL_STRING("<="),
                     TAO_PEGTL_STRING(">="),
                     TAO_PEGTL_STRING("<"),
                     TAO_PEGTL_STRING(">"),
                     TAO_PEGTL_STRING("=")>
    {
    };

    struct op : pegtl::sor<op_math, cmp> {};

    /*
     * Keywords.
     */
    struct str_return : TAO_PEGTL_STRING("return") {};
    struct str_arrow : TAO_PEGTL_STRING("<-") {};
    struct str_if : TAO_PEGTL_STRING("if") {};
    struct str_while : TAO_PEGTL_STRING("while") {};
    struct str_goto : TAO_PEGTL_STRING("goto") {};
    struct str_continue : TAO_PEGTL_STRING("continue") {};
    struct str_break : TAO_PEGTL_STRING("break") {};
    struct str_length : TAO_PEGTL_STRING("length") {};
    struct str_new_Array : TAO_PEGTL_STRING("new Array") {};
    struct str_new_Tuple : TAO_PEGTL_STRING("new Tuple") {};
    struct str_void : TAO_PEGTL_STRING("void") {};

    struct label : pegtl::seq<pegtl::one<':'>, name> {};

    struct number : pegtl::seq<
                        pegtl::opt<pegtl::sor<pegtl::one<'-'>, pegtl::one<'+'>>>,
                        pegtl::plus<pegtl::digit>>
    {
    };

    struct type : pegtl::sor<
                        TAO_PEGTL_STRING("tuple"),
                        TAO_PEGTL_STRING("code"),
                        pegtl::seq<TAO_PEGTL_STRING("int64"), pegtl::star<TAO_PEGTL_STRING("[]")>>
    > {};

    struct T_rule_type : pegtl::sor<type, str_void> {};

    struct t_rule : pegtl::sor<name, number> {};

    struct comment : pegtl::disable<
                         TAO_PEGTL_STRING("//"),
                         pegtl::until<pegtl::eolf>>
    {
    };

    /*
     * Separators.
     */
    struct spaces : pegtl::star<pegtl::sor<pegtl::one<' '>, pegtl::one<'\t'>>> {};
    struct seps : pegtl::star<pegtl::seq<spaces, pegtl::eol>> {};
    struct seps_with_comments : pegtl::star<
                                    pegtl::seq<
                                        spaces,
                                        pegtl::sor<pegtl::eol, comment>>>
    {
    };

    struct type_def : pegtl::seq<type, spaces, name> {};

    struct cond_rule : pegtl::seq<t_rule, spaces, cmp, spaces, t_rule> {};
    
    struct names_rule : pegtl::seq<
                            name, 
                            pegtl::star<pegtl::seq<spaces, pegtl::one<','>, spaces, name>>
                        > {};

    struct args_rule : pegtl::opt<
                           pegtl::seq<
                               t_rule,
                               pegtl::star<pegtl::seq<spaces, pegtl::one<','>, spaces, t_rule>>>>
    {
    };

    // Instructions
    struct Instruction_Type_Names_rule : pegtl::seq<type, spaces, names_rule> {};

    struct Instruction_Name_T_Assignment_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, t_rule> {};

    struct Instruction_Name_T_Op_T_Assignment_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, t_rule, spaces, op, spaces, t_rule> {};

    struct Instruction_Label_rule : label {};

    struct Instruction_If_rule : pegtl::seq<
        str_if, spaces, pegtl::one<'('>, spaces, cond_rule, spaces, pegtl::one<')'>, spaces, label, spaces, label> {};

    struct Instruction_While_rule : pegtl::seq<
        str_while, spaces, pegtl::one<'('>, spaces, cond_rule, spaces, pegtl::one<')'>, spaces, label, spaces, label> {};

    struct Instruction_Goto_rule : pegtl::seq<str_goto, spaces, label> {};

    struct Instruction_Return_rule : str_return {};

    struct Instruction_Return_T_rule : pegtl::seq<str_return, spaces, t_rule> {};

    struct Instruction_Continue_rule : str_continue {};
    struct Instruction_Break_rule : str_break {};

    struct Instruction_Name_Array_Assignment_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, name,
        pegtl::plus<pegtl::seq<TAO_PEGTL_STRING("["), spaces, t_rule, spaces, TAO_PEGTL_STRING("]")>>
    > {};

    struct Instruction_Array_T_Assignment_rule : pegtl::seq<
        name,
        pegtl::plus<pegtl::seq<TAO_PEGTL_STRING("["), spaces, t_rule, spaces, TAO_PEGTL_STRING("]")>>,
        spaces, str_arrow, spaces, t_rule
    > {};

    struct Instruction_Name_Length_Name_T_Assignment_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, str_length, spaces, name, spaces, t_rule> {};

    struct Instruction_Name_Length_Name_Assignment_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, str_length, spaces, name> {};

    struct Instruction_Call_Function_rule : pegtl::seq<
        name, spaces, pegtl::one<'('>, spaces, args_rule, spaces, pegtl::one<')'>> {};

    struct Instruction_Name_Function_Assignment_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, name, spaces, pegtl::one<'('>, spaces, args_rule, spaces, pegtl::one<')'>> {};

    struct Instruction_Name_Array_Init_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, str_new_Array, spaces, pegtl::one<'('>, spaces, args_rule, spaces, TAO_PEGTL_STRING(")")> {};

    struct Instruction_Name_Tuple_Init_rule : pegtl::seq<
        name, spaces, str_arrow, spaces, str_new_Tuple, spaces, pegtl::one<'('>, spaces, t_rule, spaces, TAO_PEGTL_STRING(")")> {};

    struct Scope_start : pegtl::one<'{'> {};
    struct Scope_end : pegtl::one<'}'> {};

    struct Instruction_rule; // Forward declare for nesting

    struct Instructions_rule : pegtl::star<
                                   pegtl::seq<seps_with_comments, Instruction_rule, seps_with_comments>> {};

    struct Instruction_Scope_rule : pegtl::seq<
        Scope_start, seps_with_comments, Instructions_rule, seps_with_comments, Scope_end> {};

    struct Instruction_rule : pegtl::sor<
                                pegtl::seq<pegtl::at<Instruction_Type_Names_rule>, Instruction_Type_Names_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_Array_Init_rule>, Instruction_Name_Array_Init_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_Tuple_Init_rule>, Instruction_Name_Tuple_Init_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_Function_Assignment_rule>, Instruction_Name_Function_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_Call_Function_rule>, Instruction_Call_Function_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_Array_Assignment_rule>, Instruction_Name_Array_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_Array_T_Assignment_rule>, Instruction_Array_T_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_Length_Name_T_Assignment_rule>, Instruction_Name_Length_Name_T_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_Length_Name_Assignment_rule>, Instruction_Name_Length_Name_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_T_Op_T_Assignment_rule>, Instruction_Name_T_Op_T_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_Name_T_Assignment_rule>, Instruction_Name_T_Assignment_rule>,
                                pegtl::seq<pegtl::at<Instruction_If_rule>, Instruction_If_rule>,
                                pegtl::seq<pegtl::at<Instruction_While_rule>, Instruction_While_rule>,
                                pegtl::seq<pegtl::at<Instruction_Goto_rule>, Instruction_Goto_rule>,
                                pegtl::seq<pegtl::at<Instruction_Return_T_rule>, Instruction_Return_T_rule>,
                                pegtl::seq<pegtl::at<Instruction_Return_rule>, Instruction_Return_rule>,
                                pegtl::seq<pegtl::at<Instruction_Continue_rule>, Instruction_Continue_rule>,
                                pegtl::seq<pegtl::at<Instruction_Break_rule>, Instruction_Break_rule>,
                                pegtl::seq<pegtl::at<Instruction_Label_rule>, Instruction_Label_rule>,
                                pegtl::seq<pegtl::at<Instruction_Scope_rule>, Instruction_Scope_rule>
                                >
    {
    };


    struct params_rule : pegtl::opt<
                           pegtl::seq<
                                type_def,
                               pegtl::star<pegtl::seq<spaces, pegtl::one<','>, spaces, type_def>>>>
    {
    };

    struct Function_Def_rule : pegtl::seq<
                                   T_rule_type, spaces, name, spaces,
                                   pegtl::one<'('>, spaces, params_rule, spaces, pegtl::one<')'>, spaces>
    {
    };

    struct Function_rule : pegtl::seq<
                               Function_Def_rule,
                               seps_with_comments,
                               Instruction_Scope_rule>
    {
    };

    struct grammar : pegtl::seq<
                         seps_with_comments,
                         pegtl::plus<Function_rule>,
                         seps_with_comments,
                         pegtl::eof>
    {
    };

    /*
     * Actions attached to grammar rules.
     */
    template <typename Rule>
    struct action : pegtl::nothing<Rule> {};

    template <>
    struct action<name> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            parsed_items.push_back(std::make_shared<Name>(in.string()));
        }
    };

    template <>
    struct action<label> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            parsed_items.push_back(std::make_shared<Label>(name->name));
        }
    };

    template <>
    struct action<number> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            parsed_items.push_back(std::make_shared<Number>(std::stoll(in.string())));
        }
    };

    template <>
    struct action<type> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {   
            std::string in_string = in.string();
            std::shared_ptr<Type> type = std::make_shared<Type>();

            if (in_string == "tuple") {
                type->name_type = EType::TUPLE;
            } else if (in_string == "code") {
                type->name_type = EType::CODE;
            } else {
                int dims = 0;
                size_t pos = 0;
                while ((pos = in_string.find("[]", pos)) != std::string::npos) {
                    dims++;
                    pos += 2;
                }
                if (dims == 0) type->name_type = EType::INT;
                else type->name_type = EType::ARRAY;
                type->array_dims = dims;
            }
            parsed_items.push_back(std::move(type));
        }
    };

    template <>
    struct action<str_void> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {   
            std::shared_ptr<Type> type = std::make_shared<Type>();
            type->name_type = EType::VOID;
            parsed_items.push_back(std::move(type));
        }
    };

    template <>
    struct action<type_def> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Type> type = std::dynamic_pointer_cast<Type>(parsed_items.back());
            parsed_items.pop_back();
            parsed_items.push_back(std::make_shared<TypeDef>(type, name));
        }
    };

    template <>
    struct action<cmp> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            EOperator op_enum;
            std::string op_str = in.string();
            if (op_str == "<") op_enum = EOperator::LT;
            else if (op_str == "<=") op_enum = EOperator::LTE;
            else if (op_str == "=") op_enum = EOperator::EQ;
            else if (op_str == ">") op_enum = EOperator::GT;
            else if (op_str == ">=") op_enum = EOperator::GTE;
            parsed_items.push_back(std::make_shared<Operator>(op_enum));
        }
    };

    template <>
    struct action<op_math> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            EOperator op_enum;
            std::string op_str = in.string();
            if (op_str == "+") op_enum = EOperator::ADD;
            else if (op_str == "-") op_enum = EOperator::SUB;
            else if (op_str == "*") op_enum = EOperator::MULT;
            else if (op_str == "&") op_enum = EOperator::AND;
            else if (op_str == "<<") op_enum = EOperator::LEFT_SHIFT;
            else if (op_str == ">>") op_enum = EOperator::RIGHT_SHIFT;
            parsed_items.push_back(std::make_shared<Operator>(op_enum));
        }
    };

    template <>
    struct action<Function_Def_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::unique_ptr<Function> f = std::make_unique<Function>();

            while (parsed_items.size() > 2) {
                std::shared_ptr<TypeDef> type_def = std::dynamic_pointer_cast<TypeDef>(parsed_items.back());
                parsed_items.pop_back();
                f->params.push_back(std::move(type_def));
            }
            std::reverse(f->params.begin(), f->params.end());

            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            f->function_name = std::move(name);

            std::shared_ptr<Type> return_type = std::dynamic_pointer_cast<Type>(parsed_items.back());
            parsed_items.pop_back();
            f->return_type = std::move(return_type);

            p.functions.push_back(std::move(f));

            // Initialize the scope stack with the outer function's instructions vector
            instruction_stack.clear();
            instruction_stack.push_back(&(p.functions.back()->instructions));
        }
    };

    template <>
    struct action<Scope_start> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            auto scope = std::make_unique<Instruction_Scope>();
            auto ptr = &(scope->instructions);
            instruction_stack.back()->push_back(std::move(scope));
            instruction_stack.push_back(ptr);
        }
    };

    template <>
    struct action<Scope_end> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            instruction_stack.pop_back();
        }
    };

    template <>
    struct action<Instruction_Label_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            instruction_stack.back()->push_back(std::make_unique<Instruction_Label>(std::move(label)));
        }
    };

    template <>
    struct action<Instruction_Type_Names_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::vector<std::shared_ptr<Name>> names;
            while(true) {
                std::shared_ptr<Name> n = std::dynamic_pointer_cast<Name>(parsed_items.back());
                if (n) {
                    names.push_back(n);
                    parsed_items.pop_back();
                } else {
                    break;
                }
            }
            std::reverse(names.begin(), names.end());
            
            std::shared_ptr<Type> type = std::dynamic_pointer_cast<Type>(parsed_items.back());
            parsed_items.pop_back();
            
            instruction_stack.back()->push_back(std::make_unique<Instruction_Type_Names>(std::move(type), std::move(names)));
        }
    };

    template <>
    struct action<Instruction_Name_T_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction_stack.back()->push_back(std::make_unique<Instruction_Name_T_Assignment>(std::move(name), std::move(t)));
        }
    };

    template <>
    struct action<Instruction_Name_T_Op_T_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<T> t2 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Operator> op = std::dynamic_pointer_cast<Operator>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t1 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction_stack.back()->push_back(std::make_unique<Instruction_Name_T_Op_T_Assignment>(std::move(name), std::move(t1), std::move(op), std::move(t2)));
        }
    };

    template <>
    struct action<Instruction_If_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Label> label_false = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Label> label_true = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t2 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Operator> op = std::dynamic_pointer_cast<Operator>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t1 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();

           instruction_stack.back()->push_back(std::make_unique<Instruction_If>(std::move(t1), std::move(op), std::move(t2), std::move(label_true), std::move(label_false)));
        }
    };

    template <>
    struct action<Instruction_While_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Label> label_false = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Label> label_true = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t2 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Operator> op = std::dynamic_pointer_cast<Operator>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t1 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();

           instruction_stack.back()->push_back(std::make_unique<Instruction_While>(std::move(t1), std::move(op), std::move(t2), std::move(label_true), std::move(label_false)));
        }
    };

    template <>
    struct action<Instruction_Goto_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            instruction_stack.back()->push_back(std::make_unique<Instruction_Goto>(std::move(label)));
        }
    };

    template <>
    struct action<Instruction_Name_Array_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::unique_ptr<Instruction_Name_Array_Assignment> instruction = std::make_unique<Instruction_Name_Array_Assignment>();

            while (parsed_items.size() > 2) {
                std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
                parsed_items.pop_back();
                instruction->idxs.push_back(std::move(t));
            }
            std::reverse(instruction->idxs.begin(), instruction->idxs.end());

            std::shared_ptr<Name> arr_name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction->arr_name = std::move(arr_name);
            instruction->name = std::move(name);

            p.functions.back()->max_length_access = std::max<int>(p.functions.back()->max_length_access, instruction->idxs.size());
            instruction_stack.back()->push_back(std::move(instruction));
            instruction_stack.back()->back()->line_number = in.position().line;
        }
    };

    template <>
    struct action<Instruction_Array_T_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();

            std::unique_ptr<Instruction_Array_T_Assignment> instruction = std::make_unique<Instruction_Array_T_Assignment>();
            instruction->t = std::move(t);

            while (parsed_items.size() > 1) {
                std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
                parsed_items.pop_back();
                instruction->idxs.push_back(std::move(t));
            }
            std::reverse(instruction->idxs.begin(), instruction->idxs.end());

            std::shared_ptr<Name> arr_name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction->arr_name = std::move(arr_name);

            p.functions.back()->max_length_access = std::max<int>(p.functions.back()->max_length_access, instruction->idxs.size());
            instruction_stack.back()->push_back(std::move(instruction));
            instruction_stack.back()->back()->line_number = in.position().line;
        }
    };

    template <>
    struct action<Instruction_Name_Length_Name_T_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name2 = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name1 = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction_stack.back()->push_back(std::make_unique<Instruction_Name_Length_Name_T_Assignment>(std::move(name1), std::move(name2), std::move(t)));
            instruction_stack.back()->back()->line_number = in.position().line;
        }
    };

    template <>
    struct action<Instruction_Name_Length_Name_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<Name> name2 = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name1 = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction_stack.back()->push_back(std::make_unique<Instruction_Name_Length_Name_Assignment>(std::move(name1), std::move(name2)));
            instruction_stack.back()->back()->line_number = in.position().line;
        }
    };

    template <>
    struct action<Instruction_Name_Array_Init_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::unique_ptr<Instruction_Name_Array_Init> instruction = std::make_unique<Instruction_Name_Array_Init>();

            while (parsed_items.size() > 1) {
                std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
                parsed_items.pop_back();
                instruction->dims.push_back(std::move(t));
            }
            std::reverse(instruction->dims.begin(), instruction->dims.end());

            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();
            instruction->name = std::move(name);

            instruction_stack.back()->push_back(std::move(instruction));
            instruction_stack.back()->back()->line_number = in.position().line;
        }
    };

    template <>
    struct action<Instruction_Name_Tuple_Init_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            instruction_stack.back()->push_back(std::make_unique<Instruction_Name_Tuple_Init>(std::move(name), std::move(t)));
            instruction_stack.back()->back()->line_number = in.position().line;
        }
    };

    template <>
    struct action<Instruction_Return_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {   
            instruction_stack.back()->push_back(std::make_unique<Instruction_Return>());
        }
    };

    template <>
    struct action<Instruction_Return_T_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            instruction_stack.back()->push_back(std::make_unique<Instruction_Return_T>(std::move(t)));
        }
    };

    template <>
    struct action<Instruction_Continue_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            instruction_stack.back()->push_back(std::make_unique<Instruction_Continue>());
        }
    };

    template <>
    struct action<Instruction_Break_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            instruction_stack.back()->push_back(std::make_unique<Instruction_Break>());
        }
    };

    template <>
    struct action<Instruction_Call_Function_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {   
            std::unique_ptr<Instruction_Call_Function> function_call = std::make_unique<Instruction_Call_Function>();

            while (parsed_items.size() != 0) {
                if (parsed_items.size() == 1) {
                    std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
                    if (name != nullptr) {
                        function_call->name = std::move(name);
                        parsed_items.pop_back();
                        break;
                    }
                }
  
                std::shared_ptr<T> arg = std::dynamic_pointer_cast<T>(parsed_items.back());
                function_call->args.push_back(std::move(arg));
                parsed_items.pop_back();
            }
            std::reverse(function_call->args.begin(), function_call->args.end());
            instruction_stack.back()->push_back(std::move(function_call));
        }
    };

    template <>
    struct action<Instruction_Name_Function_Assignment_rule> {
        template <typename Input>
        static void apply(const Input &in, Program &p) {
            std::unique_ptr<Instruction_Call_Function> function_call = std::make_unique<Instruction_Call_Function>();

            while (parsed_items.size() != 1) {
                if (parsed_items.size() == 2) {
                    std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
                    if (name != nullptr) {
                        function_call->name = std::move(name);
                        parsed_items.pop_back();
                        break;
                    }
                }
  
                std::shared_ptr<T> arg = std::dynamic_pointer_cast<T>(parsed_items.back());
                function_call->args.push_back(std::move(arg));
                parsed_items.pop_back();
            }
            std::reverse(function_call->args.begin(), function_call->args.end());

            std::shared_ptr<Name> name = std::dynamic_pointer_cast<Name>(parsed_items.back());
            parsed_items.pop_back();

            std::unique_ptr<Instruction_Name_Function_Assignment> function_assignment = std::make_unique<Instruction_Name_Function_Assignment>(std::move(name), std::move(function_call));
            instruction_stack.back()->push_back(std::move(function_assignment));
        }
    };

    Program parse_file(char *fileName)
    {
        /*
         * Check the grammar for some possible issues.
         */
        if (pegtl::analyze<grammar>() != 0) {
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
            std::cerr << "Parser failed before reaching the end of the file! (LB)" << std::endl;
            exit(1);
        }

        return p;
    }

}