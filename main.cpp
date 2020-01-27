#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <iostream>

#define BUFSIZE 4096 


int main(int argc, TCHAR* argv[])
{
    std::cout << "Start execution" << std::endl;

    HANDLE parent_pipe = NULL;
    HANDLE child_pipe = NULL;
    SECURITY_ATTRIBUTES saAttr;

    // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create a pipe for the child process's STDOUT. 

    if (!CreatePipe(&parent_pipe, &child_pipe, &saAttr, 0)) {
        std::cout << "failed to create pipe, exiting..." << std::endl;
        exit(1);
    }

    // Ensure the read handle to the pipe for STDOUT is not inherited.
    SetHandleInformation(parent_pipe, HANDLE_FLAG_INHERIT, 0);

    // Create the child process. 
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure. 
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure.
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.hStdOutput = child_pipe;
    si.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process.
    wchar_t command[] = L"C:\\Windows\\System32\\ipconfig.exe";

    bSuccess = CreateProcess(NULL,
        command,    // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &si,  // STARTUPINFO pointer 
        &pi);  // receives PROCESS_INFORMATION 

    // If an error occurs, exit the application. 
    if (!bSuccess) {
        std::cout << "Subprocess creation failed" << std::endl;
        exit(1);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Read from pipe that is the standard output for child process. 
    std::cout << "output of the child process:" << std::endl;
    DWORD dwRead;
    CHAR chBuf[BUFSIZE];

    while (true)
    {
        bSuccess = ReadFile(parent_pipe, chBuf, BUFSIZE, &dwRead, NULL);
        std::cout << chBuf << std::endl;
        if (!bSuccess || dwRead == 0) break;
    }




    return 0;
}
