#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "RayCore.h"

enum OptFlags { Disabled, Enabled };

class TerminalApp : public RAYX::Application {
  public:
    TerminalApp();
    TerminalApp(int argc, char** argv);
    ~TerminalApp();

    void run() override;

    bool callPythonInterp(const char* outputName);

    void pythonCleanup(PyObject* pName, PyObject* pModule, PyObject* pFunc,
                       PyObject* pValue, PyObject* presult);

    const std::string& getProvidedFilePath() const { return providedFile; };

  private:
    char** m_argv;
    int m_argc;
    std::string providedFile;

    // CLI Arguments
    // Flags initialize to DISABLED
    struct Optargs {
        OptFlags m_plotFlag = OptFlags::Disabled;  // -p (Plot)
        char* m_providedFile = NULL;               // -i (Input)
    } m_optargs;
};