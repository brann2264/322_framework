#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

namespace L3
{
  class Instruction;
  enum class ItemType;
  class Tile;
  class InstructionTree;
  class Context;
  class Item;
  class Function;
  class Program;
}

#include "L3.h"

namespace L3
{

  class Context
  {
  public:
    int start_idx = -1;
    int end_idx = -1;
    std::vector<int> instruction_ids;
    std::vector<std::unique_ptr<InstructionTree>> trees;
    std::unordered_set<std::string> end_contexts_vars;

    void add(int instruction_id)
    {
      if (start_idx == -1)
        start_idx = instruction_id;
      end_idx = std::max(end_idx, instruction_id);
      instruction_ids.push_back(instruction_id);
    }
    bool empty()
    {
      return instruction_ids.empty();
    }
    void generate_trees(std::vector<std::unique_ptr<Instruction>> &instructions);
    void merge_trees();
  };

  class InstructionTree
  {
  public:
    std::string value;
    std::shared_ptr<Item> item;
    ItemType type;
    std::vector<std::unique_ptr<InstructionTree>> children;
    std::vector<std::unique_ptr<Tile>> tiles;

    bool isWrite = false;
    bool isRead = true;
    bool isVar;

    InstructionTree(std::string val, ItemType _type) : value(val), type(_type), isVar(val[0]=='%') {};

    std::unordered_set<std::string> getWriteVars();
    std::unordered_set<std::string> getReadVars();

    void add_child(std::unique_ptr<InstructionTree> child)
    {
      children.push_back(std::move(child));
    }
    void tile_tree(InstructionTree& tree_root);
    void get_leaves(std::vector<InstructionTree*>& leaves){
      if (children.size() == 0){
        leaves.push_back(this);
        return;
      }

      for (auto& child : children){
        child->get_leaves(leaves);
      }
    }

    bool merge_tree(std::unique_ptr<InstructionTree>& other_tree){
      
      for (int i = 0; i < children.size(); i++){

        if (children[i]->children.size() != 0){
          if (children[i]->merge_tree(other_tree)){
            return true;
          }
        } else if (children[i]->value == other_tree->value) {
          other_tree->isRead = true;
          children[i] = std::move(other_tree);
          return true;
        }
      }
      return false;
    }

    // ai code to help visualize tree
    void print(std::string prefix, bool isLast) const {

      std::cout << prefix;
      std::cout << (isLast ? "└── " : "├── ");
      
      std::cout << this->value << std::endl;

      std::string next_prefix = prefix + (isLast ? "    " : "│   ");

      for (size_t i = 0; i < this->children.size(); ++i) {
          bool child_is_last = (i == this->children.size() - 1);
          
          if (this->children[i] != nullptr) {
              this->children[i]->print(next_prefix, child_is_last);
          }
      }
  }

  };

  class Tile
  {
  public:
    virtual ~Tile() = default;
    virtual void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const = 0;
  };

  class W_Assign_S_Tile : public Tile
  {
  public:
    W_Assign_S_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> s;
  };
  // class W_Assign_W_Assign_S_Tile : public Tile
  // {
  // public:
  //   W_Assign_W_Assign_S_Tile(InstructionTree &tree);
  //   static bool tileable(InstructionTree &tree, InstructionTree& root);
  //   void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

  //   std::shared_ptr<Item> w;
  //   std::shared_ptr<Item> s;
  // };
  class W_Assign_Mem_Tile : public Tile
  {
  public:
    W_Assign_Mem_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> x;
  };
  class Mem_Assign_S_Tile : public Tile
  {
  public:
    Mem_Assign_S_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> x;
    std::shared_ptr<Item> s;
  };
  // class W_Assign_StackArg_M_Tile: Tile {
  //     static bool tileable(InstructionTree& tree);
  //    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;
  // };
  class W_Aop_T_Tile : public Tile
  {
  public:
    W_Aop_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> op;
    std::shared_ptr<Item> t;
  };
  class W_Sop_Sx_Tile : public Tile
  {
  public:
    W_Sop_Sx_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> sop;
    std::shared_ptr<Item> sx;
  };
  class W_Sop_N_Tile : public Tile
  {
  public:
    W_Sop_N_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> sop;
    std::shared_ptr<Item> n;
  };
  class Mem_Increment_T_Tile : public Tile
  {
  public:
    Mem_Increment_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> x;
    std::shared_ptr<Item> t;
  };
  class Mem_Decrement_T_Tile : public Tile
  {
  public:
    Mem_Decrement_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> x;
    std::shared_ptr<Item> t;
  };
  class W_Increment_Mem_Tile : public Tile
  {
  public:
    W_Increment_Mem_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> x;
  };
  class W_Decrement_Mem_Tile : public Tile
  {
  public:
    W_Decrement_Mem_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> x;
  };
  class W_Assign_T_Cmp_T_Tile : public Tile
  {
  public:
    W_Assign_T_Cmp_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> t1;
    std::shared_ptr<Item> cmp;
    std::shared_ptr<Item> t2;
  };
  class Cjump_T_Label_Tile : public Tile
  {
  public:
    Cjump_T_Label_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> t;
    std::shared_ptr<Item> label;
  };
  // Label skipped
  class Goto_Label_Tile : public Tile
  {
  public:
    Goto_Label_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> label;
  };
  class Return_Tile : public Tile
  {
  public:
    Return_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> t;
  };
  // function call skipped
  class W_Increment_Tile : public Tile
  {
  public:
    W_Increment_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
  };
  class W_Decrement_Tile : public Tile
  {
  public:
    W_Decrement_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
  };
  class Address_Calculation_Tile : public Tile
  {
  public:
    Address_Calculation_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w1;
    std::shared_ptr<Item> w2;
    std::shared_ptr<Item> w3;
    std::shared_ptr<Item> E;
  };

  class W_Assign_T_Aop_T_Tile : public Tile
  {
  public:
    W_Assign_T_Aop_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> t1;
    std::shared_ptr<Item> aop;
    std::shared_ptr<Item> t2;
  };

  class W_Assign_T_Sop_T_Tile : public Tile
  {
  public:
    W_Assign_T_Sop_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree, InstructionTree& root);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> sop;
    std::shared_ptr<Item> t1;
    std::shared_ptr<Item> t2;
  };
}