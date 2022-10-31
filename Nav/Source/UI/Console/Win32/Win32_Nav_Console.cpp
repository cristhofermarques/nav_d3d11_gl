#include <UI/Console/Nav_Console.hpp>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

Int32 ConvertToPlatformColor(Console::PrintColor color)
{
    switch(color)
    {
        case Console::PrintColor::Black :
            return 0;
        break;

        case Console::PrintColor::Gray :
            return 8;
        break;

        case Console::PrintColor::White :
            return 7;
        break;

        case Console::PrintColor::Yellow :
            return 6;
        break;

        case Console::PrintColor::Red :
            return 4;
        break;

        case Console::PrintColor::Blue :
            return 1;
        break;

        case Console::PrintColor::Green :
            return 2;
        break;
    }
}

void Console::SetPrintColor(PrintColor color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!hConsole){return;}

    SetConsoleTextAttribute(hConsole, ConvertToPlatformColor(color));
}