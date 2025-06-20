#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib> // For system()

using namespace std;

void speak(string text)
{
    string command = "powershell -Command \"Add-Type -AssemblyName System.Speech; "
                     "$speak = New-Object System.Speech.Synthesis.SpeechSynthesizer; "
                     "$speak.Speak(\\\"" +
                     text + "\\\")\"";
    system(command.c_str());
}

int main()
{
    string input;
    cout << "You: ";
    getline(cin, input);

    transform(input.begin(), input.end(), input.begin(), ::tolower);

    if (input == "hi" || input == "hello" || input == "hey")
    {
        cout << "AI: Welcome! Nice to meet you." << endl;
        speak("Welcome! Nice to meet you.");
    }
    else
    {
        cout << "AI: I don't understand that." << endl;
        speak("I don't understand that.");
    }

    return 0;
}


// g++ welcome_ai.cpp -o welcome_ai
// ./welcome_ai.exe