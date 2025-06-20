#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;

const string API_KEY = "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"; // 🔐 Replace with your real API key

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
    ofstream request("request.json");
    request << R"({
        "model": "gpt-3.5-turbo",
        "messages": [{"role": "user", "content": ")" << prompt << R"("}]
    })";
    request.close();

    string command = "curl https://api.openai.com/v1/chat/completions "
                     "-H \"Content-Type: application/json\" "
                     "-H \"Authorization: Bearer " + API_KEY + "\" "
                     "-d @request.json > response.txt";
    system(command.c_str());

    // Extract response from response.txt
    ifstream res("response.txt");
    string line, result;
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

int main() {
    speak("Hello! I'm your interview assistant. Let's begin.");

    while (true) {
        cout << "\n🎙️ Speak now...\n";
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
        } else {
            speak("Sorry, I couldn’t get a response.");
        }

        // Optional delay
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}

// g++ ai_voice.cpp -o ai_voice
// ./ai_voice
