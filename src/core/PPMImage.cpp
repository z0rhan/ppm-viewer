#include "PPMImage.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <optional>
#include <cstdint>

static constexpr char s_commentChar = '#';
static const std::string s_PPMextension = ".ppm";

enum class PPMType
{
    None = 0,
    P3,
    P6 // will not be supported for now
};

//------------------------------------------------------------------------------
void parseP3Data(const std::string& fileName, ImageData& data);

void parseP6Data(const std::string& fileName, ImageData& data);

// Helpers
PPMType parseMagicNumber(const std::string& fileName);

bool hasPPMextension(const std::string& fileName);

std::string& stripComment(std::string& line, const char commentChar);

std::vector<std::string> getLines(const std::string& fileName);

std::optional<std::pair<unsigned int, unsigned int>>
parseDimensions(const std::string& line);

std::optional<unsigned int>
parseMaxColorValue(const std::string& line);

std::optional<std::vector<unsigned int>>
parsePixelData(const std::vector<std::string>& lines, size_t startLine,
               unsigned int width, unsigned int height, unsigned int maxColorVal,
               std::string& errorMsg);

//------------------------------------------------------------------------------
void getImageData(const std::string& fileName, ImageData& data)
{
    PPMType type = parseMagicNumber(fileName);
    switch (type)
    {
        case PPMType::P3:
            parseP3Data(fileName, data);
            break;

        case PPMType::P6:
            data.exceptionMsg = "Not implemented now";
            //parseP6Data(fileName, data);
            break;

        case PPMType::None:
            data.exceptionMsg = "Invalid file format";
            break;
    }
}

//------------------------------------------------------------------------------
bool hasPPMextension(const std::string& fileName)
{
    if (fileName.length() > s_PPMextension.length())
    {
        return fileName.compare(fileName.length() - s_PPMextension.length(),
                                s_PPMextension.length(), s_PPMextension) == 0;
    }

    return false;
}

PPMType parseMagicNumber(const std::string& fileName)
{
    if (!hasPPMextension(fileName))
    {
        throw std::runtime_error("Invalid file: " + fileName);
    }

    std::ifstream fileObj(fileName, std::ios::in | std::ios::binary);
    if (!fileObj)
    {
        throw std::runtime_error(fileName + " could not be opened");
    }
    char headerBuf[3]; // 2 for magic number + 1 for null terminator
    fileObj.read(headerBuf, 2);
    fileObj.close();

    std::string header(headerBuf);

    if (header == "P3")
    {
        return PPMType::P3;
    }
    else if (header == "P6")
    {
        return PPMType::P6;
    }

    return PPMType::None;
}

//------------------------------------------------------------------------------
std::string& stripComment(std::string& line, const char commentChar)
{
    if (!line.empty() && line[0] == commentChar)
    {
        line.clear();
        return line;
    }

    size_t hashPos = line.find(commentChar);
    if (hashPos != std::string::npos)
    {
        line = line.substr(0, hashPos);
    }
    return line;
}

std::vector<std::string> getLines(const std::string& fileName)
{
    std::ifstream fileObj(fileName, std::ios::in);
    if (!fileObj)
    {
        throw std::runtime_error(fileName + " could not be opened");
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(fileObj, line))
    {
        stripComment(line, s_commentChar);

        lines.emplace_back(std::move(line));
    }

    return lines;
}

std::optional<std::pair<uint32_t, uint32_t>>
parseDimensions(const std::string& line)
{
    std::istringstream dimensionStream(line);
    uint32_t width, height;

    if (dimensionStream >> width >> height)
    {
        return std::make_pair(width, height);
    }

    return std::nullopt;
}

std::optional<uint32_t> parseMaxColorValue(const std::string& line)
{
    std::istringstream maxColorStream(line);
    uint32_t maxColorVal;

    if (maxColorStream >> maxColorVal)
    {
        return maxColorVal;
    }

    return std::nullopt;
}

std::optional<std::vector<uint32_t>>
parsePixelData(const std::vector<std::string>& lines, size_t startLine,
               uint32_t width, uint32_t height, uint32_t maxColorVal,
               std::string& errorMsg)
{
    std::vector<uint32_t> pixels;
    uint32_t pixel;
    uint32_t channelCount = 0;

    if (maxColorVal <= 255)
    {
        pixels.reserve(width * height);
        for (size_t it = startLine; it < lines.size(); it++)
        {
            uint32_t value;
            std::vector<uint32_t> buffer;

            std::istringstream iss(lines[it]);
            while (iss >> value)
            {
                buffer.emplace_back(value);
                if (buffer.size() == 3)
                {
                    uint32_t r = buffer[0];
                    uint32_t g = buffer[1];
                    uint32_t b = buffer[2];
                    buffer.clear();

                    pixel = (r << 16) | (g << 8 ) | b;
                    pixels.emplace_back(pixel);
                    channelCount += 3;
                }
            }
        }
    }

    if (channelCount != width * height * 3)
    {
        errorMsg = "Pixel data invalid or corrupted";
        return std::nullopt;
    }

    return pixels;
}

void parseP3Data(const std::string& fileName, ImageData& data)
{
    std::vector<std::string> lines;
    try
    {
        lines = getLines(fileName);
    }
    catch (const std::runtime_error& e)
    {
        data.exceptionMsg = e.what();
        return;
    }

    if (lines.size() < 4)
    {
        data.exceptionMsg = "Incomplete PPM header";
        return;
    }

    auto dimensions = parseDimensions(lines[1]);
    if (!dimensions)
    {
        data.exceptionMsg = "Invalid image dimensions";
        return;
    }

    auto maxColorVal = parseMaxColorValue(lines[2]);
    if (!maxColorVal)
    {
        data.exceptionMsg = "Invalid maximum color value";
        return;
    }

    std::string errorMsg;
    auto pixelData = parsePixelData(lines, 3,
                                    dimensions->first, dimensions->second,
                                    *maxColorVal, errorMsg);
    if (!pixelData)
    {
        data.exceptionMsg = errorMsg;
        return;
    }

    data.imageWidth = dimensions->first;
    data.imageHeight = dimensions->second;
    data.maxColorValue = *maxColorVal;
    data.pixelData = *pixelData;
}
