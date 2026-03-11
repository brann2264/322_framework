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

namespace L3
{

    /*
     * Tokens parsed
     */
    std::vector<std::shared_ptr<Item>> parsed_items;

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

    // instructions

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

    struct Instruction_Var_T_Cmp_T_Assignment_rule : pegtl::seq<
                                                         var,
                                                         spaces,
                                                         str_arrow,
                                                         spaces,
                                                         t_rule,
                                                         spaces,
                                                         cmp,
                                                         spaces,
                                                         t_rule>
    {
    };

    struct Instruction_Var_Load_Var_Assignment_rule : pegtl::seq<
                                                          var,
                                                          spaces,
                                                          str_arrow,
                                                          spaces,
                                                          TAO_PEGTL_STRING("load"),
                                                          spaces,
                                                          var>
    {
    };

    struct Instruction_Store_Var_S_Assignment_rule : pegtl::seq<
                                                         TAO_PEGTL_STRING("store"),
                                                         spaces,
                                                         var,
                                                         spaces,
                                                         str_arrow,
                                                         spaces,
                                                         s_rule>
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

    struct Instruction_Label_rule : pegtl::seq<label>
    {
    };

    struct Instruction_Br_Label_rule : pegtl::seq<TAO_PEGTL_STRING("br"), spaces, label>
    {
    };

    struct Instruction_Br_T_Label_rule : pegtl::seq<TAO_PEGTL_STRING("br"), spaces, t_rule, spaces, label>
    {
    };

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

    struct Instruction_rule : pegtl::sor<

                                  pegtl::seq<pegtl::at<Instruction_Var_Function_Assignment_rule>, Instruction_Var_Function_Assignment_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Call_Function_rule>, Instruction_Call_Function_rule>,

                                  pegtl::seq<pegtl::at<Instruction_Br_Label_rule>, Instruction_Br_Label_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Br_T_Label_rule>, Instruction_Br_T_Label_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Label_rule>, Instruction_Label_rule>,

                                  pegtl::seq<pegtl::at<Instruction_Return_T_rule>, Instruction_Return_T_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Return_rule>, Instruction_Return_rule>,

                                  pegtl::seq<pegtl::at<Instruction_Store_Var_S_Assignment_rule>, Instruction_Store_Var_S_Assignment_rule>,

                                  pegtl::seq<pegtl::at<Instruction_Var_T_Op_T_Assignment_rule>, Instruction_Var_T_Op_T_Assignment_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Var_T_Cmp_T_Assignment_rule>, Instruction_Var_T_Cmp_T_Assignment_rule>,
                                  pegtl::seq<pegtl::at<Instruction_Var_Load_Var_Assignment_rule>, Instruction_Var_Load_Var_Assignment_rule>,
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

    struct vars_rule : pegtl::opt<
                           pegtl::seq<
                               var,
                               pegtl::star<
                                   pegtl::seq<
                                       spaces,
                                       pegtl::one<','>,
                                       spaces,
                                       var>>>>
    {
    };

    struct Function_def_rule : pegtl::seq<
                                   spaces, TAO_PEGTL_STRING("define"), spaces, function_name, spaces,
                                   pegtl::one<'('>, spaces, vars_rule, spaces, pegtl::one<')'>, spaces>
    {
    };

    struct Function_rule : pegtl::seq<
                               Function_def_rule,
                               seps_with_comments,
                               pegtl::must<pegtl::one<'{'>>,
                               seps_with_comments,
                               pegtl::must<Instructions_rule>,
                               seps_with_comments,
                               spaces,
                               pegtl::must<pegtl::one<'}'>>>
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
                         pegtl::star<Functions_rule>,
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
            } else {
                throw std::runtime_error("Unknown Operator: " + op_str);
            }

            parsed_items.push_back(std::make_shared<Operator>(op_enum));
        }
    };

    template <>
    struct action<cmp>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            EComparator cmp;
            std::string cmp_str = in.string();

            if (cmp_str == "<") {
                cmp = EComparator::LT;
            } else if (cmp_str == "<=") {
                cmp = EComparator::LTE;
            } else if (cmp_str == "=") {
                cmp = EComparator::EQ;
            } else if (cmp_str == ">") {
                cmp = EComparator::GT;
            } else if (cmp_str == ">=") {
                cmp = EComparator::GTE;
            } else {
                throw std::runtime_error("Unknown Comparator: " + cmp_str);
            }

            parsed_items.push_back(std::make_shared<Comparator>(cmp));
        }
    };

    template <>
    struct action<Function_def_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::unique_ptr<Function> f = std::make_unique<Function>();

            // based on our grammar, there can't be nested function definitions, so all parsed objects must either be a label or vars rn
            while (parsed_items.size() != 0)
            {
                // should be function name
                if (parsed_items.size() == 1)
                {
                    std::shared_ptr<FunctionName> function_name = std::dynamic_pointer_cast<FunctionName>(parsed_items.back());
                    parsed_items.pop_back();
                    if (function_name == nullptr)
                        throw std::runtime_error("Function Name not parsed properly");
                    f->function_name = std::move(function_name);
                }
                else
                {
                    std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
                    parsed_items.pop_back();
                    if (var == nullptr)
                        throw std::runtime_error("Function Parameter Vars not parsed properly");
                    f->vars.push_back(std::move(var));
                }
            }

            // reverse bc var order was reversed
            std::reverse(f->vars.begin(), f->vars.end());

            p.functions.push_back(std::move(f));
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

            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Var_S_Assignment>(std::move(var), std::move(s)));
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

            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Var_T_Op_T_Assignment>(std::move(var), std::move(t1), std::move(op), std::move(t2)));
        }
    };

    template <>
    struct action<Instruction_Var_T_Cmp_T_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<T> t2 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Comparator> cmp = std::dynamic_pointer_cast<Comparator>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t1 = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Var_T_Cmp_T_Assignment>(std::move(var), std::move(t1), std::move(cmp), std::move(t2)));
        }
    };

    template <>
    struct action<Instruction_Var_Load_Var_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Variable> var2 = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var1 = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Var_Load_Var_Assignment>(std::move(var1), std::move(var2)));
        }
    };

    template <>
    struct action<Instruction_Store_Var_S_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<S> s = std::dynamic_pointer_cast<S>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Store_Var_S_Assignment>(std::move(var), std::move(s)));
        }
    };

    template <>
    struct action<Instruction_Return_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Return>());
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
            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Return_T>(std::move(t)));
        }
    };

    template <>
    struct action<Instruction_Label_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {   
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();

            if (p.longest_label.empty() || label->label_name.length() > p.longest_label.length())
                p.longest_label = label->label_name;
            
            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Label>(std::move(label)));
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
            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Br_Label>(std::move(label)));
        }
    };

    template <>
    struct action<Instruction_Br_T_Label_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::shared_ptr<Label> label = std::dynamic_pointer_cast<Label>(parsed_items.back());
            parsed_items.pop_back();
            std::shared_ptr<T> t = std::dynamic_pointer_cast<T>(parsed_items.back());
            parsed_items.pop_back();

            p.functions.back()->instructions.push_back(std::make_unique<Instruction_Br_T_Label>(std::move(t), std::move(label)));
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
            
            p.functions.back()->instructions.push_back(std::move(function_call));
        }
    };

    template <>
    struct action<Instruction_Var_Function_Assignment_rule>
    {
        template <typename Input>
        static void apply(const Input &in, Program &p)
        {
            std::unique_ptr<Instruction_Call_Function> function_call = std::unique_ptr<Instruction_Call_Function>(dynamic_cast<Instruction_Call_Function *>(p.functions.back()->instructions.back().release()));
            p.functions.back()->instructions.pop_back();
            std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(parsed_items.back());
            parsed_items.pop_back();

            if (function_call == nullptr) throw std::runtime_error("Function Call cast");

            std::unique_ptr<Instruction_Var_Function_Assignment> function_assignment = std::make_unique<Instruction_Var_Function_Assignment>(std::move(var), std::move(function_call));
            p.functions.back()->instructions.push_back(std::move(function_assignment));
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
        parse<grammar, action>(fileInput, p);

        return p;
    }

}
