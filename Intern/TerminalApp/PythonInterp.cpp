#include "PythonInterp.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdexcept>

#include "Debug.h"

PythonInterp::PythonInterp() {}

/**
 * @brief
 * // https://docs.python.org/3/extending/embedding.html
 * Used to launch functions in Python 3
 * Creates a class that wrappes a python interpreter
 *
 * @param pyName Python file name
 * @param pyFunc Python function name
 * @param pyPath Python 3 Interpreter Path (if NULL, uses default system python3
 * path)
 */
PythonInterp::PythonInterp(const char* pyName, const char* pyFunc,
                           const char* pyPath)
    : m_pyPath(pyPath), m_funcName(pyFunc) {
    // Set custom python interpreter
    if (!m_pyPath) Py_SetPath((const wchar_t*)pyPath);

    // Initiliaze the Interpreter
    Py_Initialize();

    // Set module lookup dir
    PyRun_SimpleString("import os");
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(os.getcwd()+'/py/python')");

    // Python file
    m_pName = PyUnicode_DecodeFSDefault(pyName);

    // Load the module object
    m_pModule = PyImport_Import(m_pName);

    if (m_pModule == NULL) {
        // cleanup();
        PyErr_Print();
        //  Throw exception
        throw std::runtime_error("No python module was found");
    }
    // pDict is a borrowed reference
    m_pDict = PyModule_GetDict(m_pModule);

    // pFunc is also a borrowed reference
    // Python function
    m_pFunc = PyDict_GetItemString(m_pDict, pyFunc);
}

/**
 * @brief Executes the python function with the specified interpreter class.
 * @throw runtime_error if Python result is not expected
 */
void PythonInterp::execute() {
    if (m_pFunc && PyCallable_Check(m_pFunc)) {
        if (m_outputName) m_pValue = Py_BuildValue("(z)", m_outputName);
        PyErr_Print();
        RAYX_D_LOG << "Launching Python3 Interpreter!\n";
        m_presult = PyObject_CallObject(m_pFunc, m_pValue);
        PyErr_Print();
    } else {
        PyErr_Print();
        throw std::runtime_error("Error while running the python module: " +
                                 (std::string)(m_funcName));
    }

    if (PyLong_AsLong(m_presult) == 0 || !m_presult) {
        cleanup();
        throw std::runtime_error("Error while running the python module: " +
                                 (std::string)(m_funcName));
    }
    cleanup();
}

/**
 * @brief Clean up and stop Python
 *
 * @param pName
 * @param pModule
 * @param pFunc
 * @param pValue
 * @param presult
 */
void PythonInterp::cleanup() {
    // Clean up and free allocated memory
    Py_DECREF(m_pModule);
    Py_DECREF(m_pName);
    // Py_DECREF(m_pValue);
    //   Py_DECREF(m_pFunc);
    //    Py_DECREF(m_presult);

    // Finish the Python Interpreter
    Py_Finalize();
}
/**files
 * @param outputName Name of output file. Defaults to output.h5 (Plotting
 * related)
 */
void PythonInterp::setPlotFileName(const char* outputName) {
    m_outputName = outputName;
}

PythonInterp::~PythonInterp() {}
