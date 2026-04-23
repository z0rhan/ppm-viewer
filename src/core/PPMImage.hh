#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <vector>
#include <cstdint>

struct ImageData
{
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t maxColorValue;
    std::vector<uint16_t> pixelData;
};

void getImageData(const std::string& fileName, ImageData& data);

#endif // PARSER_HH
