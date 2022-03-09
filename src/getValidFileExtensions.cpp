
#include "definitions.h"

std::vector<std::string> getValidFileExtensions()
{
    std::vector<std::string> fe;
    fe.push_back("cpp");
    fe.push_back("cc");
    fe.push_back("c");
    fe.push_back("cxx");
    fe.push_back("c++");
    fe.push_back("h");
    fe.push_back("hpp");
    fe.push_back("hh");
    fe.push_back("hxx");
    fe.push_back("h++");

    // TODO: REMOVE THIS
    fe.push_back("dart");

    return fe;
}

