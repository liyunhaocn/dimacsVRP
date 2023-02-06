
#ifndef CN_HUST_LYH_UTIL_LOGGER_H
#define CN_HUST_LYH_UTIL_LOGGER_H

#include <iostream>

namespace hust {

class Logger {

public:

    static bool infoFlag;
    static bool debugFlag;
    static bool errorFlag;

    template<typename ...T>
    static void INFO(const T&... args) {
        if (infoFlag == true) {
            println_("[INFO]:", args...);
        }
    }

    template<typename ...T>
    static void DEBUG(const T&... args) {
        if (debugFlag == true) {
            println_("[INFO]:", args...);
        }
    }

    template<typename ...T>
    static void ERROR(const T&... args) {
        if (errorFlag == true) {
            printlnerr_("[INFO]:", args...);
        }
    }

    template<typename T>
    static void printElementInContainer(T arr) {
        std::cout << "[ ";
        for (auto& i : arr) {
            std::cout << i << ",";
        }
        std::cout << "]" << std::endl;
    }

private:

    static void println_() { std::cout << std::endl; }

    template<typename T, typename ... Types>
    static void println_(const T& firstArg, const Types&... args) {

        std::cout << firstArg << " ";
        println_(args...);
    }

    static void printlnerr_() { std::cerr << std::endl; }
    template<typename T, typename ... Types>
    static void printlnerr_(const T& firstArg, const Types&... args) {

        std::cerr << firstArg << " ";
        printlnerr_(args...);
    }

};

}

#endif // CN_HUST_LYH_UTIL_LOGGER_H