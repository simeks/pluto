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

    bool has_key(const std::string& key) const;

    PyObject* get(const std::string& key) const;
    
    void set(const std::string& key, PyObject* item);
    void set(const std::string& key, Object* item);

    bool valid() const;

    PyObject* dict() const;

private:
    PyObject* _d;

};


#endif // __PYTHON_DICT_H__
