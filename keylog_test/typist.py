import win32com.client
import time

def send_keys(text):
    for src, dst in [('{', '['), ('}', ']'), ('(', '{(}'), (')','{)}'), ('+', '{+}'), ('^', '{^}'), (r'%', r'{%}'), ('~', '{~}')]:
        text = text.replace(src, dst)
    sh.SendKeys(text)

print "I'LL TAKE THE RAPISTS FOR $200"

sh = win32com.client.Dispatch('WScript.Shell')
sh.run("notepad")
f = open("war_peace.txt", "r")

sh.AppActivate("notepad")
for ch in iter(lambda: f.read(1), ''):
	send_keys(ch)
	time.sleep(0.001)
