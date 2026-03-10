#pragma once
#include <vector>
#include <memory>
#include <string>

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
  };

  class InstructionTree
  {
  public:
    std::string value;
    std::shared_ptr<Item> item;
    int depth;
    ItemType type;
    std::vector<std::unique_ptr<InstructionTree>> children;
    std::vector<std::unique_ptr<Tile>> tiles;

    InstructionTree(std::string val, ItemType _type) : value(val), type(_type) {};
    void add_child(std::unique_ptr<InstructionTree> child)
    {
      depth = std::max(child->depth + 1, depth);
      children.push_back(std::move(child));
    }
    void tile_tree();
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
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> s;
  };
  class W_Assign_Mem_Tile : public Tile
  {
  public:
    W_Assign_Mem_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> x;
  };
  class Mem_Assign_S_Tile : public Tile
  {
  public:
    Mem_Assign_S_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
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
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> op;
    std::shared_ptr<Item> t;
  };
  class W_Sop_Sx_Tile : public Tile
  {
  public:
    W_Sop_Sx_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> sop;
    std::shared_ptr<Item> sx;
  };
  class W_Sop_N_Tile : public Tile
  {
  public:
    W_Sop_N_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> sop;
    std::shared_ptr<Item> n;
  };
  class Mem_Increment_T_Tile : public Tile
  {
  public:
    Mem_Increment_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> x;
    std::shared_ptr<Item> t;
  };
  class Mem_Decrement_T_Tile : public Tile
  {
  public:
    Mem_Decrement_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> x;
    std::shared_ptr<Item> t;
  };
  class W_Increment_Mem_Tile : public Tile
  {
  public:
    W_Increment_Mem_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> x;
  };
  class W_Decrement_Mem_Tile : public Tile
  {
  public:
    W_Decrement_Mem_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> x;
  };
  class W_Assign_T_Cmp_T_Tile : public Tile
  {
  public:
    W_Assign_T_Cmp_T_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
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
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> t;
    std::shared_ptr<Item> label;
  };
  // Label skipped
  class Goto_Label_Tile : public Tile
  {
  public:
    Goto_Label_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> label;
  };
  class Return_Tile : public Tile
  {
  public:
    Return_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> t;
  };
  // function call skipped
  class W_Increment_Tile : public Tile
  {
  public:
    W_Increment_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
  };
  class W_Decrement_Tile : public Tile
  {
  public:
    W_Decrement_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
  };
  class Address_Calculation_Tile : public Tile
  {
  public:
    Address_Calculation_Tile(InstructionTree &tree);
    static bool tileable(InstructionTree &tree);
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
    static bool tileable(InstructionTree &tree);
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
    static bool tileable(InstructionTree &tree);
    void generate_code(std::ofstream &stream, Function &function_scope, Program &global_scope) const override;

    std::shared_ptr<Item> w;
    std::shared_ptr<Item> sop;
    std::shared_ptr<Item> t1;
    std::shared_ptr<Item> t2;
  };
}