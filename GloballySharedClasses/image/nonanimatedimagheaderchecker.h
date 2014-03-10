#ifndef NONANIMATEDIMAGHEADERCHECKER_H
#define NONANIMATEDIMAGHEADERCHECKER_H

#include <string>

class NonAnimatedImageHeaderChecker
{
public:
    //my usage of this is more strict than the guessing one below, we use it to reject image uploads
    static inline bool headerIndicatesNonAnimatedImage(char headerMagic[2])
    {
        if(isBmp(headerMagic))
        {
            return true;
        }
        if(isJpeg(headerMagic))
        {
            return true;
        }
        return false;
    }
    //my usage of this isn't strict and wants a fallback, so it's jpeg if not bmp...
    static inline std::string guessImageFormatFromHeaderMagic(char headerMagic[2])
    {
        if(isBmp(headerMagic))
            return "bmp";
        //if(isJpeg(headerMagic)) //uncomment if adding more types (WHICH!?!?!? FFFFFFF)
        //    return "jpeg";
        return "jpeg";
    }
    static inline bool isBmp(char headerMagic[2])
    {
        return (static_cast<unsigned char>(headerMagic[0]) == 'B' && static_cast<unsigned char>(headerMagic[1]) == 'M'); //42 AD //BM[P]
    }
    static inline bool isJpeg(char headerMagic[2])
    {
        return (static_cast<unsigned char>(headerMagic[0]) == 0xFF && static_cast<unsigned char>(headerMagic[1]) == 0xD8); //JPEG
    }
};

#endif // NONANIMATEDIMAGHEADERCHECKER_H
