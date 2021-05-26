#pragma once

#include <algorithm>
#include <iostream>
#include <vector>


// WARNING: class is currently not in use - might be deleted at any point (delete this if you are using it)
class InputParser
{
public:
    InputParser(int& argc, char* argv[]);
    /// @author iain
    const std::string& getCmdOption(const std::string& option);

    bool cmdOptionExists(const std::string& option) const;

private:
    std::vector<std::string> tokens;
};