// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_UTIL_H__
#define __CORE_UTIL_H__

/// @file Util.h
/// @brief Common utilities

/// Returns the smallest of two values
template<typename T> const T& min(const T& a, const T& b);
/// Returns the biggest of two values
template<typename T> const T& max(const T& a, const T& b);

/// @brief Constructs copies of object from the source value to the destination buffer
template<typename T>
void array_fill(T* dst, const T& src, size_t count);

/// @brief Copies an array to a specified memory region
template<typename T>
void array_move(T* dst, const T* src, size_t count);

/// @brief Constructs copies of object from the source array to the destination buffer
template<typename T>
void array_copy(T* dst, const T* src, size_t count);

/// @brief Constructs a number of elements in an array
template<typename T>
void array_construct(T* arr, size_t count);

/// @brief Destructs a number of elements in an array
template<typename T>
void array_destruct(T* arr, size_t count);

/// @brief Destructs an object
template<typename T>
void destruct(T* arr);


/// Returns an invalid index for the specified type
template<typename T> T invalid();
/// Checks if the specified index is valid
template<typename T> bool is_valid(T index);
/// Checks if the specified index is invalid
template<typename T> bool is_invalid(T index);
/// Invalidates an index
template<typename T> void set_invalid(T& index);


/// Default function class for less than
template<typename T>
class Less
{
public:
    bool operator()(const T& a, const T& b) const;
};

/// Searches for the specfied value between the two iterators.
///	@return Iterator to the first element that compares to value. Returns the last iterator if no such element was found.
template<typename I, typename T> I find(I first, I last, const T& value);



#include "Util.inl"

#endif // __CORE_UTIL_H__