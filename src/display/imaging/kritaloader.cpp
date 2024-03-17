#include <display/imaging/kritaloader.h>
#include <fstream>
#include <memory>
#include <zlib.h>
#include <display/imaging/stb.h>

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

std::vector<char> loadKritaFile(const std::string &zipFile, const std::string &requestedFile)
{
	std::vector<char> uncompressedData;
	std::ifstream inFile(zipFile, std::ios::binary | std::ios::in);

	if (inFile)
	{
		bool done = false;
		while (!done)
		{
			HeaderSignature sig;
			inFile.read(sig.data, sizeof(HeaderSignature));

			if (sig.asInteger == LocalFileHeader::Signature)
			{
				LocalFileHeader localFileHeader;
				inFile.read(reinterpret_cast<char *>(&localFileHeader), sizeof(LocalFileHeader));

				uint16_t compressionMethod = *reinterpret_cast<uint16_t *>(localFileHeader.compressionMethod);
				uint16_t generalFlags = *reinterpret_cast<uint16_t *>(localFileHeader.generalFlags);
				uint32_t sizeCompressed = *reinterpret_cast<uint32_t *>(localFileHeader.sizeCompressed);
				uint32_t sizeUncompressed = *reinterpret_cast<uint32_t *>(localFileHeader.sizeUncompressed);
				uint16_t fileNameLength = *reinterpret_cast<uint16_t *>(localFileHeader.fileNameLength);
				uint16_t extraFieldLength = *reinterpret_cast<uint16_t *>(localFileHeader.extraFieldLength);

				// read filename
				std::string filename;
				filename.resize(fileNameLength);
				inFile.read(filename.data(), fileNameLength);

				auto extraFields = std::unique_ptr<char[]>(new char[extraFieldLength]);
				inFile.read(extraFields.get(), extraFieldLength);

				// only supporting uncompressed data for now
				if (requestedFile == filename)
				{
					if (compressionMethod == 8) // deflate
					{
						std::vector<char> compressedData;
						compressedData.resize(sizeCompressed);
						inFile.read(compressedData.data(), sizeCompressed);

						uncompressedData.resize(sizeUncompressed);

						z_stream inStream;
						inStream.zalloc = Z_NULL;
						inStream.zfree = Z_NULL;
						inStream.opaque = Z_NULL;
						inStream.avail_in = sizeCompressed;
						inStream.next_in = (Bytef *)compressedData.data();
						inStream.avail_out = sizeUncompressed;
						inStream.next_out = (Bytef *)uncompressedData.data();

						inflateInit2(&inStream, -8);
						inflate(&inStream, Z_NO_FLUSH);
						inflateEnd(&inStream);
					}
				}

				// if unsupported data was found, ignore it (nothing was allocated above)
				if (uncompressedData.size() == 0)
				{
					inFile.ignore(compressionMethod == 0 ? sizeUncompressed : sizeCompressed);
				}

			}
			else
			{
				// found non PK record, stop
				done = true;
			}
		}
		inFile.close();
	}
	return uncompressedData;
}
