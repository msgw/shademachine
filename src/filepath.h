#include <iostream>
#include <algorithm>
#include <string>
#include <list>

class filepath
{
public:
    static bool make_directory(std::string path);
    
    static std::string get_absolute_path(std::string file);

    static std::list<std::string> list_directory(std::string path);
};
