#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <set>
#include <iterator>
#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>
#include "parser.h"
#include "code_generator.h"

using namespace std;

void print_help (char *progName){
  std::cerr << "Usage: " << progName << " [-v] [-g 0|1] [-O 0|1|2] [-s] [-l] [-i] SOURCE" << std::endl;
  return ;
}

int main(int argc, char **argv)
{
  auto enable_code_generator = true;
  auto spill_only = false;
  auto interference_only = false;
  auto liveness_only = false;
  auto verbose = false;
  int32_t optLevel = 3;

  /* 
   * Check the compiler arguments.
   */
  if( argc < 2 ) {
    print_help(argv[0]);
    return 1;
  }
  int32_t opt;
  int64_t functionNumber = -1;
  while ((opt = getopt(argc, argv, "vg:O:slif:")) != -1) {
    switch (opt){

      case 'l':
        liveness_only = true;
        break ;

      case 'i':
        interference_only = true;
        break ;

      case 's':
        spill_only = true;
        break ;

      case 'O':
        optLevel = strtoul(optarg, NULL, 0);
        break ;

      case 'f':
        functionNumber = strtoul(optarg, NULL, 0);
        break ;

      case 'g':
        enable_code_generator = (strtoul(optarg, NULL, 0) == 0) ? false : true ;
        break ;

      case 'v':
        verbose = true;
        break ;

      default:
        print_help(argv[0]);
        return 1;
    }
  }

  /*
   * Parse the input file.
   */
  if (spill_only){

    /* 
     * Parse an L2 function and the spill arguments.
     */
    // TODO
    auto p = L2::parse_spill(argv[optind]);

    p.spill_test(true);
 
  } else if (liveness_only){

    /*
     * Parse an L2 function.
     */
    auto p = L2::parse_liveness(argv[optind]);
    // std::cout << "liveness parsing done" << std::endl;
    p.determine_liveness(true);

    if (verbose)
      std::cout << std::endl << p.to_string();

  } else if (interference_only){

    /*
     * Parse an L2 function.
     */
    // TODO
    auto p = L2::parse_liveness(argv[optind]);
    p.determine_liveness(false);
    p.construct_graphs(true);

    if (verbose)
       std::cout << std::endl << p.to_string();

  } else {

    /* 
     * Parse the L2 program.
     */
    auto p = L2::parse_file(argv[optind]);
  
    std::cout << "parsing done" << std::endl;

    if (verbose)
      std::cout << std::endl << p.to_string();

    p.allocate_registers();
    L2::generate_code(p);


    /*
     * Check if the L2 program is correct.
     */
    // TODO
  }

  /*
   * Transform the code.
   */
  // TODO

  /*
   * Print a single L2 function case.
   */
  if (functionNumber != -1){
    // TODO

    return 0;
  }

  /*
   * Generate the target code.
   */
  if (enable_code_generator){
    // TODO
  }

  return 0;
}
