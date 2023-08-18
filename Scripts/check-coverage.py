######################################################################
######################################################################
# HOW TO USE:

# Make sure that llvm-cov is installed (check shell command "llvm-cov --version")
# Compile RAYX in DEBUG mode with Clang (Ideally 14)
# Run this script from the root of the project
# A folder will be created in Scripts/coverage-outputs containg the coverage report
# Open index.html in your browser to view the report

######################################################################
######################################################################
import os
import datetime
import subprocess

def checkForTerminal():
    cwd = os.getcwd()
    TerminalApp_Path = "build/bin/debug/rayx-core-tst"
    test = os.path.join(cwd, TerminalApp_Path)
    return os.path.exists(test), test


def run_command(cmd, cwd=None):
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd)
    while True:
        output = proc.stdout.readline().decode()
        if output:
            print(output, end='')
        err = proc.stderr.readline().decode()
        if err:
            print(err, end='')
        if output == '' and err == '' and proc.poll() is not None:
            break
    exitcode = proc.wait()
    if exitcode != 0:
        return False, f"Command exited with code {exitcode}"
    else:
        return True, "Command executed successfully"


def execute_tests(path, env, flags=[]):
    proc = subprocess.Popen(
        [path] + flags,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        env=env
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

    return failed_tests, proc.poll()


def main():
    exists, path = checkForTerminal()
    if not (exists):
        print("Check for build!")
        return

    env = os.environ.copy()

    # Run tests with -x flag
    env["LLVM_PROFILE_FILE"] = "cpu.profraw"
    failed_tests_x, exit_code_x = execute_tests(path, env, ["-x"])

    # Run tests without -x flag
    env["LLVM_PROFILE_FILE"] = "gpu.profraw"
    failed_tests, exit_code = execute_tests(path, env)

    if failed_tests or failed_tests_x:
        print("\nThe following tests failed:")
        for failed_test in (failed_tests + failed_tests_x):
            print(failed_test)

    if exit_code != 0 or exit_code_x != 0 or failed_tests or failed_tests_x:
        print("\nSome tests did not execute successfully. Make sure that all tests are passing before analyzing coverage")
        return

    try:
        # Merge both profraw files and delete them
        success, output = run_command('llvm-profdata merge -sparse cpu.profraw gpu.profraw -o default.profdata')
        if not success:
            print("Failed to merge profraw: " + output)
            print("\nMake sure that you have selected the Clang 14 compiler in your CMake settings")
            return

        timestamp = datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
        output_dir = f'./Scripts/coverage-outputs/{timestamp}'
        
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        success, output = run_command(f'llvm-cov show -format=html -output-dir={output_dir} ./build/bin/debug/rayx-core-tst -instr-profile=default.profdata -object=./build/lib/debug/librayx-core.so')
        if not success:
            print("Failed to generate coverage: " + output)
            return
    finally:
        if os.path.exists('cpu.profraw'):
            os.remove('cpu.profraw')
        if os.path.exists('gpu.profraw'):
            os.remove('gpu.profraw')
        if os.path.exists('default.profdata'):
            os.remove('default.profdata')

    print("\nAll tasks were successfully completed!")

    return


if __name__ == "__main__":
    main()
