/*
 * JPC-1000 programmable process controller
 * 
 * Libraries needed:
 *  - ssd1306
 *  
 * Pins:
 *  - 3: OLED reset
 *  - 4: OLED CS
 *  - 5: OLED DC
 *  - 6: OK button (pull to ground to press)
 *  - 10: SSR for heater
 *  - 11: MOSI for SPI (OLED)
 *  - 13: CLK for SPI (OLED)
 */

#include <EEPROM.h>
#include "ssd1306.h"
#include "nano_gfx.h"

#define MAXSEGMENTS 32

typedef struct ProgramSegment {
  char type; // RAMP/STEP
  int target; // degrees C
  unsigned long duration; // seconds
} ProgramSegment;

enum segment_types { RAMP = 0, STEP };
enum display_modes { MAIN = 0, MENU, PROGRAM, SETUP, SEGMENT };
enum input_buttons { UP = 0, DOWN, OK, CANCEL };
typedef void (*ScreenHandlerFunc)(void);

const float min_temp = 5;
const float max_temp = 1150;
const int heater_pin = 10;
const int debounce_delay = 50; // ms
const int buttonpin[4] = { 8, 7, 6, 9 };

float k_p, t_i, t_d; // PID parameters
unsigned long cycle_time;
float setpoint = 1150;
float cur_temp;
int heater_state;
int manual_control;
unsigned long debouncetime[4];
int lastbuttonstate[4];
int button[4];
int buttonpress[4];
int mode = MAIN;

ProgramSegment program[MAXSEGMENTS];
int nsegments = 0;
int editing_segment = 0;

SAppMenu menu, program_menu, setup_menu, segment_menu;
const char *menu_items[] = {
  "Program", "Setup",
};
char *program_menu_items[MAXSEGMENTS];
char *setup_menu_items[7];
char *segment_menu_items[4];

// prototypes & array of screen handler functions:
void main_display(void);
void menu_display(void);
void program_menu_display(void);
void setup_menu_display(void);
void segment_menu_display(void);
ScreenHandlerFunc screen_handler[] = {
  main_display, menu_display, program_menu_display, setup_menu_display, segment_menu_display,
};

void setup() {
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  sh1106_128x64_spi_init(3,4,5); // (3,4,5 = Reset,CS,DC)

  ssd1306_clearScreen();

  pinMode(heater_pin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(buttonpin[i], INPUT_PULLUP);
  }
  
  load_config();

  ssd1306_createMenu(&menu, menu_items, sizeof(menu_items)/sizeof(char*));
  ssd1306_createMenu(&program_menu, program_menu_items, sizeof(program_menu_items)/sizeof(char*));
}

void load_config() {
  // first, check that the EEPROM actually contains our config
  long magic;
  EEPROM.get(0, magic);
  if (magic != 0x6a706331)
    return;

  EEPROM.get(4, k_p);
  EEPROM.get(8, t_i);
  EEPROM.get(12, t_d);
  EEPROM.get(16, cycle_time);
  EEPROM.get(20, setpoint);
  EEPROM.get(24, nsegments);
  for (int i = 0; i < MAXSEGMENTS; i++) {
    EEPROM.get(26 + (i*7) + 0, program[i].type);
    EEPROM.get(26 + (i*7) + 1, program[i].target);
    EEPROM.get(26 + (i*7) + 3, program[i].duration);
  }
}

void save_config() {
  EEPROM.put(0, 0x6a706331);

  EEPROM.put(4, k_p);
  EEPROM.put(8, t_i);
  EEPROM.put(12, t_d);
  EEPROM.put(16, cycle_time);
  EEPROM.put(20, setpoint);
  EEPROM.put(24, nsegments);
  for (int i = 0; i < MAXSEGMENTS; i++) {
    EEPROM.put(26 + (i*7) + 0, program[i].type);
    EEPROM.put(26 + (i*7) + 1, program[i].target);
    EEPROM.put(26 + (i*7) + 3, program[i].duration);
  }
}

void loop() {
  int safe_to_operate = 1;

  // read & sanity-check temperature
  cur_temp = read_thermocouple();
  if (cur_temp < min_temp || cur_temp > max_temp) {
    safe_to_operate = 0;
    heater(0);
  }
  // TODO: if heater has been on for a while and temperature hasn't risen, assume thermocouple dislodged and set not-safe-to-operate

  if (safe_to_operate)
    pid_control();

  // check button states w/debounce
  read_buttons();

  screen_handler[mode]();
}

float read_thermocouple() {
  return 1000;
}

// https://www.arduino.cc/en/Tutorial/Debounce
void read_buttons() {
  for (int i = 0; i < 4; i++) {
    buttonpress[i] = 0;
    int state = digitalRead(buttonpin[i]);
    if (state != lastbuttonstate[i]) {
      debouncetime[i] = millis();
    }
    lastbuttonstate[i] = state;
    if ((millis() - debouncetime[i]) > debounce_delay) {
      if (button[i] != !state) {
        button[i] = !state; // button is pressed when pin is pulled to ground
        if (button[i])
          buttonpress[i] = 1;
      }
    }
  }
}

void heater(int state) {
  heater_state = state;
  digitalWrite(heater_pin, state);
}

void pid_control() {
  static float last_error, err_i;
  static unsigned long lastcheck;
  
  // TODO: if we're in a duty-cycle-on part: switch heater on, else switch it off
  heater(1); //is_duty_cycle_on);

  if (manual_control)
    return;

  unsigned long elapsed_ms = millis() - lastcheck;
  if (elapsed_ms < 1)
    elapsed_ms = 1;

  float error = setpoint - cur_temp;
  float err_d = error - last_error;
  float dutycycle_adjust = k_p * (error + err_i / t_i + err_d * t_d);
  err_d = (error - last_error) * 1000 / elapsed_ms;
  err_i += error / (1000 / elapsed_ms);
  last_error = error;
  // TODO: anti-windup

  lastcheck = millis();
}

void main_display() {
  static unsigned long lasthash;

  heater_state = (millis()/1000)%2;

  // rendering
  unsigned long screen_hash = 10*(int)cur_temp + 2*(int)setpoint + heater_state + 1;
  if (screen_hash != lasthash) {
    // temperatures
    char buf[32];
    sprintf(buf, "%d / %d", (int)cur_temp, (int)setpoint);
    ssd1306_clearScreen();
    ssd1306_setFixedFont(courier_new_font11x16_digits);
    ssd1306_printFixed(0,0, buf, STYLE_NORMAL);

    // output state
    if (heater_state)
      ssd1306_fillRect(123, 1, 127, 12);

    // TODO: if running a program, give info about the program

    lasthash = screen_hash;
  }

  if (buttonpress[OK]) {
    lasthash = 0;
    mode = MENU;
  }
  // TODO: up/down buttons alter setpoint, cancel button?
}

void menu_display() {
  static unsigned long redraw = 1;

  if (buttonpress[UP]) {
    redraw = 1;
    ssd1306_menuUp(&menu);
  }
  if (buttonpress[DOWN]) {
    redraw = 1;
    ssd1306_menuDown(&menu);
  }

  if (redraw) {
    ssd1306_clearScreen();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_showMenu(&menu);
    redraw = 0;
  }

  if (buttonpress[OK]) {
    redraw = 1;
    int sel = ssd1306_menuSelection(&menu);
    if (sel == 0) {
      setup_program_menu();
      mode = PROGRAM;
    } else {
      setup_setup_menu();
      mode = SETUP;
    }
  }
  if (buttonpress[CANCEL]) {
    redraw = 1;
    mode = MAIN;
  }
}

void program_menu_display() {
  static unsigned long redraw = 1;

  if (buttonpress[UP]) {
    redraw = 1;
    ssd1306_menuUp(&program_menu);
  }
  if (buttonpress[DOWN]) {
    redraw = 1;
    ssd1306_menuDown(&program_menu);
  }

  if (redraw) {
    ssd1306_clearScreen();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_showMenu(&program_menu);
    redraw = 0;
  }

  if (buttonpress[OK]) {
    redraw = 1;
    int sel = ssd1306_menuSelection(&program_menu);
    // sel=1..N are segments if N segments exist, first one is run/stop, and the final 2 options are "add segment" and "clear program"
    if (sel == 0) { // Run/Abort
      // TODO: this
    } else if (sel <= nsegments) {
      setup_segment_menu(sel-1);
      mode = SEGMENT;
    } else if (sel == nsegments+1) { // add segment
      if (nsegments < 32) {
        program[nsegments].type = STEP;
        program[nsegments].target = 100;
        program[nsegments].duration = 3600;
        setup_segment_menu(nsegments);
        mode = SEGMENT;
      }
    } else { // clear program
      // TODO: "are you sure?"
      nsegments = 0;
      save_config();
      setup_program_menu();
      program_menu.selection = 1;
    }
  }
  if (buttonpress[CANCEL]) {
    redraw = 1;
    mode = MENU;
  }
}

void setup_menu_display() {
  static unsigned long redraw = 1;

  if (buttonpress[UP]) {
    redraw = 1;
    ssd1306_menuUp(&setup_menu);
  }
  if (buttonpress[DOWN]) {
    redraw = 1;
    ssd1306_menuDown(&setup_menu);
  }

  if (redraw) {
    ssd1306_clearScreen();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_showMenu(&setup_menu);
    redraw = 0;
  }

  if (buttonpress[OK]) {
    redraw = 1;
    int sel = ssd1306_menuSelection(&setup_menu);
    // TODO: interact with setup menu
  }
  if (buttonpress[CANCEL]) {
    redraw = 1;
    mode = MENU;
  }
}

void segment_menu_display() {
  static unsigned long redraw = 1;

  if (buttonpress[UP]) {
    redraw = 1;
    ssd1306_menuUp(&segment_menu);
  }
  if (buttonpress[DOWN]) {
    redraw = 1;
    ssd1306_menuDown(&segment_menu);
  }

  if (redraw) {
    ssd1306_clearScreen();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_showMenu(&segment_menu);
    redraw = 0;
  }

  if (buttonpress[OK]) {
    redraw = 1;
    int sel = ssd1306_menuSelection(&segment_menu);
    if (sel == 0) { // type
      program[editing_segment].type = !program[editing_segment].type;
      setup_segment_menu(editing_segment);
      segment_menu.selection = sel;
    } else if (sel == 1) { // target
      // TODO: enter a mode where you can edit the number with up/down
    } else if (sel == 2) { // time
      // TODO: enter a mode where you can edit the number with up/down
    } else { // add/remove
      int added_segment = 0;
      if (editing_segment == nsegments) { // add
        added_segment = 1;
        nsegments++;
      } else { // remove
        // TODO: shift all the other segments along so that we just delete this one from the middle
        nsegments--;
      }
      save_config();
      redraw = 1;
      setup_program_menu();
      program_menu.selection = added_segment ? nsegments+1 : editing_segment+1;
      mode = PROGRAM;
    }
  }
  if (buttonpress[CANCEL]) {
    redraw = 1;
    mode = PROGRAM;
  }
}

void setup_program_menu() {
  static char buf[MAXSEGMENTS][24];
  
  program_menu_items[0] = "Run";
  for (int i = 0; i < nsegments; i++) {
    sprintf(buf[i], "%d. %s %d 1h30", i+1, program[i].type == RAMP ? "ramp" : "step", program[i].target);
    program_menu_items[i+1] = buf[i];
  }
  program_menu_items[nsegments+1] = "Add segment";
  program_menu_items[nsegments+2] = "Clear program";
  
  ssd1306_createMenu(&program_menu, program_menu_items, nsegments+3);
}

void setup_setup_menu() {
  setup_menu_items[0] = "Kp: 4";
  setup_menu_items[1] = "Ti: 3";
  setup_menu_items[2] = "Td: 0.1";
  setup_menu_items[3] = "Cyc. time: 4s";
  setup_menu_items[4] = "Min. time: 5s";
  setup_menu_items[5] = "Show state: on";
  setup_menu_items[6] = "Manual control: off";
  
  ssd1306_createMenu(&setup_menu, setup_menu_items, 7);
}

void setup_segment_menu(int seg) {
  static char buftype[16], buftarget[16], buftime[16];
  
  sprintf(buftype, "Type:    %s", program[seg].type == RAMP ? "ramp" : "step");
  sprintf(buftarget, "Target:  %d", program[seg].target);
  sprintf(buftime, "Time:    1h30");
  
  segment_menu_items[0] = buftype;
  segment_menu_items[1] = buftarget;
  segment_menu_items[2] = buftime;
  segment_menu_items[3] = seg == nsegments ? "Add" : "Remove";

  editing_segment = seg;
  
  ssd1306_createMenu(&segment_menu, segment_menu_items, 4);
}
