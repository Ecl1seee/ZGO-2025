#include "pch.cpp"
#include <iostream>
#include <time.h>

extern "C" {

    int __stdcall slength(char* buffer, char* str)
    {
        if (str == nullptr)
            return 0;
        int len = 0;
        for (int i = 0; i < 256; i++)
            if (str[i] == '\0')
            {
                len = i; break;
            }
        return len;
    }

    char* __stdcall itoa_func(char* buffer, int value)
    {
        bool neg = value < 0;
        unsigned int n = neg ? -value : value;

        int i = 0;
        do {
            buffer[i++] = char('0' + (n % 10));
            n /= 10;
        } while (n && i < 255);

        if (neg && i < 255) buffer[i++] = '-';
        buffer[i] = '\0';

        for (int l = 0, r = i - 1; l < r; ++l, --r)
            std::swap(buffer[l], buffer[r]);

        return buffer;
    }

    int __stdcall outrad(char* ptr)
    {
        if (ptr == nullptr)
        {
            std::cout << std::endl;
            return 0;
        }
        for (int i = 0; ptr[i] != '\0'; i++)
            std::cout << ptr[i];
        return 0;
    }
    int __stdcall outlich(int value)
    {
        std::cout << value;
        return 0;
    }
}
