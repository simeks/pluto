#include "Common.h"

#include "PythonCommon.h"
#include "Types.h"


namespace
{
    // integers <= 32bits

    template<typename T>
    PyObject* long_to_python(void const* val)
    {
        return PyLong_FromLong(*((T*)val));
    }
    template<typename T>
    void long_from_python(PyObject* obj, void* val)
    {
        new (val) T((T)PyLong_AsLong(obj));
    }

    // unsigned integers <= 32bits

    template<typename T>
    PyObject* unsigned_long_to_python(void const* val)
    {
        return PyLong_FromUnsignedLong(*((T*)val));
    }
    template<typename T>
    void unsigned_long_from_python(PyObject* obj, void* val)
    {
        if (PyLong_Check(obj))
        {
            new (val) T((T)PyLong_AsUnsignedLong(obj));
        }
        else
        {
            // PyLong_AsUnsignedLong does not seem to work as well as PyLong_AsLong for non-long types
            //  so we handle them manually.
            long v = PyLong_AsLong(obj);
            if (v < 0)
            {
                PYTHON_ERROR(PyExc_OverflowError, "overflow detected");
                return;
            }
            new (val) T((T)v);
        }
    }

    // int64_t

    PyObject* int64_to_python(void const* val)
    {
        return PyLong_FromLongLong(*((int64_t*)val));
    }
    void int64_from_python(PyObject* obj, void* val)
    {
        new (val) int64_t(PyLong_AsLongLong(obj));
    }

    // uint64_t

    PyObject* uint64_to_python(void const* val)
    {
        return PyLong_FromUnsignedLongLong(*((uint64_t*)val));
    }
    void uint64_from_python(PyObject* obj, void* val)
    {
        if (PyLong_Check(obj))
        {
            new (val) uint64_t(PyLong_AsUnsignedLongLong(obj));
        }
        else
        {
            // PyLong_AsUnsignedLong does not seem to work as well as PyLong_AsLong for non-long types
            //  so we handle them manually.
            long long v = PyLong_AsLongLong(obj);
            if (v < 0)
            {
                PYTHON_ERROR(PyExc_OverflowError, "overflow detected");
                return;
            }
            new (val) uint64_t(v);
        }
    }

    // float & double

    template<typename T>
    PyObject* float_to_python(void const* val)
    {
        return PyFloat_FromDouble((double)(*((T*)val)));
    }
    template<typename T>
    void float_from_python(PyObject* obj, void* val)
    {
        new (val) T((T)PyFloat_AsDouble(obj));
    }

    // bool

    PyObject* bool_to_python(void const* val)
    {
        if (*((bool*)val))
        {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    void bool_from_python(PyObject* obj, void* val)
    {   
        if (PyBool_Check(obj))
        {
            new (val) bool(obj == Py_True);
            return;
        }
        PyErr_SetString(PyExc_ValueError, "Expected bool");
    }

    // void

    PyObject* void_to_python(void const*)
    {
        Py_RETURN_NONE;
    }
    void void_from_python(PyObject* , void* )
    {
    }

    // PyObject*

    PyObject* pyobject_to_python(void const* val)
    {
        return python::incref(*((PyObject**)val));
    }
    void pyobject_from_python(PyObject* obj, void* val)
    {
        new (val) (PyObject*)(obj);
    }

    // python::Object

    PyObject* object_to_python(void const* val)
    {
        return python::incref(((python::Object*)val)->ptr());
    }
    void object_from_python(PyObject* obj, void* val)
    {
        new (val) (python::Object)(python::Borrowed(obj));
    }

    // C-style string

    PyObject* cstring_to_python(void const* val)
    {
        return PyUnicode_FromString(*((const char**)val));
    }
    void cstring_from_python(PyObject* obj, void* val)
    {
        new (val) (const char*)(PyUnicode_AsUTF8(obj));
    }

    // std::string

    PyObject* string_to_python(void const* val)
    {
        return PyUnicode_FromString(((std::string*)val)->c_str());
    }
    void string_from_python(PyObject* obj, void* val)
    {
        new (val) std::string(PyUnicode_AsUTF8(obj));
    }

    // Tuple

    PyObject* tuple_to_python(void const* val)
    {
        return python::incref(((Tuple*)val)->ptr());
    }
    void tuple_from_python(PyObject* obj, void* val)
    {
        if (PySequence_Check(obj) || PyTuple_Check(obj))
        {
            new (val) Tuple(python::incref(obj));
            return;
        }

        if (obj == Py_None)
        {
            return;
        }

        // Special case, if we didn't receive a tuple we take our input and put it inside 
        // a new tuple

        Tuple* t = new (val) Tuple(1);
        t->set(0, python::Object(python::Borrowed(obj)));
    }

    // Dict

    PyObject* dict_to_python(void const* val)
    {
        return python::incref(((Dict*)val)->ptr());
    }
    void dict_from_python(PyObject* obj, void* val)
    {
        if (PyDict_Check(obj))
        {
            new (val) Dict(python::incref(obj));
            return;
        }
        PyErr_SetString(PyExc_ValueError, "Expected dict");
    }



    // Vec3 
    // TODO: Move this to a more appropiate place maybe?

    template<typename T>
    PyObject* vec3_to_python(void const* val)
    {
        PyObject* t = PyTuple_New(3);
        PyTuple_SetItem(t, 0, python::to_python<T>(((Vec3<T>*)val)->x));
        PyTuple_SetItem(t, 1, python::to_python<T>(((Vec3<T>*)val)->y));
        PyTuple_SetItem(t, 2, python::to_python<T>(((Vec3<T>*)val)->z));
        return t;
    }

    template<typename T>
    void vec3_from_python(PyObject* obj, void* val)
    {
        if (PySequence_Check(obj) && PySequence_Size(obj))
        {
            Vec3<T>& v = *(new (val) Vec3<T>());
            for (int i = 0; i < std::min<Py_ssize_t>(PySequence_Size(obj), 3); ++i)
            {
                v[i] = python::from_python<T>(PySequence_GetItem(obj, i));
            }
            return;
        }
        PyErr_SetString(PyExc_ValueError, "Failed to convert value to Vec3i");
    }

    void register_builtin_types()
    {
        using namespace python;

#define LONG_CONVERTER(T) type_registry::insert(typeid(T), long_to_python<T>, long_from_python<T>);
#define UNSIGNED_LONG_CONVERTER(T) type_registry::insert(typeid(T), unsigned_long_to_python<T>, unsigned_long_from_python<T>);

        LONG_CONVERTER(int8_t);
        LONG_CONVERTER(int16_t);
        LONG_CONVERTER(int32_t);

        UNSIGNED_LONG_CONVERTER(uint8_t);
        UNSIGNED_LONG_CONVERTER(uint16_t);
        UNSIGNED_LONG_CONVERTER(uint32_t);

#undef LONG_TYPE_CONVERTER
#undef UNSIGNED_LONG_TYPE_CONVERTER

        type_registry::insert(typeid(int64_t), int64_to_python, int64_from_python);
        type_registry::insert(typeid(uint64_t), uint64_to_python, uint64_from_python);

        type_registry::insert(typeid(float), float_to_python<float>, float_from_python<float>);
        type_registry::insert(typeid(double), float_to_python<double>, float_from_python<double>);

        type_registry::insert(typeid(Vec3<uint8_t>), vec3_to_python<uint8_t>, vec3_from_python<uint8_t>);
        type_registry::insert(typeid(Vec3<int>), vec3_to_python<int>, vec3_from_python<int>);
        type_registry::insert(typeid(Vec3<float>), vec3_to_python<float>, vec3_from_python<float>);
        type_registry::insert(typeid(Vec3<double>), vec3_to_python<double>, vec3_from_python<double>);

        type_registry::insert(typeid(bool), bool_to_python, bool_from_python);
        type_registry::insert(typeid(void), void_to_python, void_from_python);

        type_registry::insert(typeid(PyObject*), pyobject_to_python, pyobject_from_python);
        type_registry::insert(typeid(python::Object), object_to_python, object_from_python);

        type_registry::insert(typeid(const char*), cstring_to_python, cstring_from_python);
        type_registry::insert(typeid(std::string), string_to_python, string_from_python);

        type_registry::insert(typeid(Tuple), tuple_to_python, tuple_from_python);
        type_registry::insert(typeid(Dict), dict_to_python, dict_from_python);

    }
}


namespace python
{
    namespace type_registry
    {
        /// For std::set
        bool operator<(const Entry& e0, const Entry& e1)
        {
            return e0.cpp_type < e1.cpp_type;
        }

        std::set<Entry>& registry()
        {
            static std::set<Entry> entries;
            static bool initialized = false;
            if (!initialized)
            {
                initialized = true;
                register_builtin_types();
            }
            return entries;
        }
        void clear()
        {
            // We can't really clear the registry here as we have references to entries all over the place
            // Therefore we simply reset all entries to the initial state.

            for (auto& e : registry())
            {
                Entry& entry = const_cast<Entry&>(e);
                
                // cpp_type will not change so we just leave it as it is
                
                entry.to_python = nullptr;
                entry.from_python = nullptr;
                entry.py_type = nullptr;
            }
            register_builtin_types();
        }

        const Entry& lookup(const std::type_info& type)
        {
            return *registry().insert(Entry(type)).first;
        }

        void insert(const std::type_info& type,
                    ToPythonFunction to_fn,
                    FromPythonFunction from_fn)
        {
            // std::set iterators are constant, which is logical since it's sorted. However, we want
            //  to modify the other fields in Entry, hence the const_cast. This should be fine as long
            //  as we don't modify the field used as key; Entry::cpp_type
            Entry& e = const_cast<Entry&>(*registry().insert(Entry(type)).first);

            // This should be our first and only call for this particular type, if not, something has gone wrong.
            assert(e.to_python == nullptr);
            assert(e.from_python == nullptr);

            e.to_python = to_fn;
            e.from_python = from_fn;
        }
        void insert(const std::type_info& type,
            PyTypeObject* py_type,
            ToPythonFunction to_fn,
            FromPythonFunction from_fn)
        {
            // std::set iterators are constant, which is logical since it's sorted. However, we want
            //  to modify the other fields in Entry, hence the const_cast. This should be fine as long
            //  as we don't modify the field used as key; Entry::cpp_type
            Entry& e = const_cast<Entry&>(*registry().insert(Entry(type)).first);

            // This should be our first and only call for this particular type, if not, something has gone wrong.
            assert(e.py_type == nullptr);
            assert(e.to_python == nullptr);
            assert(e.from_python == nullptr);

            e.py_type = py_type;
            e.to_python = to_fn;
            e.from_python = from_fn;
        }
    }
}