#include <iostream>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>
#include <fstream>
#include <thread>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printCredits() {
    ifstream creditsFile("Credits.txt");

    if (creditsFile.is_open()) {
        string line;
        setColor(1);
        while (getline(creditsFile, line)) {
            cout << line << endl;
        }
        creditsFile.close();
        cout << "\n\n";
        setColor(7);
    } else {
        cout << "Error: Couldn't open or find Credits.txt.\n";
    }
}

void sendWebhk(const wstring& webhookPath, const string& message) {
    HINTERNET hSession = WinHttpOpen(L"Fontesie/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        wcerr << L"WinHttpOpen failed.\n";
        return;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        wcerr << L"WinHttpConnect failed.\n";
        WinHttpCloseHandle(hSession);
        return;
    }

    string payload = R"({"content": ")" + message + R"("})";

    while (true) {
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", webhookPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            wcerr << L"WinHttpOpenRequest failed.\n";
            break;
        }

        BOOL bResults = WinHttpSendRequest(hRequest,
            L"Content-Type: application/json\r\n",
            (DWORD)-1L,
            (LPVOID)payload.c_str(),
            (DWORD)payload.length(),
            (DWORD)payload.length(),
            0);

        if (!bResults) {
            wcerr << L"WinHttpSendRequest failed.\n";
            WinHttpCloseHandle(hRequest);
            break;
        }

        bResults = WinHttpReceiveResponse(hRequest, NULL);
        if (!bResults) {
            wcerr << L"WinHttpReceiveResponse failed.\n";
            WinHttpCloseHandle(hRequest);
            break;
        }

        WinHttpCloseHandle(hRequest);
    }

    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}

void showMenu() {
    string fullWebhookURL;
    string message;

    cout << "Enter the Webhook URL: ";
    getline(cin, fullWebhookURL);

    cout << "Enter the message to send: ";
    getline(cin, message);

    size_t pos = fullWebhookURL.find("discord.com");
    string webhookPath = fullWebhookURL;
    if (pos != string::npos) {
        webhookPath = fullWebhookURL.substr(pos + strlen("discord.com"));
    }

    wstring wWebhookPath(webhookPath.begin(), webhookPath.end());

    cout << "Sending messages..." << endl;

    setColor(1);
    sendWebhk(wWebhookPath, message);
}

int main() {
    printCredits();
    showMenu();
    return 0;
}
