#ifndef __PYTHON_DICT_H__
#define __PYTHON_DICT_H__

class Object;

/// Wrapper for python dicts
class CORE_API Dict
{
public:
    Dict();
    explicit Dict(PyObject* d);
    ~Dict();

    bool has_key(const char* key) const;

    PyObject* get(const char* key) const;
    
    void set(const char* key, PyObject* item);
    void set(const char* key, Object* item);
    void clear();

    int next(size_t* pos, PyObject** key, PyObject** value) const;

    bool valid() const;

    Dict copy() const;

    PyObject* dict() const;

    Dict(const Dict& other);
    Dict& operator=(const Dict& other);

    template<typename T>
    T get(const char* key) const
    {
        return python::from_python<T>(get(key));
    }

    template<typename T>
    void set(const char* key, T value)
    {
        set(key, python::to_python(value));
    }


private:
    PyObject* _d;

};


#endif // __PYTHON_DICT_H__
