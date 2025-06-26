#include "PPMImage.hh"
#include "stb_image/stb_image_write.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

static constexpr char s_commentChar = '#'; // P6 is not supported for now

std::vector<std::string> parseFile(const std::string& fileName);
void processFileData(const std::vector<std::string>& lines);
bool isLineComment(const std::string& line);
PPMType parseMagicNumber(const std::string& magicNum);
ImageData handleP3Data(const std::vector<std::string>& lines);

bool converToPNG(const std::string& fileName)
{
    std::vector<std::string> lines;
    try
    {
        lines = parseFile(fileName);
        processFileData(lines);
        return true;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

std::vector<std::string> parseFile(const std::string &fileName)
{
    std::ifstream fileObj(fileName);
    if (!fileObj)
    {
        throw std::runtime_error(fileName + " cound not be opened");
    }

    std::string line;
    std::vector<std::string> lines;

    while (std::getline(fileObj, line))
    {
        if (isLineComment(line))
            continue;

        lines.push_back(line);
    }
    fileObj.close();

    return lines;
}

bool isLineComment(const std::string& line)
{
    return !line.empty() && line[0] == s_commentChar;
}

void processFileData(const std::vector<std::string>& lines)
{
    ImageData data;

    switch (parseMagicNumber(lines.at(0)))
    {
        case PPMType::P3:
            data = handleP3Data(lines);
            break;

        case PPMType::P6:
            throw std::runtime_error("PPM type P6 is not supported yet\n");
            break;

        case PPMType::None:
            throw std::runtime_error("Invalid or Unsupported PPM format\n");
            break;

        default:
            throw std::runtime_error("Hello there! ;)\n");
    }

    if (!data.pixelData.empty())
    {
        if (!stbi_write_png("output.png", data.imageWidth, data.imageHeight, 3, data.pixelData.data(), data.imageWidth * 3))
        {
            throw std::runtime_error("Failed writing to output.png\n");
        }
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
        throw std::runtime_error("Incomplete PPM header or Data\n");
    } // Incomplete data

    // Image dimensions
    unsigned int imageWidth, imageHeight;
    {
        std::istringstream dimensionStream(lines.at(1));
        if (!(dimensionStream >> imageWidth >> imageHeight))
        {
            throw std::runtime_error("Invalid image dimension format\n");
        }
    }

    // Max color value
    unsigned int maxColorValue;
    {
        std::istringstream maxColorValueStream(lines.at(2));
        if (!(maxColorValueStream >> maxColorValue))
        {
            throw std::runtime_error("Invalid max color value format\n");
        }
        if (maxColorValue != 255)
        {
            throw std::runtime_error("Max value " + std::to_string(maxColorValue) + " not supported\n");
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
            throw std::runtime_error("Pixel value out of valid range (0-255)\n");
        }
        pixelData.push_back(static_cast<unsigned char>(r));
        pixelData.push_back(static_cast<unsigned char>(g));
        pixelData.push_back(static_cast<unsigned char>(b));
    }

    if (pixelData.size() != imageWidth * imageHeight * 3)
    {
        throw std::runtime_error("Pixel data inconsistent with image dimension\n");
    }

    return {imageWidth, imageHeight, pixelData};
}
