#include <Windows.h>
#include <iostream>
#include <vector>
//#include <bindlink.h>
//#pragma comment(lib, "bindlink.lib")

typedef enum CREATE_BIND_LINK_FLAGS
{
    CREATE_BIND_LINK_FLAG_NONE = 0x00000000,
    CREATE_BIND_LINK_FLAG_READ_ONLY = 0x00000001,
    CREATE_BIND_LINK_FLAG_MERGED = 0x00000002,
} CREATE_BIND_LINK_FLAGS;

DEFINE_ENUM_FLAG_OPERATORS(CREATE_BIND_LINK_FLAGS);

typedef  HRESULT(__stdcall* PtrCreateBindLink)(
    PVOID jobHandle,
    CREATE_BIND_LINK_FLAGS createBindLinkFlags,
    PCWSTR virtualPath,
    PCWSTR backingPath,
    UINT32 exceptionCount,
    PCWSTR* const exceptionPaths);


typedef  HRESULT(__stdcall* PtrRemoveBindLink)(
    PVOID reserved,
    PCWSTR backingPath);


PtrCreateBindLink MyCreateBindLink = NULL;
PtrRemoveBindLink MyRemoveBindLink = NULL;


void PrintHresultInfo(HRESULT hr) {
    DWORD win32 = HRESULT_CODE(hr); // same as hr & 0xFFFF
    LPWSTR msg = nullptr;
    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        win32,
        0,
        (LPWSTR)&msg,
        0,
        nullptr
    );
    std::wcout << L"HRESULT: 0x" << std::hex << hr << L"  Win32: " << std::dec << win32 << L"\n";
    if (msg) { std::wcout << L"Message: " << msg << L"\n"; LocalFree(msg); }
}

bool CreateProxyFolder(const std::wstring& folderPath)
{
    if (CreateDirectoryW(folderPath.c_str(), nullptr))
    {
        std::wcout << L"Folder created: " << folderPath << std::endl;
        return true;
    }
    else {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            //std::wcout << L"Folder already exists: " << folderPath << std::endl;
            return true;
        }
        else
        {
            std::wcerr << L"Failed to create folder: " << folderPath
                << L" (Error code: " << error << L")" << std::endl;
            return false;
        }
    }
}

std::vector<std::wstring> GetFolderPathsInDirectory(PCWSTR inputPath, std::wstring exceptionPath, std::wstring proxyPath)
{
    std::vector<std::wstring> folderPaths;
    std::wstring basePath = inputPath;
    if (basePath.back() != L'\\')
    {
        basePath += L'\\';
    }
    std::wstring searchPattern = basePath + L"*";
    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                wcscmp(findData.cFileName, L".") != 0 &&
                wcscmp(findData.cFileName, L"..") != 0) {
                std::wstring fullPath = basePath + findData.cFileName;
                // Skip if fullPath matches the exception
                if (_wcsicmp(fullPath.c_str(), exceptionPath.c_str()) == 0) {
                    continue;
                }

                //create new folder on proxy path (backed path)
                std::wstring tempProxyPath = proxyPath + L'\\' + findData.cFileName;
                CreateProxyFolder(tempProxyPath);

                folderPaths.push_back(findData.cFileName);
            }
        } while (FindNextFileW(hFind, &findData));
        FindClose(hFind);
    }
    return folderPaths;
}

int wmain(int argc, wchar_t* argv[])
{
    std::wcout << L"\nEDR-Redir.exe: Tool to redirect the EDR to another location\n"
        << L"\nGitHub:  https://github.com/TwoSevenOneT/EDR-Redir\n"
        << L"\n  Two Seven One Three: https://x.com/TwoSevenOneT\n"
        << L"\n==========================================================\n\n";

    if (argc > 4 || argc < 2)
    {
        std::wcerr << std::endl;
        std::wcerr << L"EDR-Redir.exe <VirtualPath> <BackingPath>" << std::endl;
        std::wcerr << L"EDR-Redir.exe <VirtualPath> <BackingPath> <ExceptionPath>" << std::endl;
        std::wcerr << L"\nTo remove a link that was previously created" << std::endl;
        std::wcerr << L"EDR-Redir.exe <VirtualPath>" << std::endl;
        std::wcerr << std::endl;
        return 1;
    }
    //std::vector<PCWSTR> exceptionPaths = GetFolderPathsInDirectory(argv[1]);
    HRESULT hr;
    HMODULE hBindflt = LoadLibraryW(L"bindfltapi.dll");
    if (hBindflt)
    {
        MyCreateBindLink = (PtrCreateBindLink)GetProcAddress(hBindflt, "BfSetupFilter");
        MyRemoveBindLink = (PtrRemoveBindLink)GetProcAddress(hBindflt, "BfRemoveMapping");
    }
    else
    {
        std::wcerr << std::endl;
        std::wcerr << L"OS NOT SUPPORT" << std::endl;
        return 1;
    }
    if (argc == 2)
    {
        int result = 0;
        hr = MyRemoveBindLink(0, argv[1]);
        if (FAILED(hr))
        {
            std::wcerr << L"Failed to remove Bind Link. HRESULT: " << hr << std::endl;
            PrintHresultInfo(hr);
            result = 1;
        }
        else
        {
            std::wcout << L"Remove Bind Link: " << argv[1] << L" successfully" << std::endl;
        }
        //just make sure all subfolders bind link are also removed
        std::wstring basePath = argv[1];
        if (basePath.back() != L'\\')
        {
            basePath += L'\\';
        }
        std::wstring searchPattern = basePath + L"*";
        WIN32_FIND_DATAW findData;
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    wcscmp(findData.cFileName, L".") != 0 &&
                    wcscmp(findData.cFileName, L"..") != 0) {
                    std::wstring fullPath = basePath + findData.cFileName;
                    MyRemoveBindLink(0, fullPath.c_str());
                }
            } while (FindNextFileW(hFind, &findData));
            FindClose(hFind);
        }
        if (result != 0)
        {
            //try to remove again
            hr = MyRemoveBindLink(0, argv[1]);
            if (FAILED(hr))
            {
                std::wcerr << L"Retry Failed to remove Bind Link. HRESULT: " << hr << std::endl;
                PrintHresultInfo(hr);
                result = 1;
            }
            else
            {
                std::wcout << L"Retry Remove Bind Link: " << argv[1] << L" successfully" << std::endl;
            }

        }
        //
        return result;
    }
    std::wstring virtualPath = argv[1];
    std::wstring backingPath = argv[2];
    if (argc == 3)
    {
        hr = MyCreateBindLink(0, CREATE_BIND_LINK_FLAG_NONE, virtualPath.c_str(), backingPath.c_str(), 0, NULL);
        if (FAILED(hr))
        {
            std::wcerr << L"CreateBindLink failed, HRESULT=0x" << std::hex << hr << L"\n";
            PrintHresultInfo(hr);
            return 1;
        }
    }
    else if (argc == 4)
    {
        std::wstring exceptionPath = argv[3];
        std::vector<std::wstring> exceptionPaths = GetFolderPathsInDirectory(virtualPath.c_str(), exceptionPath, backingPath);

        std::wcout << L"Total paths found in : " << virtualPath << L":" << exceptionPaths.size() << std::endl;


        std::wcout << L"Starting create reverse proxy bind link..." << std::endl;
        for (const std::wstring& path : exceptionPaths)
        {
            //vector contains only folder names, need to append to full path
            //create reverse bind link for each folder found, except the exception path
            //this will keep access to virtual path work normally
            std::wstring tvirtualPath = backingPath;
            tvirtualPath += L"\\";
            tvirtualPath += path;

            std::wstring tbackingPath = virtualPath;
            tbackingPath += L"\\";
            tbackingPath += path;

            std::wcout << tvirtualPath << L" <==> " << tbackingPath << std::endl;
            MyRemoveBindLink(0, tvirtualPath.c_str());
            hr = MyCreateBindLink(0, CREATE_BIND_LINK_FLAG_NONE, tvirtualPath.c_str(), tbackingPath.c_str(), 0, NULL);
            if (FAILED(hr))
            {
                std::wcerr << L"CreateBindLink with exception paths failed, HRESULT=0x" << std::hex << hr << L"\n";
                PrintHresultInfo(hr);
            }
        }
        std::wcout << L"Starting create proxy bind link..." << std::endl;
        for (const std::wstring& path : exceptionPaths)
        {
            std::wstring tvirtualPath = virtualPath;
            tvirtualPath += L"\\";
            tvirtualPath += path;

            std::wstring tbackingPath = backingPath;
            tbackingPath += L"\\";
            tbackingPath += path;

            std::wcout << tvirtualPath << L" <==> " << tbackingPath << std::endl;
            MyRemoveBindLink(0, tvirtualPath.c_str());
            hr = MyCreateBindLink(0, CREATE_BIND_LINK_FLAG_NONE, tvirtualPath.c_str(), tbackingPath.c_str(), 0, NULL);
            if (FAILED(hr))
            {
                std::wcerr << L"CreateBindLink with exception paths failed, HRESULT=0x" << std::hex << hr << L"\n";
                PrintHresultInfo(hr);
            }
        }

        std::wcout << L"Starting create main bind link..." << std::endl;
        hr = MyCreateBindLink(0, CREATE_BIND_LINK_FLAG_NONE, virtualPath.c_str(), backingPath.c_str(), 0, NULL);
        if (FAILED(hr))
        {
            std::wcerr << L"CreateBindLink with exception paths failed, HRESULT=0x" << std::hex << hr << L"\n";
            PrintHresultInfo(hr);
            return 1;
        }

        //
    }

    std::wcout << L"CreateBindLink: (VirtualPath) <==> (BackingPath): " << virtualPath << L" <==> " << backingPath << L" successfully" << std::endl;
    //
    return 0;
}
