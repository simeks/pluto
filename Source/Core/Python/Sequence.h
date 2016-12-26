#ifndef __PYTHON_SEQUENCE_H__
#define __PYTHON_SEQUENCE_H__

class Object;

/// Wrapper for python sequence
class CORE_API Sequence
{
public:
    Sequence();
    explicit Sequence(PyObject* s);
    ~Sequence();

    size_t size() const;

    PyObject* get(size_t idx) const;
    
    bool valid() const;

    PyObject* ptr() const;

private:
    PyObject* _s;

};



#endif // __PYTHON_SEQUENCE_H__
