#include <Python.h>

int *CA_LoadCells(char *pattern, int width) {
    PyObject *pModuleName;  // the name of the module 
    PyObject *pModule;      // the module object
    PyObject *pFunc;        // the callable object 
    PyObject *pWidth;       // the width parameter of the CA
    PyObject *pArgs;        // the argument list to the callable
    PyObject *pResult;      // the result of the python invocation

    char *module_name = "capattern";
    int i;

    // initialize the python interpreter
    Py_Initialize();

    // add current directory to sys.path, so that import can succeed
    // alternative approach is to export: PYTHONPATH=. when starting the executable
    // NOTE: this is somehow wrong as it replaces sys.path
    //PySys_SetPath(".");

    // import the module
    pModuleName = PyString_FromString(module_name);
    pModule = PyImport_Import(pModuleName);

    if (pModule == NULL) {
        fprintf(stderr, "error: could not import module: %s\n", module_name);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        return NULL;
    }
    
    // get a reference to the callable with the specified name
    pFunc = PyObject_GetAttrString(pModule, pattern);
    if (pFunc == NULL) {
        fprintf(stderr, "error: could not find callable: %s\n", pattern);
        return NULL;
    }

    if (!PyCallable_Check(pFunc)) {
        fprintf(stderr, "error: %s is not callable\n", pattern);
        return NULL;
    }

    // prepare the arguments for the callable
    pWidth = PyInt_FromLong(width);
    pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, pWidth);

    // invoke the callable with the arguments
    pResult = PyObject_CallObject(pFunc, pArgs);

    // check for errors
    if (PyErr_Occurred()) {
        PyErr_Print();
        return NULL;
    }

    // check whether the result is a sequence
    if (!PySequence_Check(pResult)) {
        fprintf(stderr, "error: result is not a sequence\n");
        return NULL;
    }

    // check length of returned result
    if (PySequence_Size(pResult) != width) {
       fprintf(stderr, "error: returned sequence has incorrect length\n");
    }

    // allocate cells
    int *cells = (int *) malloc(width * sizeof(int));

    // iterate over elements in sequence
    for (i=0; i<width; i++) {
        // check that the element is an integer
        PyObject *pElement;
        pElement = PySequence_GetItem(pResult, i);
        if (!PyInt_Check(pElement)) {
            fprintf(stderr, "error: element with index %d is not an integer\n", i);
            return NULL;
        }

        // check that the element is equal to one or zero
        int value = PyInt_AsLong(pElement);
        if (!((value == 0) || (value == 1))) {
            fprintf(stderr, "error: element with index %d is not 0 or 1\n", i);
            return NULL;
        }

        // get the cell
        *(cells+i) = value;
    }

    Py_Finalize();
    return cells;
}

