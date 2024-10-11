import subprocess
import re

# The name of your test executable
TEST_BINARY = "../build/bin/debug/rayx-core-tst"

def list_tests():
    """Run the test binary with --gtest_list_tests to get a list of all tests."""
    result = subprocess.run([TEST_BINARY, '--gtest_list_tests'], stdout=subprocess.PIPE, text=True)
    
    # Parse the output to get the test names
    output = result.stdout
    tests = []
    current_test_suite = ""

    # Google Test outputs test suites and their test cases like so:
    # FooTest.
    #   Bar
    #   Baz
    for line in output.splitlines():
        # Test suite names end with a dot
        if line.endswith("."):
            current_test_suite = line.strip()
        # Test case names are indented
        elif line.startswith("  "):
            test_case = line.strip()
            full_test_name = f"{current_test_suite}{test_case}"
            tests.append(full_test_name)
    print(tests)
    return tests

def run_test(test_name):
    """Run a single test using the --gtest_filter flag."""
    print(f"Running test: {test_name}")
    
    result = subprocess.run([TEST_BINARY, '-x', f'--gtest_filter={test_name}'], stdout=subprocess.PIPE, text=True)
    
    # Check if the test passed or failed
    if result.returncode != 0:
        print(f"Test failed: {test_name}")
        return False
    else:
        print(f"Test passed: {test_name}")
        return True

def main():
    # Step 1: Get all tests
    tests = list_tests()

    failed_tests = []

    for test in tests:
        test_passed = run_test(test)

        if not test_passed:
            failed_tests.append(test)

    print(f"{len(failed_tests)} tests failed.")

if __name__ == "__main__":
    main()
