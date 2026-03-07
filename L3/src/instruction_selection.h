#pragma once
#include <vector>  
#include <memory>  
#include <string>

namespace L3 {
    class Instruction;
    enum class ItemType;
    class Tile;
    class InstructionTree;
    class Context;
}

#include "L3.h"

namespace L3 {
    
    class Context {
      public:
        std::vector<int> instruction_ids;
        std::vector<std::unique_ptr<InstructionTree>> trees;
    
        void add(int instruction_id){
            instruction_ids.push_back(instruction_id);
        }
        bool empty() {
            return instruction_ids.empty();
        }
        void generate_trees(std::vector<std::unique_ptr<Instruction>>& instructions);
    };

    class InstructionTree {
      public:
        std::string value;
        int depth;
        ItemType type;
        std::vector<std::unique_ptr<InstructionTree>> children;
        std::vector<std::unique_ptr<Tile>> tiles;

        InstructionTree(std::string val, ItemType _type) : value(val), type(_type) {};
        void add_child(std::unique_ptr<InstructionTree> child) {
            depth = std::max(child->depth+1, depth);
            children.push_back(std::move(child));
        }
        void tile_tree();
    };

    class Tile {
        public:
            std::string instruction_translation;
    };

    class W_Assign_S_Tile: public Tile {
      public:
        W_Assign_S_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Assign_Mem_Tile: public Tile {
      public:
        W_Assign_Mem_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class Mem_Assign_S_Tile: public Tile {
      public:
        Mem_Assign_S_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    // class W_Assign_StackArg_M_Tile: Tile {
    //     static bool tileable(InstructionTree& tree);
    // };
    class W_Aop_T_Tile: public Tile {
      public:
        W_Aop_T_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Sop_Sx_Tile: public Tile {
      public:
        W_Sop_Sx_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Sop_N_Tile: public Tile {
      public:
        W_Sop_N_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class Mem_Increment_T_Tile: public Tile {
      public:
        Mem_Increment_T_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class Mem_Decrement_T_Tile: public Tile {
      public:
        Mem_Decrement_T_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Increment_Mem_Tile: public Tile {
      public:
        W_Increment_Mem_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Decrement_Mem_Tile: public Tile {
      public:
        W_Decrement_Mem_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Assign_T_Cmp_T_Tile: public Tile {
      public:
        W_Assign_T_Cmp_T_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class Cjump_T_Cmp_T_Label_Tile: public Tile {
      public:
        Cjump_T_Cmp_T_Label_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    // Label skipped
    class Goto_Label_Tile: public Tile {
      public:
        Goto_Label_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class Return_Tile: public Tile {
      public:
        Return_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    // function call skipped
    class W_Increment_Tile: public Tile {
      public:
        W_Increment_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class W_Decrement_Tile: public Tile {
      public:
        W_Decrement_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };
    class Address_Calculation_Tile: public Tile {
      public:
        Address_Calculation_Tile(InstructionTree& tree);
        static bool tileable(InstructionTree& tree);
    };


}