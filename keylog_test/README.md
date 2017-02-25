The quickest solution I could come up with for a keylogging test.

First, you need meterpreter running on the system you run this on.
Then, on your msf host, interact with the session and run
`keyscan_start`

From here:

1) Close all important programs
2) Start the script
3) If notepad isn't in focus, click it
4) Go read a real copy of War and Peace
5) Run keyscan_dump on the msf host and see what you get

Note: win32com is provided in pypiwin32.  Run `pip install pypiwin32` to
install it.  Feel free to come up with a more robust solution if you
like.
