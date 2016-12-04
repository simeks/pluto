// Copyright 2008-2014 Simon Ekström

#include <Core/Common.h>

#include "../Semaphore.h"


//-------------------------------------------------------------------------------
Semaphore::Semaphore(int n) : _n(n)
{
	int res = pthread_mutex_init(&_mutex, NULL);
	assert(res == 0);
	res = pthread_cond_init(&_cond, NULL);
	assert(res == 0);
}
Semaphore::~Semaphore()
{
	pthread_cond_destroy(&_cond);
	pthread_mutex_destroy(&_mutex);
}

void Semaphore::set()
{
	int res = pthread_mutex_lock(&_mutex);
	assert(res == 0);

	++_n;

	res = pthread_cond_signal(&_cond);
	assert(res == 0);

	pthread_mutex_unlock(&_mutex);	
}

void Semaphore::wait()
{
	int res = pthread_mutex_lock(&_mutex);
	assert(res == 0);

	while(_n < 1)
	{
		res = pthread_cond_wait(&_cond, &_mutex);
		assert(res == 0);
	}
	--_n;
	pthread_mutex_unlock(&_mutex);
}
//-------------------------------------------------------------------------------



