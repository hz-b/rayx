######################################################################
######################################################################
# HOW TO USE:

# Compile RAYX in DEBUG mode with Clang (Ideally 14)
# Run this script from the root of the project
# A folder will be created in Scripts/coverage-outputs containg the coverage report
# Open index.html in your browser to view the report

######################################################################
######################################################################
import os
import datetime
import subprocess


# Only execute inside IDE with rayx as root
def checkForTerminal():
    cwd = os.getcwd()
    TerminalApp_Path = "build/bin/debug/rayx-core-tst"
    test = os.path.join(cwd, TerminalApp_Path)
    return os.path.exists(test), test


def run_command(cmd, cwd=None):
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd)

    # While the process is running, read lines from stdout/stderr and print them
    while True:
        # Read a line from stdout
        output = proc.stdout.readline().decode()
        if output:
            print(output, end='')

        # Read a line from stderr
        err = proc.stderr.readline().decode()
        if err:
            print(err, end='')

        # If both stdout and stderr were empty and the process has ended, break the loop
        if output == '' and err == '' and proc.poll() is not None:
            break

    # Wait for the process to finish, and get the final exit code
    exitcode = proc.wait()

    if exitcode != 0:
        return False, f"Command exited with code {exitcode}"
    else:
        return True, "Command executed successfully"


def main():
    exists, path = checkForTerminal()
    if not (exists):
        print("Check for build!")
        return
    
    
    proc = subprocess.Popen(
        [path, "-x"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )

    failed_tests = []
    current_test = ""
    
    while True:
        output = proc.stdout.readline().decode()
        if output == '' and proc.poll() is not None:
            break
        if output:
            if '[ RUN      ]' in output:
                current_test = output.strip()
            if 'Failure' in output:
                failed_tests.append(current_test)

    if failed_tests:
        print("\nThe following tests failed:")
        for failed_test in failed_tests:
            print(failed_test)

    if proc.poll() != 0 or failed_tests:  # if the subprocess didn't exit correctly or if there were failed tests
        print("\nSome tests did not execute successfully. Make sure that all tests are passing before analyzing coverage")
        return

    try:
        # Merge and delete default.profraw
        success, output = run_command('llvm-profdata merge -sparse default.profraw -o default.profdata')
        if not success:
            print("Failed to merge profraw: " + output)
            return
        

        # Define output directory with timestamp
        timestamp = datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
        output_dir = f'./Scripts/coverage-outputs/{timestamp}'
        
        # If the directory doesn't exist, create it
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # Generate coverage and delete default.profdata
        success, output = run_command(f'llvm-cov show -format=html -output-dir={output_dir} ./build/bin/debug/rayx-core-tst -instr-profile=default.profdata -object=./build/lib/debug/librayx-core.so')
        if not success:
            print("Failed to generate coverage: " + output)
            return
    finally:
        if os.path.exists('default.profraw'):
            os.remove('default.profraw')
        if os.path.exists('default.profdata'):
            os.remove('default.profdata')

    print("\nAll tasks were successfully completed!")

    return


# main
if __name__ == "__main__":
    main()


