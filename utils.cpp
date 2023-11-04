// Credits to https://stackoverflow.com/questions/74949626/print-function-in-c-similar-to-python
#pragma once

#include <iostream>
#include <vector>
#include <map>

namespace detail {
    template<typename T>
    void printImpl(const T& o)
    {
        std::cout << o;
    }

    template<typename T>
    void printImpl(const std::vector<T>& vec)
    {
        char sep = '{';
        for (const auto& innerVector : vec) {
            std::cout << sep;
            printImpl(innerVector);
            sep = ',';
        }
        std::cout << "}";
    }

    template<typename K, typename V>
    void printImpl(const std::map<K, V>& cont)
    {
        char sep = '{';
        for (auto&& [k, v] : cont) {
            std::cout << sep;
            printImpl(k);
            std::cout << ":";
            printImpl(v);
            sep = ',';
        }
        std::cout << "}";
    }
}

template<typename... Args>
void print(Args&&... args)
{
    (detail::printImpl(std::forward<Args>(args)), ...);
    std::cout << std::endl;
}

int test_this_print()
{
    std::vector<int> vec1{ 1, 2, 3, 4, 5 };
    std::vector<std::vector<double>> vec2{ { 1.1, 2.2 }, { 3.3, 4.4 } };
    std::vector<std::vector<std::vector<char>>> vec3{ { { 'a', 'b' }, { 'c', 'd' } }, { { 'e', 'f' }, { 'g', 'h' } } };
    std::map<int, std::string> m{{0, "a"},{1, "b"}};

    print("chicken");
    print(vec1,vec1, "\n");
    print(vec2,"\n");
    print(vec3,"\n");
    print(m,"\n");

    return 0;
}