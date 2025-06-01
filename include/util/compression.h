#pragma once

#include <zlib.h>
#include <cstdint>

union HeaderSignature
{
	char data[4];
	uint32_t asInteger;
};

template<int S>
struct PKStructure
{
	constexpr static uint32_t Signature = S;
};

struct LocalFileHeader : public PKStructure<0x04034b50>
{
	char version[2];
	char generalFlags[2];
	char compressionMethod[2];
	char lastModifiedTime[2];
	char lastModifiedDate[2];
	char crc32[4];
	char sizeCompressed[4];
	char sizeUncompressed[4];
	char fileNameLength[2];
	char extraFieldLength[2];
};
