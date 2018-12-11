#include "BufferPool.h"

#include <assert.h>

BufferPool::BufferPool()
	: BufferPool(128)
{

}

BufferPool::BufferPool(uint32_t capacity)
{
	assert(capacity > 0);

	for (int i = 0; i < capacity; ++i)
	{
		_pool.push(new Buffer());
	}
}

BufferPool::~BufferPool()
{
	while (!_pool.empty())
	{
		delete _pool.front();
		_pool.pop();
	}
}

// TODO Release �ȵǴ� �͵��� ������ ���?
Buffer* BufferPool::Acquire()
{
	_mutex.lock();
	if (_pool.empty())
	{
		_mutex.unlock();
		return new Buffer();
	}
	auto buffer = _pool.front();
	_pool.pop();
	_mutex.unlock();

	return buffer;
}

void BufferPool::Release(Buffer* buffer)
{
	_mutex.lock();
	_pool.push(buffer);
	_mutex.unlock();
}