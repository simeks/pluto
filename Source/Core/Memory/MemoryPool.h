// Copyright 2008-2014 Simon Ekström

#ifndef __CORE_MEMORYPOOL_H__
#define __CORE_MEMORYPOOL_H__

#include "Thread/Thread.h"


/// @brief Thread-safe memory pool for a specified type
/// 
/// This is not really an allocator in the same way as for example HeapAllocator
///	as it doesn't allocate raw memory, it's just a memory pool for objects. The objects
///	are not destroyed when released, therefore the pool handle all objects as POD-objects
template<typename T, int BLOCK_SIZE>
class MemoryPool : public NonCopyable
{
public:
	/// Constructor
	///	@param backing Backing allocator
	MemoryPool(Allocator& backing = memory::default_allocator());
	~MemoryPool();

	/// @brief Allocates an object
	///	@return Returns NULL if pool is full
	T* allocate();

	/// @brief Releases an object
	void release(T* obj);

	/// @brief Returns the index in this pool for the specified object
	///	@return Object index, or INVALID_INDEX if index is invalid
	uint32_t index(T* obj);

	/// @brief Returns the object at the specified index
	///	@returns Object at the index or NULL if object doesn't exist in pool
	T* object(uint32_t index);

private:
	struct Block
	{
		/// Object buffer
		T buffer[BLOCK_SIZE];

		/// Next block in the pool
		Block* next;
	};
	Block* _head_block;
	uint32_t _block_count; ///< Number of allocated blocks

	T** _free_objects; ///< List of free object slots
	uint32_t _free_object_count;

	CriticalSection _lock;
	Allocator& _backing;

	/// @brief Allocates a new block for objects
	///	@remark This is not thread-safe so it's assumed that any locks needed are in place
    void allocate_block();
};


//-------------------------------------------------------------------------------
template<typename T, int BLOCK_SIZE>
MemoryPool<T, BLOCK_SIZE>::MemoryPool(Allocator& backing)
	: _head_block(nullptr),
	_block_count(0),
	_free_objects(nullptr),
	_free_object_count(0),
	_backing(backing)
{
	allocate_block();
}
template<typename T, int BLOCK_SIZE>
MemoryPool<T, BLOCK_SIZE>::~MemoryPool()
{
	Block* block = 0;
	Block* next_block = _head_block;
	while (next_block)
	{
		block = next_block;
		next_block = block->next;

		_backing.free(block);
	}
	_backing.free(_free_objects);
}
//-------------------------------------------------------------------------------
template<typename T, int BLOCK_SIZE>
T* MemoryPool<T, BLOCK_SIZE>::allocate()
{
	ScopedLock<CriticalSection> scoped_lock(_lock);

	if (_free_object_count > 0) // Check if pool is full
	{
		T* object = _free_objects[--_free_object_count];

		return object;
	}

	allocate_block();

	assert(_free_object_count > 0);
	T* object = _free_objects[--_free_object_count];

	return object;
}
template<typename T, int BLOCK_SIZE>
void MemoryPool<T, BLOCK_SIZE>::release(T* obj)
{
	assert(obj);
	ScopedLock<CriticalSection> scoped_lock(_lock);

	_free_objects[_free_object_count] = obj;
	++_free_object_count;
}
template<typename T, int BLOCK_SIZE>
uint32_t MemoryPool<T, BLOCK_SIZE>::index(T* obj)
{
	ScopedLock<CriticalSection> scoped_lock(_lock);

	uint32_t index = 0;
	for (Block* block = _head_block; block != nullptr; block = block->next)
	{
		T* objects = block->buffer;
		// Is the object in this block?
		if (obj >= objects && obj < objects + BLOCK_SIZE)
		{
			return index + (uint32_t)(obj - objects);
		}
		index += BLOCK_SIZE;
	}
	return invalid<uint32_t>();
}
template<typename T, int BLOCK_SIZE>
T* MemoryPool<T, BLOCK_SIZE>::object(uint32_t index)
{
	assert(is_valid(index));

	ScopedLock<CriticalSection> scoped_lock(_lock);
	for (Block* block = _head_block; block != nullptr; block = block->next)
	{
		// Is the index in this block?
		if (index < BLOCK_SIZE)
		{
			return block->buffer + index;
		}
		index -= BLOCK_SIZE;
	}
	return nullptr;
}
template<typename T, int BLOCK_SIZE>
void MemoryPool<T, BLOCK_SIZE>::allocate_block()
{
	assert(_free_object_count == 0);

	_block_count++;

	// Put the Block object at the end of the buffer
	Block* block = (Block*)_backing.allocate(sizeof(Block));
	block->next = nullptr;

	if (_head_block == nullptr)
	{
		_head_block = block;
	}
	else
	{
		// Put the new block at the end of the list
		Block* tmp = _head_block;
		while (tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = block;
	}

	_free_objects = (T**)_backing.reallocate(_free_objects, sizeof(T*)* _block_count * BLOCK_SIZE);
	_free_object_count = BLOCK_SIZE;

	T* objects = block->buffer;

	// Add new block to free objects
	for (uint32_t i = BLOCK_SIZE; i > 0; --i)
	{
		_free_objects[i-1] = objects;
		++objects;
	}

}
//-------------------------------------------------------------------------------


#endif // __CORE_MEMORYPOOL_H__

