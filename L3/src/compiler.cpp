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
  while ((opt = getopt(argc, argv, "vg:O:f:")) != -1) {
    switch (opt){

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

  auto p = L3::parse_file(argv[optind]);
  
  if (verbose) {
    std::cout << "Parsing Complete" << std::endl;
    std::cout << std::endl << p.to_string();
  }

  L3::generate_code(p);

  return 0;
}
