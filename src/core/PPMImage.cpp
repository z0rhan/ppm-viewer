#include "PPMImage.hh"

#include <fstream>
#include <ios>
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
bool getToken(std::istream& f, std::string& token);

//------------------------------------------------------------------------------
template <typename T>
T fromStream(std::istream& f)
{
    std::string token;
    if (!getToken(f, token))
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
            parseP6Data(fileObj, data);
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
        std::string PPM_t = fromStream<std::string>(f);
        if (PPM_t == "P3")
            type = PPMType::P3;
        else if (PPM_t == "P6")
            type = PPMType::P6;
        else
        {
            data.exceptionMsg = "Unsupported PPM type: " + PPM_t;
            return PPMType::None;
        }

        data.imageWidth = fromStream<uint32_t>(f);
        data.imageHeight = fromStream<uint32_t>(f);
        data.maxColorValue = fromStream<uint32_t>(f);

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
bool getToken(std::istream& f, std::string& token)
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
    std::vector<uint16_t> pixelData;
    pixelData.reserve(data.imageWidth * data.imageHeight * 3);

    std::streampos currentPos = f.tellg();
    f.seekg(0, std::ios::end);
    std::streampos endPos = f.tellg();
    std::streamsize size = endPos - currentPos;
    
    f.seekg(currentPos);

    std::string fileContent(size, '\0');
    f.read(reinterpret_cast<char*>(&fileContent[0]), size);

    const char* contentPtr = fileContent.c_str();
    const char* endPtr = contentPtr + fileContent.size();

    while (contentPtr < endPtr)
    {
        while (contentPtr < endPtr && std::isspace(static_cast<unsigned char>(*contentPtr))) contentPtr++;

        if (*contentPtr == '#')
        {
            while (contentPtr < endPtr && *contentPtr != '\n') contentPtr++;
        }

        char* next;
        long value = std::strtol(contentPtr, &next, 10);
        if (contentPtr == next) break;
        contentPtr = next;

        if (data.maxColorValue <= 255)
            value = (value * 255) / data.maxColorValue;
        else
            value = (value * 65535) / data.maxColorValue;

        pixelData.emplace_back(static_cast<uint16_t>(value));
    }

    data.pixelData = std::move(pixelData);
}

void parseP6Data(std::istream& f, ImageData& data)
{
    // Get the size of the binary data
    std::streampos currentPos = f.tellg(); // get current position
    f.seekg(0, std::ios::end); // get the position of EOF
    std::streampos endPos = f.tellg();
    std::streamsize dataSize = endPos - currentPos;

    f.seekg(currentPos); // Move to the starting position
    // The above can be avoided by just allocating a buffer of
    // width * height * bytesPerSample
    // But I'm doing it anyway for learning purposes

    uint32_t channelCount = 0;
    if (data.maxColorValue <= 255)
    {
        std::vector<uint8_t> buffer(dataSize);

        if (!f.read(reinterpret_cast<char*>(buffer.data()), dataSize))
        {
            data.exceptionMsg = "Error: could only read " +
                                std::to_string(f.gcount()) +
                                " of " + std::to_string(dataSize) + " bytes!";
            return;
        }

        data.pixelData.reserve(data.imageWidth * data.imageHeight * 3);
        for (size_t it = 0; it < buffer.size(); it += 3)
        {
            uint16_t r = buffer[it];
            uint16_t g = buffer[it + 1];
            uint16_t b = buffer[it + 2];

            // Map each value to 8-bit range
            r = (r * 255) / data.maxColorValue;
            g = (g * 255) / data.maxColorValue;
            b = (b * 255) / data.maxColorValue;

            data.pixelData.emplace_back(r);
            data.pixelData.emplace_back(g);
            data.pixelData.emplace_back(b);
            channelCount += 3;
        }
    }
    else
    {
        std::vector<uint8_t> buffer(dataSize);

        if (!f.read(reinterpret_cast<char*>(buffer.data()), dataSize))
        {
            data.exceptionMsg = "Error: could only read " +
                                std::to_string(f.gcount()) +
                                " of " + std::to_string(dataSize) + " bytes!";
            return;
        }

        data.pixelData.reserve(data.imageWidth * data.imageHeight * 3);
        for (size_t it = 0; it < buffer.size(); it += 6)
        {
            uint16_t r = (buffer[it] << 8) | buffer[it + 1];
            uint16_t g = (buffer[it + 2] << 8) | buffer[it + 3];
            uint16_t b = (buffer[it + 4] << 8) | buffer[it + 5];

            // Map each value to 16-bit range
            r = (r * 65535) / data.maxColorValue;
            g = (g * 65535) / data.maxColorValue;
            b = (b * 65535) / data.maxColorValue;

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
