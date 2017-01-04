// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILEPATH_H__
#define __FOUNDATION_FILEPATH_H__


class CORE_API FilePath
{
public:
    FilePath();
    FilePath(const char* path);
    FilePath(const std::string& path);
    ~FilePath();

    void set(const char* path);
    void set(const std::string& path);
    void clear();

    /// Joins the given path separated by a path separator to this path.
    void join(const char* path);
    /// Joins the given path separated by a path separator to this path.
    void join(const std::string& path);

    /// Converts the path to use the specified separator
    void set_separator(char c);

    std::string directory() const;
    std::string filename() const;
    std::string extension() const;

    /// Removes the extension from the path
    void trim_extension();

    /// Returns the full path
    std::string& get();

    /// Returns the full path
    const std::string& get() const;

    const char* c_str() const;

    FilePath& operator+=(const std::string& other);
    FilePath& operator+=(const char* other);
    FilePath& operator+=(const char other);

private:
    std::string _path;

};


#endif // __FOUNDATION_FILEPATH_H__
