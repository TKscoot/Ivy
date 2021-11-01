#pragma once
#include <stdint.h>
#include <bitset>

class BitMask
{
public:
	BitMask() = default;
	BitMask(uint64_t defaultValue)
		: mBitmask(defaultValue)
	{
	}

	const uint64_t GetBitmask() const { return mBitmask; }

	const bool Get(uint8_t pos) const
	{
		return (mBitmask & pos) == pos;
	}

	void Set(uint8_t pos, bool value = true)
	{
		if (value)
		{
			mBitmask |= pos;
		}
		else
		{
			mBitmask ^= pos;
		}
	}

	void Reset()
	{
		mBitmask = 0;
	}

	void Flip()
	{
		mBitmask = ~mBitmask;
	}

	const int CountSetBits() const
	{
		// Ich check zwar nich, wie das funktioniert, aber es funktioniert...
		int count = mBitmask;
		count = count - ((count >> 1) & 0x55555555);				 // add pacountrs of bcountts
		count = (count & 0x33333333) + ((count >> 2) & 0x33333333);  // quads
		count = (count + (count >> 4)) & 0x0F0F0F0F;				 // groups of 8
		return (count * 0x01010101) >> 24;							 // horcountzontal sum of bytes
	}

	const bool& operator[](uint8_t pos) const
	{
		return (mBitmask & pos) == pos;
	}

	BitMask& operator |=(uint8_t pos)
	{
		mBitmask |= pos;

		return *this;
	}

	BitMask& operator &=(uint8_t pos)
	{
		mBitmask &= pos;

		return *this;
	}

	BitMask& operator ^=(uint8_t pos)
	{
		mBitmask ^= pos;

		return *this;
	}

	BitMask& operator |=(const BitMask& bitMask)
	{
		mBitmask |= bitMask.mBitmask;

		return *this;
	}

	BitMask& operator &=(const BitMask& bitMask)
	{
		mBitmask &= bitMask.mBitmask;

		return *this;
	}

	BitMask& operator ^=(const BitMask& bitMask)
	{
		mBitmask ^= bitMask.mBitmask;

		return *this;
	}

	const bool operator==(const BitMask& rhs) const
	{
		return mBitmask == rhs.mBitmask;
	}

	const bool operator!=(const BitMask& rhs) const
	{
		return mBitmask != rhs.mBitmask;
	}

	const uint64_t operator=(const BitMask& rhs) 
	{
		mBitmask = rhs.mBitmask;
		return mBitmask;
	}

	const uint64_t operator=(const uint64_t& rhs)
	{
		mBitmask = rhs;
		return mBitmask;
	}


private:
	uint64_t mBitmask = 0;
};

enum TestEnum
{
	BA = 0b00000001,
	BB = 0b00000010,
	BC = 0b00000100,
	BD = 0b00001000
};

inline void TestBitmask()
{
	std::bitset<32> bs;
	bs[5] = 3;


	BitMask b;
	BitMask b1;

	b.Set(BC);
	//b.Set(BA);
	b.Set(BD);

	bool bcGet = b.Get(BC);

	b |= BD;

	auto abc = b[2];

	int i = b.CountSetBits();

	if (b == b1)
	{
	}
}