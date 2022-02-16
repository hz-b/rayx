#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

class PythonInterp {
  public:
    PythonInterp();
    PythonInterp(const char* pyName, const char* pyFunc, const char* pyPath);
    ~PythonInterp();

    void cleanup();
    void execute();
    void setPlotFileName(const char* outputName);

  private:
    const char* m_outputName = NULL;
    const char* m_pyPath;
    const char* m_funcName;
    PyObject* m_pName;
    PyObject* m_pModule;
    PyObject* m_pDict;
    PyObject* m_pFunc = NULL;
    PyObject* m_pValue = NULL;
    PyObject* m_presult = NULL;
};