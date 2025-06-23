@echo off
REM Load .env file into environment variables
for /f "usebackq tokens=1,2 delims==" %%A in (".env") do set %%A=%%~B

REM Run the executable
ai_voice.exe
pause
