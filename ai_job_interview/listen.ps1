Add-Type -AssemblyName System.Speech
$recognizer = New-Object System.Speech.Recognition.SpeechRecognitionEngine
$recognizer.LoadGrammar([System.Speech.Recognition.DictationGrammar]::new())
$recognizer.SetInputToDefaultAudioDevice()

Write-Host "🎤 Listening..."
$result = $recognizer.Recognize()
$result.Text | Out-File -FilePath input.txt -Encoding UTF8
