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
 * @param pyPath Python 3 Interpreter Path (if NULL, uses default system python3
 * path)
 * @param pyName Python file name
 * @param pyFunc Python function name
 */
PythonInterp::PythonInterp(const char* pyName, const char* pyFunc,
                           const char* pyPath = NULL)
    : m_pyPath(pyPath) {
    // Set custom python interpreter
    if (!m_pyPath) Py_SetPath((const wchar_t*)pyPath);

    // Initiliaze the Interpreter
    Py_Initialize();

    // Python file
    m_pName = PyUnicode_FromString(pyName);

    // Load the module object
    m_pModule = PyImport_Import(m_pName);

    if (m_pModule == NULL) {
        RAYX_ERR << "No python module was found. \n";
        cleanup();
        // Throw exception
        throw std::runtime_error("No python module was found.");
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
        m_pValue = Py_BuildValue("(z)", m_outputName);
        PyErr_Print();
        RAYX_D_LOG << "Launching Python3 Interpreter!\n";
        RAYX_D_LOG << Py_GetVersion() << "\n";
        m_presult = PyObject_CallObject(m_pFunc, m_pValue);
        PyErr_Print();
    } else {
        PyErr_Print();
        RAYX_ERR << "Error while running the python plot. \n";
        cleanup();
        throw std::runtime_error("Error while running the python plot");
    }

    if (PyLong_AsLong(m_presult) == 0) {
        RAYX_ERR << "Error while running the python plot. \n";
        cleanup();
        throw std::runtime_error("Error while running the python plot");
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
    Py_DECREF(m_pValue);
    Py_DECREF(m_pFunc);
    Py_DECREF(m_presult);

    // Finish the Python Interpreter
    Py_Finalize();
}
/**
 * @param outputName Name of output file. Defaults to output.h5 (Plotting
 * related)
 */
void PythonInterp::setPlotFileName(
    const char* outputName = (const char*)"output.h5") {
    m_outputName = outputName;
}
