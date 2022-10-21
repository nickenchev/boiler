#include <SDL.h>
#include "util/filemanager.h"
#include "core/logger.h"

using namespace Boiler;

std::string FileManager::readTextFile(const std::string &filePath)
{
    std::string output;
    SDL_RWops *file = SDL_RWFromFile(filePath.c_str(), "r");

    if (file != NULL)
    {
        const Sint64 size = SDL_RWsize(file);

        char *buffer = new char[size + 1];
        char *buffOffset = buffer;
        int totalRead = 0, charsRead = 1;
        while (totalRead < size && charsRead != 0)
        {
            charsRead = SDL_RWread(file, buffer, (size - totalRead), 1);
            totalRead += charsRead;
            buffOffset += charsRead;
        }
        SDL_RWclose(file);
        buffer[size] = '\0';
        output = buffer;
		delete[] buffer;
    }

    return output;
}

std::vector<char> FileManager::readBinaryFile(const std::string &filePath)
{
	std::vector<char> output;
	SDL_RWops *file = SDL_RWFromFile(filePath.c_str(), "rb");

	if (file != nullptr)
	{
		Sint64 fileSize = SDL_RWsize(file);
		output.resize(fileSize);

		size_t objsRead = 0;
		do
		{
			objsRead = SDL_RWread(file, output.data(), fileSize, 1);
		} while (objsRead > 0);

		SDL_RWclose(file);
	}

	return output;
}
