#include "PPMImage.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <optional>
#include <format>

static constexpr char s_commentChar = '#';
static const std::string s_PPMextension = ".ppm";

enum class PPMType
{
    None = 0,
    P3,
    P6 // will not be supported for now
};

//------------------------------------------------------------------------------
std::vector<std::string> loadFileData(const std::string& fileName);

ImageData processFileData(const std::vector<std::string>& lines);

ImageData handleP3Data(const std::vector<std::string>& lines);

// Helpers
bool isLineComment(const std::string& line);

bool hasPPMextension(const std::string& fileName);

PPMType parseMagicNumber(const std::string& magicNum);

std::optional<std::pair<unsigned int, unsigned int>>
parseDimensions(const std::string& line);

std::optional<unsigned int> parseMaxColorValue(const std::string& line);

std::optional<std::vector<unsigned int>>
parsePixelData(const std::vector<std::string>& lines, size_t startLine,
               unsigned int width, unsigned int height, unsigned int maxColorVal,
               std::string& errorMsg);
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

bool isLineComment(const std::string& line)
{
    return !line.empty() && line[0] == s_commentChar;
}

bool hasPPMextension(const std::string& fileName)
{
    if (fileName.length() > s_PPMextension.length())
    {
        return fileName.compare(fileName.length() - s_PPMextension.length(),
                                s_PPMextension.length(), s_PPMextension) == 0;
    }

    return false;
}

std::vector<std::string> loadFileData(const std::string &fileName)
{
    if (!hasPPMextension(fileName))
    {
        throw std::runtime_error("Invalid file: " + fileName);
    }

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

PPMType parseMagicNumber(const std::string& magicNum)
{
    if (magicNum == "P3") return PPMType::P3;
    if (magicNum == "P6") return PPMType::P6;
    return PPMType::None;
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
            return {0, 0, 0, {}, "PPM P6 not supported"};
            break;

        case PPMType::None:
            return {0, 0, 0, {}, "Invalid image format"};
            break;

        default:
            return {0, 0, 0, {}, "Reached the unreachable!"};
    }
}

std::optional<std::pair<unsigned int, unsigned int>>
parseDimensions(const std::string& line)
{
    std::istringstream dimensionStream(line);
    unsigned int width, height;

    if (dimensionStream >> width >> height)
    {
        return std::make_pair(width, height);
    }

    return std::nullopt;
}

std::optional<unsigned int> parseMaxColorValue(const std::string& line)
{
    std::istringstream maxColorStream(line);
    unsigned int maxColorVal;

    if (maxColorStream >> maxColorVal)
    {
        return maxColorVal;
    }

    return std::nullopt;
}

std::optional<std::vector<unsigned int>>
parsePixelData(const std::vector<std::string>& lines, size_t startLine,
               unsigned int width, unsigned int height, unsigned int maxColorVal,
               std::string& errorMsg)
{
    std::vector<unsigned int> pixels;
    pixels.reserve(width * height * 3);
    unsigned int r, g, b;
    unsigned int pixelCount = 0;

    for (size_t it = startLine; it < lines.size(); it++)
    {
        std::istringstream pixelStream(lines[it]);
        while (pixelStream >> r >> g >> b)
        {
            if (r > maxColorVal || g > maxColorVal || b > maxColorVal)
            {
                errorMsg = std::format("Pixel data out of range (0-{})", maxColorVal);
                return std::nullopt;
            }

            pixels.push_back(r);
            pixels.push_back(g);
            pixels.push_back(b);

            pixelCount++;
        }
    }

    if (pixelCount != width * height)
    {
        errorMsg = "Pixel data invalid or corrupted";
        return std::nullopt;
    }

    return pixels;
}

ImageData handleP3Data(const std::vector<std::string>& lines)
{
    if (lines.size() < 4)
    {
        return {0, 0, 0, {}, "Incomplete PPM header"};
    }

    auto dimensions = parseDimensions(lines[1]);
    if (!dimensions)
    {
        return {0, 0, 0, {}, "Invalid image dimensions"};
    }

    auto maxColorValue = parseMaxColorValue(lines[2]);
    if (!maxColorValue)
    {
        return {0, 0, 0, {}, "Invalid value for maximum color range"};
    }

    std::string errorMsg;
    auto pixelData = parsePixelData(lines, 3,
                                    dimensions->first, dimensions->second,
                                    *maxColorValue, errorMsg);

    if (!pixelData)
    {
            return {0, 0, 0, {}, errorMsg};
    }

    return {dimensions->first, dimensions->second, *maxColorValue, *pixelData};
}
