#ifndef __PYTHON_TUPLE_H__
#define __PYTHON_TUPLE_H__

class Object;

/// Wrapper for python tuples
class Tuple
{
public:
    explicit Tuple(size_t size);
    explicit Tuple(PyObject* t);
    ~Tuple();

    size_t size() const;

    PyObject* get(size_t idx) const;
    Object* get_object(size_t idx) const;

    template<typename T>
    T* get_object(size_t idx) const;

    void set(size_t idx, PyObject* obj);
    void set(size_t idx, Object* obj);

    PyObject* tuple() const;

private:
    PyObject* _t;

};

template<typename T>
T* Tuple::get_object(size_t idx) const
{
    return object_cast<T>(get_object(idx));
}


#endif // __PYTHON_TUPLE_H__
