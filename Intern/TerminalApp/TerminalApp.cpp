#include "TerminalApp.h"

#include "Debug.h"

// TODO: (potential) Replace Getopt with boost(header-only)
#include <unistd.h>

#include <memory>

TerminalApp::TerminalApp() {}

TerminalApp::TerminalApp(int argc, char** argv) : m_argv(argv), m_argc(argc) {
    RAYX_D_LOG << "TerminalApp created!";
}

TerminalApp::~TerminalApp() { RAYX_D_LOG << "TerminalApp deleted!"; }

void TerminalApp::run() {
    RAYX_PROFILE_FUNCTION();

    RAYX_D_LOG << "TerminalApp running...";

    /////////////////// Argument Parser
    int c;
    while ((c = getopt(m_argc, m_argv, "pi:")) != -1) {
        switch (c) {
            case 'p':
                m_optargs.m_plotFlag = OptFlags::Enabled;
                break;
            case 'i':
                m_optargs.m_providedFile = optarg;
                break;
            case '?':
                if (optopt == 'i')
                    RAYX_ERR << "Option -" << static_cast<char>(optopt)
                             << " needs an input RML file.\n";
                else if (isprint(optopt))
                    RAYX_ERR << "Unknown option -" << static_cast<char>(optopt)
                             << ".\n\n"
                             << "Known commands:\n"
                             << "-p \t Plot output footprints and histograms.\n"
                             << "-i \t Input RML File Path.\n";
                else
                    RAYX_ERR << "Unknown option character. \n";
                break;
            default:
                abort();
        }
    }

    /////////////////// Argument treatement
    if (m_optargs.m_providedFile != NULL) {
        // load rml file
        m_Beamline = std::make_shared<RAYX::Beamline>(
            RAYX::importBeamline(m_optargs.m_providedFile));
        m_Presenter = RAYX::Presenter(m_Beamline);
    } else {
        RAYX_D_LOG << "test";
        loadDummyBeamline();
    }
    m_Presenter.run();
}

/**
 * @brief
 * // https://docs.python.org/3/extending/embedding.html
 * Launches plotter function in Python 3
 *
 * @param outputName Name of output file. Defaults to output.h5
 * @return true if successful
 * @return false if failed
 */
bool TerminalApp::callPythonInterp(const char* outputName = "output.h5") {
    // C-API to Python Standard
    // http://docs.python.org/c-api/
    PyObject *pName, *pModule, *pDict, *pFunc = NULL, *pValue = NULL,
                                       *presult = NULL;
    // Initiliaze the Interpreter
    Py_Initialize();

    // Python file name
    pName = PyUnicode_FromString((char*)"plotter");

    // Load the module object
    pModule = PyImport_Import(pName);

    if (pModule == NULL) {
        RAYX_ERR << "No python module was found. \n";
        pythonCleanup(pName, pModule, pFunc, pValue, presult);
        return false;
    }

    // pDict is a borrowed reference
    pDict = PyModule_GetDict(pModule);

    // pFunc is also a borrowed reference
    pFunc = PyDict_GetItemString(pDict, (char*)"plotOutput");

    if (pFunc && PyCallable_Check(pFunc)) {
        pValue = Py_BuildValue("(z)", outputName);
        PyErr_Print();
        RAYX_D_LOG << "Launching Python3 Interpreter!\n";
        presult = PyObject_CallObject(pFunc, pValue);
        PyErr_Print();
    } else {
        PyErr_Print();
        RAYX_ERR << "Error while running the python plot. \n";
        pythonCleanup(pName, pModule, pFunc, pValue, presult);
        return false;
    }
    if (PyLong_AsLong(presult) == 0) {
        RAYX_ERR << "Error while running the python plot. \n";
        pythonCleanup(pName, pModule, pFunc, pValue, presult);
        return false;
    }

    pythonCleanup(pName, pModule, pFunc, pValue, presult);
    return true;
}

void TerminalApp::pythonCleanup(PyObject* pName, PyObject* pModule,
                                PyObject* pFunc, PyObject* pValue,
                                PyObject* presult) {
    // Clean up
    Py_DECREF(pModule);
    Py_DECREF(pName);
    Py_DECREF(pValue);
    Py_DECREF(pFunc);
    Py_DECREF(presult);

    // Finish the Python Interpreter
    Py_Finalize();
}
