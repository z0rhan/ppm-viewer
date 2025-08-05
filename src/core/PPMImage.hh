#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <vector>

struct ImageData
{
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int maxColorValue;
    std::vector<unsigned int> pixelData;
    std::string exceptionMsg = "";

    inline bool isValid() const {return exceptionMsg.empty();}
};

// I will not test your intelligence
ImageData getImageData(const std::string& fileName);

#endif // PARSER_HH
