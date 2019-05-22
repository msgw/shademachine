#include "filesystem.h"

using namespace std;

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>

list<string> filesystem::list_directory(string path)
{
    list<string> directory_list;
    
    WIN32_FIND_DATAA file_info;
    HANDLE hFind = FindFirstFileA((path + "\*").c_str(), &file_info);
    
    if( hFind != INVALID_HANDLE_VALUE )
    {
        do
        {
            list.push_back(string(file_info.cFileName));
        } while (FindNextFileA(hFind, &file_info));
        FindClose(hFind);
    }
    
    return directory_list;
}

string filesystem::get_absolute_path(string file)
{
    char path[2048];
    GetFullPathNameA(file.c_str(), 2048, path, NULL);
    string absolute(path);
    
    return absolute;
}


#define makedir(dir) _mkdir(dir)

#else

#include <sys/stat.h>
#include <dirent.h>

list<string> filesystem::list_directory(string path)
{
    list<string> directory_list;
    
    DIR *dir;
    struct dirent *dir_info;
    dir = opendir(path.c_str());
    if (dir) {
        while ((dir_info = readdir(dir)) != NULL) {
            directory_list.push_back(string(dir_info->d_name));
        }
        closedir(dir);
    }
    
    return directory_list;
}

string filesystem::get_absolute_path(string file)
{
    char path[2048];
    string absolute(realpath(file.c_str(), path));
    
    return absolute;
}


#define makedir(dir) mkdir(dir, S_IRWXU)

#endif

bool filesystem::make_directory(std::string path)
{
    makedir(path.c_str());
    return true;
}
