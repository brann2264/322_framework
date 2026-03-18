import argparse
from typing import Optional
from enum import Enum

# ANSI Colors
RED = "\033[91m"
GREEN = "\033[92m"
RESET = "\033[0m"

def TODO(message: str = ''):
    print(f'{RED}TODO:{RESET} {message}')
    raise NotImplementedError()

def as_red(message: str = ''):
    return f'{RED}{message}{RESET}'

def as_green(message: str = ''):
    return f'{GREEN}{message}{RESET}'

class Verbosity(Enum):
    QUIET = 1
    NORMAL = 2
    VERBOSE = 3

verbosity : Verbosity = Verbosity.NORMAL
    
def set_verbosity(args: argparse.Namespace) -> Verbosity:
    """
    Interprets a level of verbosity from the given args
        and reads it into the global "verbosity"
    assumes --verbose and --quiet as verbosity arguments, 
        where verbose takes precedence
    """
    global verbosity
    if args.verbose:
        verbosity = Verbosity.VERBOSE 
    elif args.quiet:
        verbosity = Verbosity.QUIET
    else:
        verbosity = Verbosity.NORMAL

def write_error(message: str) -> bool:
    """
    Writes the given error to the console if we are not quiet
    Returns whether or not anything was written
    """
    if verbosity == Verbosity.NORMAL or verbosity == Verbosity.VERBOSE:
        print(f'{RED}ERROR:{RESET} {message}')
        return True
    return False

def write_verbose(message: str):
    """
    Writes the message to the console if we are verbose
    Returns whether or not anything was written
    """
    if verbosity == Verbosity.VERBOSE:
        print(f'{message}')
        return True
    return False

def write_normal(message: str):
    """
    Writes the message to the console if we are not quiet
    Returns whether or not anything was written
    """
    if verbosity == Verbosity.NORMAL or verbosity == Verbosity.VERBOSE:
        print(f'{message}')
        return True
    return False

def save(message: str, out_file: Optional[str]):
    """
    Helper function to write to the output file if defined
    """
    if out_file is not None:
        with open(out_file, 'w') as ofile:
            ofile.write(str(message))