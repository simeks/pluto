// Copyright 2008-2014 Simon Ekström

#include <Core/Common.h>

#include "../Lock.h"


//-------------------------------------------------------------------------------
Mutex::Mutex()
{
	pthread_mutexattr_t attr;
	int res = pthread_mutexattr_init(&attr);
	assert(res == 0);

	res = pthread_mutex_init(&_mutex, &attr);
	assert(res == 0);
	
	res = pthread_mutexattr_destroy(&attr);
	assert(res == 0);
}
Mutex::~Mutex()
{
	pthread_mutex_destroy(&_mutex);
}

void Mutex::lock()
{
	pthread_mutex_lock(&_mutex);
}

void Mutex::unlock()
{
	pthread_mutex_unlock(&_mutex);
}

bool Mutex::try_lock()
{
	const int rc = pthread_mutex_trylock(&_mutex);
	if (rc == 0)
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------



