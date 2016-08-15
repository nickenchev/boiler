#include <SDL2/SDL.h>
#include "filemanager.h"

std::string FileManager::readTextFile(const std::string &filePath)
{
    std::string output;
    SDL_RWops *file = SDL_RWFromFile(filePath.c_str(), "r");

    if (file != NULL)
    {
        const int size = SDL_RWsize(file);

        char buffer[size + 1];
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
    }

    return output;
}
