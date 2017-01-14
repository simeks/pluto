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

    template<typename T>
    T get(size_t idx) const
    {
        return python_convert::from_python<T>(get(idx));
    }

    template<typename T>
    void set(size_t idx, T value)
    {
        set(idx, python_convert::to_python(value));
    }
private:
    PyObject* _s;

};



#endif // __PYTHON_SEQUENCE_H__
