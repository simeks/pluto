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
    void set(size_t idx, Object* obj);

    bool valid() const;

    PyObject* tuple() const;

private:
    PyObject* _t;

};



#endif // __PYTHON_TUPLE_H__
