#ifndef __PYTHON_DICT_H__
#define __PYTHON_DICT_H__

class Object;

/// Wrapper for python dicts
class Dict
{
public:
    Dict();
    explicit Dict(PyObject* d);
    ~Dict();

    bool has_key(const std::string& key) const;

    PyObject* get(const std::string& key) const;
    Object* get_object(const std::string& key) const;

    template<typename T>
    T* get_object(const std::string& key) const;

    void set(const std::string& key, PyObject* item);
    void set(const std::string& key, Object* item);

    bool valid() const;

    PyObject* dict() const;

private:
    PyObject* _d;

};

template<typename T>
T* Dict::get_object(const std::string& key) const
{
    return object_cast<T>(get_object(key));
}


#endif // __PYTHON_DICT_H__
