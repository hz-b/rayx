#pragma once

// On msvc
#ifdef _MSC_VER
#include <corecrt.h>
#endif
#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <PathResolver.h>

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
    PyObject* m_pFunc = nullptr;
    PyObject* m_pValue = nullptr;
    PyObject* m_presult = nullptr;
};