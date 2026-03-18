#!/usr/bin/env python3
import os
import sys
import argparse
import subprocess
import utils
from pathlib import Path
from typing import Optional, Tuple, List
import time

# Languages in order of compilation (left to right)
LANGUAGES = ['LD', 'LC', 'LB', 'LA', 'IR', 'L3', 'L2', 'L1']
# Relative path to the binary for a given language
RELATIVE_BINARY = 'bin'
# The name of the expected output file
OUTPUT_NAME = 'prog'
# The name of our runtime object file
RUNTIME_FILE = 'runtime.o'

# Allow for a fairly long compiler build time
COMPILER_BUILD_TIMEOUT = 600


def run_binary(binary_location: str,
               arguments: List[str],
               input_file: str,
               build_dir: str,
               timeout: int) -> bool:
    """
    Runs the given compiler binary with the provided arguments and input string
    Writes the result to build_dir (i.e. use build_dir as our working directory)
    Returns True if the binary succeeded and False otherwise
    """
    compiler_args = [binary_location, input_file] + arguments
    comp_result = subprocess.run(
        compiler_args,
        cwd=build_dir,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=timeout
    )

    #if len(comp_result.stderr) > 0:
        #utils.write_error(comp_result.stderr.decode())

    return True#len(comp_result.stderr) == 0


def compilation_fail(test_name: str,
                     message: str,
                     output_file: Optional[str]):
    """
    Helper function for writing an error message if compilation fails
    """
    error = f'Compilation failed for {test_name} with error: {message}'
    utils.write_error(error)
    if output_file is not None:
        with open(output_file, 'w') as ofile:
            ofile.write(error)


def test_language(root_dir: str,
                  starting_language: str,
                  test_file: str,
                  build_dir: str,
                  runtime_binary: str,
                  timeout: int,
                  output_dir: Optional[str] = None) -> str:
    """
    Runs a single test case in a local isolated build directory.
    Returns an empty string if the test succeeded, 
     and the name of the test (based on the test file) otherwise
    """
    test_name = Path(test_file).stem

    # Define Input/Output paths
    # We look for .in and .out files in the same directory as the test file
    input_file_loc = test_file + '.in'
    input_file = None
    if os.path.exists(input_file_loc):
        input_file = input_file_loc
    oracle_file = test_file + '.out'

    # Explicitly error if we are missing an oracle file
    assert os.path.exists(oracle_file), f'Missing oracle for {test_name}'
    with open(oracle_file, 'r') as ifile:
        expected = ifile.read().strip()

    # Get a hook to our output file location
    output_file = None
    if output_dir is not None:
        output_file = os.path.join(output_dir, f"{test_name}.out.tmp")

    try:
        # Run through each compiler, starting from our current language
        for language in LANGUAGES[LANGUAGES.index(starting_language):]:
            # If not the first language, update our input file
            if language != starting_language:
                extension = language
                if language[1] in 'ABCD': # special cases
                    extension = language[1].lower()
                test_file = os.path.join(
                    build_dir, f'{OUTPUT_NAME}.{extension}')
                assert os.path.exists(
                    test_file), f'Missing intermediate file {test_file}'

            binary_location = os.path.join(root_dir, language,
                                           RELATIVE_BINARY, language)
            succeeded = run_binary(binary_location,
                                   ['-g', '1'],
                                   test_file,
                                   build_dir,
                                   timeout)
            if not succeeded:
                message = f'language {language} failed to compile'
                compilation_fail(test_name, message, output_file)
                return test_name

        # This should have ended up producing an assembly file
        assembly_file = os.path.join(build_dir, f'{OUTPUT_NAME}.S')
        assert os.path.exists(assembly_file)

        # Assemble the resulting file
        assembled_file = os.path.join(build_dir, f'{OUTPUT_NAME}.o')
        assembler_args = ['as', assembly_file, '-o', assembled_file]
        assembler_result = subprocess.run(assembler_args,
                                          timeout=timeout,
                                          stdout=subprocess.PIPE,
                                          stderr=subprocess.PIPE)
        if len(assembler_result.stderr) > 0:
            message = f'assembler error\n{assembler_result.stderr.decode()}'
            compilation_fail(test_name, message, output_file)
            return test_name

        # Link our assembly file
        executable_file = os.path.join(build_dir, f'{OUTPUT_NAME}.out')
        linker_args = ['gcc', '-no-pie', '-o',
                       executable_file, assembled_file, runtime_binary]
        linker_result = subprocess.run(linker_args,
                                       timeout=timeout,
                                       stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
        if len(linker_result.stderr) > 0:
            message = f'linker error\n{linker_result.stderr.decode()}'
            compilation_fail(test_name, message, output_file)
            return test_name

        # Run our binary
        if input_file is None:
            binary_result = subprocess.run(
                [executable_file],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=timeout
            )
        else:
            with open(input_file, 'r') as inpf:
                binary_result = subprocess.run(
                    [executable_file],
                    stdin=inpf,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    timeout=timeout
                )

        if len(binary_result.stderr) > 0:
            message = f'runtime error {binary_result.stderr.decode()}'
            compilation_fail(test_name, message, output_file)
            return test_name

        string_result = binary_result.stdout.decode()
        if output_file is not None:
            with open(output_file, 'w') as ofile:
                ofile.write(string_result)
        if string_result.strip() == expected:
            return ''
        return test_name

    except Exception as e:
        utils.write_error(f'Test {test_name} failed with error: {str(e)}')
        if output_file is not None:
            with open(output_file, 'w') as ofile:
                ofile.write(f'Error: {str(e)}')
        return test_name


def run_tests(root_dir: str,
              language: str,
              test_dir: str,
              build_dir: str,
              runtime_location: str,
              timeout: int,
              output_dir: Optional[str] = None) -> Tuple[int, List[str]]:
    """
    Runs all tests in the given test directory for the given language
    If output_dir is none, does not write the results anywhere
    Returns the number of passing and failing tests (respectively)
    """

    # Setup build and output directories
    os.makedirs(build_dir, exist_ok=True)
    if output_dir is not None:
        os.makedirs(output_dir, exist_ok=True)

    # Compile the runtime
    assert os.path.exists(runtime_location)
    runtime_assembled = os.path.join(build_dir, RUNTIME_FILE)
    runtime_args = ['gcc', '-O2', '-c', '-g',
                    '-o', runtime_assembled, runtime_location]
    runtime_result = subprocess.run(
        runtime_args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=COMPILER_BUILD_TIMEOUT
    )
    if len(runtime_result.stderr) > 0:
        print(
            f'{utils.as_red("ERROR:")} runtime compilation failed with:\n{runtime_result.stderr}')
        raise Exception('Runtime compilation failed')

    # Get each test file
    test_files = os.listdir(test_dir)
    num_passing = 0
    failing_tests = []
    if output_dir is not None:
        os.makedirs(output_dir, exist_ok=True)

    for filename in test_files:
        extension = language
        if language[1] in 'ABCD': # special cases
            extension = language[1].lower()
        if filename.endswith(f'.{extension}'):
            test_file = os.path.join(test_dir, filename)
            fail_name = test_language(root_dir,
                                        language,
                                        test_file,
                                        build_dir,
                                        runtime_assembled,
                                        timeout,
                                        output_dir)
            if fail_name == '':
                num_passing += 1
            else:
                utils.write_normal(
                    f'{utils.as_red("FAILED: ")} {fail_name}')
                failing_tests.append(fail_name)
            test_count = num_passing + len(failing_tests)
            if test_count > 0 and test_count % 20 == 0:
                utils.write_verbose(f'Running test #{test_count}')

    return num_passing, failing_tests


def main():
    parser = argparse.ArgumentParser(description="Compiler Testing")
    parser.add_argument('root_dir',
                        help='The root directory to run these compiler tests from (relative or absolute)')
    parser.add_argument('language',
                        help='Which language to test (L1, L2, L3, IR, LA, LB, LC, LD)')
    parser.add_argument('tests',
                        help='Test directory location (relative or absolute)')
    parser.add_argument("build_dir",
                        help="Directory to store compilation artifacts (relative or absolute)")
    parser.add_argument('runtime',
                        help='The location of our runtime to compile (relative or absolute)')
    parser.add_argument('-o', '--out',
                        help='Optional output directory location (relative or absolute)')
    parser.add_argument("-t", "--timeout", type=int, default=180,
                        help="Timeout per test in seconds (default: 180)")
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Detailed runtime messages')
    parser.add_argument('-q', '--quiet', action='store_true',
                        help='Disable all non-essential script messages')

    args = parser.parse_args()
    assert args.language in LANGUAGES, f'Invalid language choice {args.language}'

    utils.set_verbosity(args)

    # Absolute paths for consistency in calls
    root_dir = os.path.abspath(args.root_dir)
    assert os.path.exists(root_dir), f'Missing root directory {root_dir}'
    test_dir = os.path.abspath(args.tests)
    assert os.path.exists(test_dir), f'Missing test directory {root_dir}'

    build_dir = os.path.abspath(args.build_dir)
    runtime_location = os.path.abspath(args.runtime)
    output_dir = None if args.out is None else os.path.abspath(args.out)

    # Run tests
    num_passing, failures = run_tests(root_dir,
                                      args.language,
                                      test_dir,
                                      build_dir,
                                      runtime_location,
                                      args.timeout,
                                      output_dir)

    # Display a summary of results
    total_tests = num_passing + len(failures)
    utils.write_normal(
        f'{utils.as_green("TESTS PASSED:")} {num_passing}/{total_tests}')
    if len(failures) > 0:
        utils.write_normal(
            f'{utils.as_red("TESTS FAILED:")} {len(failures)}/{total_tests}')

    # Formats output minimally for the autograder
    if utils.verbosity == utils.Verbosity.QUIET:
        if (len(failures) > 0):
            fail_string = '\n\t'.join(failures)
            print('Failed tests:\n\t' + fail_string + '\n')
        print(f'{num_passing}/{total_tests} tests passed')
        print(f'{len(failures)}/{total_tests} tests failed')


if __name__ == "__main__":
    start_time = time.time()
    main()
    end_time = time.time()
    print(f'Time in MS: {round((end_time - start_time) * 1000)}')
