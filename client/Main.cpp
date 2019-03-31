#include <iostream>
#include <string>
#include <windows.h>
#include "..\server\Constants.h"

using namespace std;

string receiveMessage(HANDLE pipe) {
	char buffer[1024];
	DWORD read = 0;
	ReadFile(pipe, buffer, 1024, &read, NULL);
	buffer[read] = '\0';
	return buffer;
}

int main() {
	HANDLE semaphore = OpenSemaphore(SYNCHRONIZE, FALSE, SEMAPHORE_NAME);
	if (semaphore == NULL) {
		cerr << "Cannot open semaphore!" << endl;
		return -1;
	}

	HANDLE pipe = CreateFile(
		PIPE_NAME,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (pipe == INVALID_HANDLE_VALUE) {
		cerr << "Cannot open pipe!" << endl;
		CloseHandle(semaphore);
		return -1;
	}

	while (true) {
		if (WaitForSingleObject(semaphore, INFINITE) == WAIT_OBJECT_0) {
			string str = receiveMessage(pipe);
			if (str == "q") {
				break;
			}
			cout << str << endl;
		} else {
			break;
		}
	}

	CloseHandle(pipe);
	CloseHandle(semaphore);
	return 0;
}