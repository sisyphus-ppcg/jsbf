#define __STDC_WANT_LIB_EXT2__ 1 // needed for vasprintf
#define _GNU_SOURCE              // needed for memfd_create
#define PY_SSIZE_T_CLEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include "third_party/libreprl.h"

static struct reprl_context* ctx;

static PyObject *FuzzilliError;
static PyObject *pyfuzzilli_initialize(PyObject *self, PyObject *args);
static PyObject *pyfuzzilli_execute(PyObject *self, PyObject *args);

static PyMethodDef PyfuzzilliMethods[] = {
    {"initialize", pyfuzzilli_initialize, METH_VARARGS, "Initialize the engine."},
    {"execute", pyfuzzilli_execute, METH_VARARGS, "Execute some JS code."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef pyfuzzillimodule = {
    PyModuleDef_HEAD_INIT,
    "_pyfuzzilli",   /* name of module */
    NULL,           /* module documentation, may be NULL */
    -1,             /* size of per-interpreter state of the module,
                       or -1 if the module keeps state in global variables. */
    PyfuzzilliMethods
};

PyMODINIT_FUNC
PyInit__pyfuzzilli(void)
{
    PyObject *m;

    m = PyModule_Create(&pyfuzzillimodule);
    if (m == NULL) {
        return NULL;
    }

    FuzzilliError = PyErr_NewException("pyfuzzilli.error", NULL, NULL);
    Py_XINCREF(FuzzilliError);
    if (PyModule_AddObject(m, "error", FuzzilliError) < 0) {
        Py_XDECREF(FuzzilliError);
        Py_CLEAR(FuzzilliError);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

static PyObject *pyfuzzilli_initialize(PyObject *self, PyObject *args)
{
    /* pyfuzzilli.initialize(path, flags=[]) */
    char *path;
    const char **pargs;
    const char *penv[] = {NULL};
    PyObject *flagsObj;
    Py_ssize_t flagsSize;

    if (!PyArg_ParseTuple(args, "sO", &path, &flagsObj)) {
        return NULL;
    }

    ctx = reprl_create_context();

    flagsSize = PyList_Size(flagsObj);
    pargs = (const char **) malloc(sizeof(const char *) * (flagsSize + 2));
    pargs[0] = path;
    pargs[flagsSize + 1] = NULL; 

    for (Py_ssize_t i = 0; i < flagsSize; i++ ) {
        PyObject *o = PyList_GetItem(flagsObj, i);
        pargs[i+1] = PyUnicode_AsUTF8(o);
    }

    if (reprl_initialize_context(ctx, pargs, penv, 1, 1) != 0) {
      goto fail;
    }

    // Sanity check
    uint64_t exec_time;
    if (reprl_execute(ctx, "1", 1, 1000, &exec_time, 0) != 0) {
      goto fail;
    }

    free(pargs);
    Py_INCREF(Py_None);
    return Py_None;

    fail:
    PyErr_SetString(FuzzilliError, reprl_get_last_error(ctx));
    reprl_destroy_context(ctx);
    free(pargs);
    ctx = NULL;
    return NULL;
}

static PyObject *pyfuzzilli_execute(PyObject *self, PyObject *args) {
    char *code;
    Py_ssize_t codeSize;
    int returnCode;

    if (!PyArg_ParseTuple(args, "y#", &code, &codeSize)) {
        return NULL;
    }

    uint64_t exec_time;
    returnCode = reprl_execute(ctx, code, codeSize, 1000 /* SET TIMEOUT HERE */, &exec_time, 0);
    
    // Err?
    if (returnCode < 0) {
      PyErr_SetString(FuzzilliError, reprl_get_last_error(ctx));
      return NULL;
    }

    PyObject *o = PyLong_FromLong(returnCode);
    Py_INCREF(o);
    return o;
}
