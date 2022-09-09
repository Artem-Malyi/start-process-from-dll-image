
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>

VOID EntryPoint()
{
    while (true) {
        OutputDebugStringA("Running from the Dll...");

        Sleep(5000);
    }
}