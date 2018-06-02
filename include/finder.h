#ifndef _FINDER_H_
#define _FINDER_H_

#include <iostream>

class finder
{
public:
    finder(const int &cmp_sVal) :s_(cmp_sVal){}
    bool operator ()(const std::map<int,int>::value_type &item)
    {
        return item.second == s_;
    }
private:
    const int &s_;
};

#endif
