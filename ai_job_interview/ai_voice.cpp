#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

const char* API_KEY = getenv("GPT_API_KEY");

void speak(const string& text) {
    string command = "powershell -Command \"Add-Type -AssemblyName System.Speech; "
                     "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; "
                     "$speak.Speak(\\\"" + text + "\\\")\"";
    system(command.c_str());
}

string getVoiceInput() {
    system("powershell -ExecutionPolicy Bypass -File listen.ps1");

    ifstream file("input.txt");
    string input;
    if (file.is_open()) {
        getline(file, input);
        file.close();
    }
    return input;
}

string askChatGPT(const string& prompt) {
    // if (!API_KEY) {
    //     cerr << "❌ GPT_API_KEY environment variable not found.\n";
    //     exit(1);
    // }
    if (!API_KEY) {
        cerr << "GPT_API_KEY environment variable not found.\n";
        exit(1);
    } else {
        cout << "API key loaded (partial): " << string(API_KEY).substr(0, 10) << "..." << endl;
    }

    // Write request body to JSON
    ofstream request("request.json");
    request << R"({
        "model": "gpt-3.5-turbo",
        "messages": [
            {"role": "system", "content": "You are a helpful AI interviewer. Ask follow-up questions if needed."},
            {"role": "user", "content": ")" << prompt << R"("}
        ]
    })";
    request.close();

    // Run cURL to send request
    string command = "curl -s https://api.openai.com/v1/chat/completions "
                     "-H \"Content-Type: application/json\" "
                     "-H \"Authorization: Bearer " + string(API_KEY) + "\" "
                     "-d @request.json > response.txt";
    system(command.c_str());

    // Parse response from response.txt
    ifstream res("response.txt");
    string line, result;
    bool capture = false;
    while (getline(res, line)) {
        if (line.find("\"content\"") != string::npos) {
            size_t start = line.find(":") + 2;
            size_t end = line.rfind("\"");
            result = line.substr(start, end - start);
            break;
        }
    }
    res.close();

    return result;
}

void logConversation(const string& user, const string& bot) {
    ofstream log("conversation_log.txt", ios::app);
    if (log.is_open()) {
        log << "You: " << user << endl;
        log << "AI: " << bot << endl << endl;
        log.close();
    }
}

int main() {
    system("chcp 65001 > nul");
    speak("Hello! I'm your interview assistant. Let's begin.");

    while (true) {
        cout << "\nSpeak now...\n";
        string userInput = getVoiceInput();
        if (userInput.empty()) {
            speak("I didn't catch that. Try again.");
            continue;
        }

        cout << "You: " << userInput << endl;
        string reply = askChatGPT(userInput);

        if (!reply.empty()) {
            cout << "AI: " << reply << endl;
            speak(reply);
            logConversation(userInput, reply);
        } else {
            speak("Sorry, I couldn’t get a response.");
        }

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
