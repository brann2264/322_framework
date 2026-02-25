#include <string>
#include <iostream>
#include <fstream>

#include "code_generator.h"

namespace L2{
  void generate_code(Program& p){

    /* 
     * Open the output file.
     */ 
    std::ofstream outputFile;
    outputFile.open("prog.L1");
   
    /* 
     * Generate target code
     */ 
    //TODO
    p.generate_code(outputFile);
    /* 
     * Close the output file.
     */ 
    outputFile.close();
   
    return ;
  }
}
