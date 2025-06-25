#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <vector>

enum class PPMType
{
    None = 0,
    P3,
    P6 // will not be supported for now
};

struct ImageData
{
    unsigned int imageWidth;
    unsigned int imageHeight;
    std::vector<unsigned char> pixelData;
};

// I will not test your intelligence
bool converToPNG(const std::string& fileName);

#endif // PARSER_HH
