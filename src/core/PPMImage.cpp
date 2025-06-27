#include "PPMImage.hh"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

enum class PPMType
{
    None = 0,
    P3,
    P6 // will not be supported for now
};

static constexpr char s_commentChar = '#'; // P6 is not supported for now

std::vector<std::string> loadFileData(const std::string& fileName);
ImageData processFileData(const std::vector<std::string>& lines);
ImageData handleP3Data(const std::vector<std::string>& lines);

// Helpers
bool isLineComment(const std::string& line);
PPMType parseMagicNumber(const std::string& magicNum);

//------------------------------------------------------------------------------
ImageData getImageData(const std::string& fileName)
{
    std::vector<std::string> lines;
    try
    {
        lines = loadFileData(fileName);
        return processFileData(lines);
    }
    catch (const std::runtime_error& e)
    {
        ImageData data;
        data.exceptionMsg = e.what();
        return data;
    }
}

//------------------------------------------------------------------------------
std::vector<std::string> loadFileData(const std::string &fileName)
{
    std::ifstream fileObj(fileName);
    if (!fileObj)
    {
        throw std::runtime_error(fileName + " could not be opened");
    }

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(fileObj, line))
    {
        if (isLineComment(line))
            continue;

        lines.emplace_back(std::move(line));
    }
    fileObj.close();

    return lines;
}

bool isLineComment(const std::string& line)
{
    return !line.empty() && line[0] == s_commentChar;
}

ImageData processFileData(const std::vector<std::string>& lines)
{
    const PPMType type = parseMagicNumber(lines.at(0));
    switch (type)
    {
        case PPMType::P3:
            return handleP3Data(lines);
            break;

        case PPMType::P6:
            return {0, 0, {}, "PPM P6 not supported"};
            break;

        case PPMType::None:
            return {0, 0, {}, "Invalid image format"};
            break;

        default:
            return {0, 0, {}, "Hello there ;)"};
    }
}

PPMType parseMagicNumber(const std::string& magicNum)
{
    if (magicNum == "P3") return PPMType::P3;
    if (magicNum == "P6") return PPMType::P6;
    return PPMType::None;
}

ImageData handleP3Data(const std::vector<std::string>& lines)
{
    if (lines.size() < 4)
    {
        return {0, 0, {}, "Incomplete PPM header"};
    } // Incomplete data

    // Image dimensions
    unsigned int imageWidth, imageHeight;
    {
        std::istringstream dimensionStream(lines.at(1));
        if (!(dimensionStream >> imageWidth >> imageHeight))
        {
            return {0, 0, {}, "Invalid image dimensions"};
        }
    }

    // Max color value
    unsigned int maxColorValue;
    {
        std::istringstream maxColorValueStream(lines.at(2));
        if (!(maxColorValueStream >> maxColorValue))
        {
            return {0, 0, {}, "Invalid max color value format"};
        }
        if (maxColorValue != 255)
        {
            return {0, 0, {}, "Max color value: " + std::to_string(maxColorValue) + " not supported"};
        }
    }

    // Pixels data
    std::string pixels;
    for (unsigned int i = 3; i < lines.size(); i++)
    {
        pixels += lines.at(i) + ' ';
    }
    std::istringstream pixelDataStream(pixels);

    std::vector<unsigned char> pixelData;
    pixelData.reserve(imageWidth * imageHeight * 3);

    unsigned int r, g, b;
    while (pixelDataStream >> r >> g >> b)
    {
        if (r < 0 || r > maxColorValue ||
            g < 0 || g > maxColorValue ||
            b < 0 || b > maxColorValue)
        {
            return {0, 0, {}, "Pixel value out of range: [0-255]"};
        }
        pixelData.emplace_back(static_cast<unsigned char>(r));
        pixelData.emplace_back(static_cast<unsigned char>(g));
        pixelData.emplace_back(static_cast<unsigned char>(b));
    }

    if (pixelData.size() != imageWidth * imageHeight * 3)
    {
        return {0, 0, {}, "Pixel data inconsistent with image dimension"};
    }

    return {imageWidth, imageHeight, pixelData};
}
