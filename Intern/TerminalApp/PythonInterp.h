#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

bool callPythonInterp(const char* outputName, const char* pyPath);

void pythonCleanup(PyObject* pName, PyObject* pModule, PyObject* pFunc,
                   PyObject* pValue, PyObject* presult);

class PythonInterp {
  public:
    PythonInterp();
    PythonInterp(const char* pyPath, const char* pyName, const char* pyFunc);
    ~PythonInterp();

    void cleanup();
    void execute();
    void setPlotFileName(const char* outputName);

  private:
    const char* m_outputName;
    const char* m_pyPath;
    PyObject* m_pName;
    PyObject* m_pModule;
    PyObject* m_pDict;
    PyObject* m_pFunc = NULL;
    PyObject* m_pValue = NULL;
    PyObject* m_presult = NULL;
};