#ifndef SafeFile_HPP
#define SafeFile_HPP

#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>

class SafeFile{
private :
int fd;
std::string filepath;

public:
// parametrized constructor
SafeFile(const std::string& path,int flags);

// copy constructor
SafeFile(const SafeFile& other)= delete;

//copy assignment
SafeFile& operator=(const SafeFile&other)=delete;


//move constructor 
SafeFile(SafeFile&& other)noexcept;

//move assignment
SafeFile&operator=(SafeFile && other)noexcept;

// destructor
~SafeFile();

//methods
bool isValid()const;
int getFd() const;
ssize_t read(void* buffer, size_t count);
ssize_t write(const void* buffer, size_t count);

};


#endif