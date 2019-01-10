#ifndef NLIB_BYTE_STREAM_H
#define NLIB_BYTE_STREAM_H

#include <cstdint>
#include <cassert>

#include "NetworkDefine.h"
#include "NetworkStruct.h"
#include "ByteArray.h"

class ByteStream
{
public:
	ByteStream(const ByteArrayPtr& data, uint32_t offset);

public:
	void Write(E_PACKET_ID packet_id);
	void Write(uint32_t v);
	void Write(uint64_t v);
	void Write(const byte* data, uint32_t length);

	void PadWithZero();

	ByteArrayPtr GetData() { return _data; }
	uint32_t Length();
	uint32_t Remain();
	uint32_t Index() { return _idx; }

private:
	ByteArrayPtr _data;
	uint32_t _capacity;
	uint32_t _idx;

public:
	template<class T> bool Read(T&)
	{
		assert(false);
		return nullptr;
	}

	template<> bool Read<E_PACKET_ID>(E_PACKET_ID& val)
	{
		if (_idx + 1 > _capacity)
			return false;

		val = (E_PACKET_ID)_data->Get(_idx++);

		return true;
	}

	template<> bool Read<uint16_t>(uint16_t& val)
	{
		if (_idx + 2 > _capacity)
			return false;

		val = 0;
		val += _data->Get(_idx++) << 8 * 0;
		val += _data->Get(_idx++) << 8 * 1;

		return true;
	}

	template<> bool Read<uint32_t>(uint32_t& val)
	{
		if (_idx + 4 > _capacity)
			return false;

		val = 0;
		val += _data->Get(_idx++) << 8 * 0;
		val += _data->Get(_idx++) << 8 * 1;
		val += _data->Get(_idx++) << 8 * 2;
		val += _data->Get(_idx++) << 8 * 3;

		return true;
	}

	template<> bool Read<uint64_t>(uint64_t& val)
	{
		if (_idx + 8 > _capacity)
			return false;

		val = 0;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 0;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 1;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 2;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 3;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 4;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 5;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 6;
		val += static_cast<uint64_t>(_data->Get(_idx++)) << 8 * 7;

		return true;
	}

	template<class T> bool Read(const byte*&, uint32_t)
	{
		assert(false);
		return nullptr;
	}

	template<> bool Read<const byte*>(const byte*& val, uint32_t length)
	{
		assert(val == nullptr);
		if (val != nullptr)
			return false;

		if (_idx + length > _capacity)
			return false;

		byte* tmp = new byte[length];
		memcpy(tmp, _data->Bytes() + _idx, length);
		_idx += length;

		val = tmp;

		return true;
	}
};

#endif
