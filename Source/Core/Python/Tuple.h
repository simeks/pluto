#ifndef __PYTHON_TUPLE_H__
#define __PYTHON_TUPLE_H__

class Object;

/// Wrapper for python tuples
class CORE_API Tuple
{
public:
    Tuple();
    explicit Tuple(size_t size);
    explicit Tuple(PyObject* t);
    ~Tuple();

    size_t size() const;

    PyObject* get(size_t idx) const;
    void set(size_t idx, PyObject* obj);

    bool valid() const;

    PyObject* tuple() const;
    PyObject* new_reference() const;

    Tuple(const Tuple& other);
    Tuple& operator=(const Tuple& other);

    template<typename T>
    T get(size_t idx) const
    {
        return python::from_python<T>(get(idx));
    }

    template<typename T>
    void set(size_t idx, T value)
    {
        set(idx, python::to_python(value));
    }

private:
    PyObject* _t;

};



#endif // __PYTHON_TUPLE_H__
