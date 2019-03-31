#include <iostream>
#include <string>
#include <windows.h>
#include "Constants.h"

using namespace std;

void sendMessage(string& str, HANDLE pipe) {
	DWORD written = 0;
	if (!WriteFile(pipe, str.c_str(), str.size(), &written, NULL)) {
		cerr << "Error while writting to pipe" << endl;
	}
	if (written != str.size()) {
		cerr << "Cannot write all " << str.size() << " bytes to pipe. Only " << written << " bytes written" << endl;
	}
}

int main() {
	HANDLE semaphore = CreateSemaphore(NULL, 0, 1, SEMAPHORE_NAME);
	if (semaphore == NULL) {
		cerr << "Error while creating semaphore" << endl;
		return -1;
	}

	HANDLE pipe = CreateNamedPipe(
		PIPE_NAME,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,
		1024, 1024,
		5000,
		NULL
	);
	if (pipe == INVALID_HANDLE_VALUE) {
		cerr << "Error while creating pipe" << endl;
		CloseHandle(semaphore);
		return -1;
	}

	cout << "Waiting for client" << endl;
	if (!ConnectNamedPipe(pipe, NULL)) {
		cerr << "Cannot connect to client!" << endl;
		CloseHandle(semaphore);
		CloseHandle(pipe);
		return -1;
	}

	string str;
	while (true) {
		cout << "Enter message for client(q for quit):" << endl;
		string str;
		getline(cin, str);
		sendMessage(str, pipe);
		ReleaseSemaphore(semaphore, 1, NULL);
		if (str == "q") {
			break;
		}
	}

	if (!DisconnectNamedPipe(pipe)) {
		cerr << "Cannot disconnect from client!" << endl;
		return -1;
	}
	CloseHandle(semaphore);
	CloseHandle(pipe);
	return 0;
}