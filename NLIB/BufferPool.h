#ifndef NLIB_BUFFER_POOL_H
#define NLIB_BUFFER_POOL_H

#include <queue>
#include <stdint.h>
#include <mutex>

#include "NetworkStruct.h"

class BufferPool
{
public:
	BufferPool();
	BufferPool(uint32_t capacity);
	~BufferPool();

	Buffer* Acquire();
	void Release(Buffer* buffer);

private:
	std::queue<Buffer*> _pool;
	std::mutex _mutex;
};

#endif
