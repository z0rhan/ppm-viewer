#include "PPMImage.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdint>

constexpr char s_commentChar = '#';
const std::string s_PPMextension = ".ppm";

enum class PPMType
{
    None = 0,
    P3,
    P6
};

//------------------------------------------------------------------------------
void parseP3Data(std::istream& f, ImageData& data);
void parseP6Data(std::istream& f, ImageData& data);

// Helpers
bool hasPPMextension(const std::string& fileName);
PPMType parseHeader(std::istream& f, ImageData& data);
bool nextToken(std::istream& f, std::string& token);

//------------------------------------------------------------------------------
template <typename T>
T readValue(std::istream& f)
{
    std::string token;
    if (!nextToken(f, token))
        throw std::runtime_error("Unexpected EOF when a token is expected");

    std::istringstream iss(token);
    T value;
    if (!(iss >> value))
    {
        throw std::runtime_error("Invalid token: " + token);
    }

    return value;
}

//------------------------------------------------------------------------------
void getImageData(const std::string& fileName, ImageData& data)
{
    if (!hasPPMextension(fileName))
    {
        data.exceptionMsg = "Invalid file: " + fileName;
        return;
    }

    std::ifstream fileObj(fileName, std::ios::in | std::ios::binary);
    if (!fileObj)
    {
        data.exceptionMsg = fileName + " could not be opened!";
        return;
    }

    PPMType type = parseHeader(fileObj, data);

    switch (type)
    {
        case PPMType::P3:
            parseP3Data(fileObj, data);
            break;

        case PPMType::P6:
            data.exceptionMsg = "Not implemented now";
            //parseP6Data(fileObj, data);
            break;

        case PPMType::None:
            break;
    }

    fileObj.close();
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

PPMType parseHeader(std::istream& f, ImageData& data)
{
    PPMType type;
    try
    {
        std::string PPM_t = readValue<std::string>(f);
        if (PPM_t == "P3")
            type = PPMType::P3;
        else if (PPM_t == "P6")
            type = PPMType::P6;
        else
        {
            data.exceptionMsg = "Unsupported PPM type: " + PPM_t;
            return PPMType::None;
        }

        data.imageWidth = readValue<uint32_t>(f);
        data.imageHeight = readValue<uint32_t>(f);
        data.maxColorValue = readValue<uint32_t>(f);

        if (data.imageWidth <= 0 || data.imageHeight <= 0)
        {
            data.exceptionMsg = "Invalid image dimensions: "
                                + std::to_string(data.imageWidth) + " "
                                + std::to_string(data.imageHeight);
            return PPMType::None;
        }

        if (data.maxColorValue<= 0)
        {
            data.exceptionMsg = "Invalid Max Color value: "
                                + std::to_string(data.maxColorValue);
            return PPMType::None;
        }
    }
    catch (const std::runtime_error& e)
    {
        data.exceptionMsg = e.what();
        return PPMType::None;
    }

    return type;
}

// Only call if you're sure that there is anohter token in the stream
bool nextToken(std::istream& f, std::string& token)
{
    token.clear();
    char c;
    while (f.get(c))
    {
        if (c == s_commentChar)
        {
            std::string dummy;
            std::getline(f, dummy); // consume the rest string
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)))
        {
            if (!token.empty()) return true;
            continue;
        }

        token.push_back(c);
    }

    return !token.empty();
}

void parseP3Data(std::istream& f, ImageData& data)
{
    uint32_t channelCount = 0;
    if (data.maxColorValue <= 255)
    {
        data.pixelData.reserve(data.imageWidth * data.imageHeight);

        for (size_t it = 0; it < data.imageWidth * data.imageHeight ; it++)
        {
            uint32_t r, g, b;
            try
            {
                r = readValue<uint32_t>(f);
                g = readValue<uint32_t>(f);
                b = readValue<uint32_t>(f);
            }
            catch (const std::runtime_error& e)
            {
                data.exceptionMsg = e.what();
                return;
            }

            uint32_t pixel = (r << 16) | (g << 8) | b;
            data.pixelData.emplace_back(pixel);
            channelCount += 3;
        }
    }
    else
    {
        data.pixelData.reserve(data.imageWidth * data.imageHeight * 3);

        for (size_t it = 0; it < data.imageWidth * data.imageHeight ; it++)
        {
            uint32_t r, g, b;
            try
            {
                r = readValue<uint32_t>(f);
                g = readValue<uint32_t>(f);
                b = readValue<uint32_t>(f);
            }
            catch (const std::runtime_error& e)
            {
                data.exceptionMsg = e.what();
                return;
            }

            data.pixelData.emplace_back(r);
            data.pixelData.emplace_back(g);
            data.pixelData.emplace_back(b);
            channelCount += 3;
        }
    }

    if (channelCount != data.imageWidth * data.imageHeight * 3)
    {
        data.exceptionMsg = "Pixel data invalid or corrupted";
        return;
    }
}
