#ifndef APPLICATION_HH
#define APPLICATION_HH

#include <string>

class Application
{
public:
    Application();
    ~Application();

    int run(int argc, char** argv);
private:
    std::string m_fileName;
    void parseArguments(int argc, char** argv);
};

#endif // APPLICATION_HH
