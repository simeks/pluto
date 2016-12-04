// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_NONCOPYABLE_H__
#define __CORE_NONCOPYABLE_H__


// Non copyable base class
class CORE_API NonCopyable
{
public:
    NonCopyable() {}

private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};



#endif // __CORE_NONCOPYABLE_H__