#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string>

class PythonInterp {
  public:
    PythonInterp();
    PythonInterp(const char* pyName, const char* pyFunc, const char* pyPath);
    ~PythonInterp();

    void cleanup();
    void execute();
    void setPlotName(const char* outputName);
    void setPlotType(int plotType);

  private:
    std::string m_outputName;  // Stored as string for better parsing
    int m_plotType = 1;        // 1 -> RAY-UI Like(default)
    const char* m_pyPath;
    const char* m_funcName;
    PyObject* m_pName;
    PyObject* m_pModule;
    PyObject* m_pDict;
    PyObject* m_pFunc = NULL;
    PyObject* m_pValue = NULL;
    PyObject* m_presult = NULL;
};