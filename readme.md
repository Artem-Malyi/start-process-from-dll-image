# Example of starting a process from DLL image using Native Windows API

It's possible to start a process from a legitimate dll (with .dll extension in the file name and with IMAGE_FILE_DLL bit set in IMAGE_NT_HEADERS->IMAGE_FILE_HEADER->Characteristics)
![Here's the how such a process looks from the TaskManager.](/example.png?raw=true "Start a process from DLL image with Native Windows API")
