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

#define MAXSEGMENTS 16

typedef struct ProgramSegment {
  char type; // RAMP/STEP
  int target; // degrees C
  unsigned long duration; // seconds
} ProgramSegment;

enum segment_types { RAMP = 0, STEP };
enum display_modes { MAIN = 0, MENU, PROGRAM, SETUP, SEGMENT, EDITNUMBER, EDITTIME };
enum input_buttons { UP = 0, DOWN, OK, CANCEL };
enum edit_values { EDIT_KP = 1, EDIT_TI, EDIT_TD, EDIT_CYCTIME, EDIT_MINTIME, EDIT_TARGET, EDIT_SEGTIME, EDIT_SETPOINT };
typedef void (*ScreenHandlerFunc)(void);

const float min_temp = 5;
const float max_temp = 1150;
const char heater_pin = 10;
const int debounce_delay = 50; // ms
const char buttonpin[4] = { 8, 7, 6, 9 };

// don't edit these here, they get initialised in load_config()
float k_p, t_i, t_d; // PID parameters
unsigned long cycle_time;
unsigned long min_time;
float dutycycle;
float setpoint;
float cur_temp;
char heater_state;
char manual_control;
unsigned long debouncetime[4];
char lastbuttonstate[4];
char button[4];
char buttonpress[4];
char mode = MAIN;

char was_editing;
float editnumber_scale;
float editnumber_initial;
float editnumber_val;
char editnumber_mode;
char *editnumber_msg;

ProgramSegment program[MAXSEGMENTS];
char nsegments = 0;
char editing_segment = 0;

SAppMenu menu, program_menu, setup_menu, segment_menu;
const char *menu_items[] = {
  "Program", "Setup",
};
char *program_menu_items[MAXSEGMENTS];
char *setup_menu_items[8];
char *segment_menu_items[5];

// prototypes & array of screen handler functions:
void main_display(void);
void menu_display(void);
void program_menu_display(void);
void setup_menu_display(void);
void segment_menu_display(void);
void editnumber_display(void);
ScreenHandlerFunc screen_handler[] = {
  main_display, menu_display, program_menu_display, setup_menu_display, segment_menu_display, editnumber_display,
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
  // initialise defaults
  k_p = 3; t_i = 120; t_d = 30;
  cycle_time = 20000;
  setpoint = 50;
  nsegments = 0;
  
  // check that the EEPROM actually contains our config
  long magic;
  EEPROM.get(0, magic);
  if (magic != 0x6a706331)
    return;

  EEPROM.get(4, k_p);
  EEPROM.get(8, t_i);
  EEPROM.get(12, t_d);
  EEPROM.get(16, cycle_time);
  EEPROM.get(20, min_time);
  EEPROM.get(24, setpoint);
  EEPROM.get(28, nsegments);
  EEPROM.get(29, manual_control);
  for (int i = 0; i < MAXSEGMENTS; i++) {
    EEPROM.get(30 + (i*7) + 0, program[i].type);
    EEPROM.get(30 + (i*7) + 1, program[i].target);
    EEPROM.get(30 + (i*7) + 3, program[i].duration);
  }
}

void save_config() {
  EEPROM.put(0, 0x6a706331);

  EEPROM.put(4, k_p);
  EEPROM.put(8, t_i);
  EEPROM.put(12, t_d);
  EEPROM.put(16, cycle_time);
  EEPROM.put(20, min_time);
  EEPROM.put(24, setpoint);
  EEPROM.put(28, nsegments);
  EEPROM.put(29, manual_control);
  for (int i = 0; i < MAXSEGMENTS; i++) {
    EEPROM.put(30 + (i*7) + 0, program[i].type);
    EEPROM.put(30 + (i*7) + 1, program[i].target);
    EEPROM.put(30 + (i*7) + 3, program[i].duration);
  }
}

void loop() {
  int safe_to_operate = 1;

  // read & sanity-check temperature
  cur_temp = read_thermocouple();
  if (cur_temp < min_temp || cur_temp > max_temp)
    safe_to_operate = 0;
  // TODO: if heater has been on for a while and temperature hasn't risen, assume thermocouple dislodged and set not-safe-to-operate

  if (safe_to_operate) {
    if (!manual_control)
      pid_control();
    unsigned long cycle_offset = millis()%cycle_time;
    unsigned long ontime = dutycycle * cycle_time;
    if (ontime < min_time)
      ontime = 0;
    if (cycle_time-ontime < min_time)
      ontime = cycle_time;
    heater(cycle_offset < ontime);
  } else {
    heater(0);
  }

  read_buttons();

  screen_handler[mode]();
}

float read_thermocouple() {
  return 10;
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

  unsigned long elapsed_ms = millis() - lastcheck;
  if (elapsed_ms < 1)
    elapsed_ms = 1;

  float error = setpoint - cur_temp;
  float err_d = error - last_error;
  float dutycycle_adjust = k_p * (error + err_i / t_i + err_d * t_d);
  err_d = (error - last_error) * 1000 / elapsed_ms;
  err_i += error / (1000 / elapsed_ms);
  last_error = error;
  dutycycle += dutycycle_adjust;
  if (dutycycle < 0) dutycycle = 0;
  if (dutycycle > 1) dutycycle = 1;
  // TODO: anti-windup

  lastcheck = millis();
}

void main_display() {
  static unsigned long lasthash;

  // TODO: if (manual_control) then editing is editing dutycycle instead of setpoint

  if (was_editing) {
    setpoint = editnumber_val;
    save_config();
    lasthash = 0;
    was_editing = 0;
  }

  if (buttonpress[UP]) {
    edit_number(EDIT_SETPOINT, setpoint+1, 1, "Edit setpoint:");
    editnumber_initial = setpoint;
  }
  if (buttonpress[DOWN]) {
    edit_number(EDIT_SETPOINT, setpoint-1, 1, "Edit setpoint:");
    editnumber_initial = setpoint;
  }

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
}

void menu_display() {
  static char redraw = 1;

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
  static char redraw = 1;

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
  static char redraw = 1;

  if (was_editing) {
    switch (was_editing) {
      case EDIT_KP:
        k_p = editnumber_val;
        break;
    }
    save_config();
    int sel = setup_menu.selection;
    setup_setup_menu();
    setup_menu.selection = sel;
    was_editing = 0;
    redraw = 1;
  }

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
    switch (sel) {
      case 0: // k_p
        edit_number(EDIT_KP, k_p, 1, "Edit Kp:");
        redraw = 1;
        break;
      case 1: // t_i
        break;
      case 2: // t_d
        break;
      case 3: // cycle time
        break;
      case 4: // min. time
        break;
      case 5: // show state
        break;
      case 6: // manual control
        manual_control = !manual_control;
        break;
      case 7: // reset all
        // TODO: are you sure?
        EEPROM.put(0, 0); // overwrite magic number with 0 so that we re-initialise with defaults
        load_config();
        break;
    }
    redraw = 1;
    setup_setup_menu();
    setup_menu.selection = sel;
  }
  if (buttonpress[CANCEL]) {
    redraw = 1;
    mode = MENU;
  }
}

void segment_menu_display() {
  static char redraw = 1;

  // TODO: edit on a copy of the segment, and only commit it if it actually gets saved, so that we don't accidentally save unsaved changes

  if (was_editing) {
    switch (was_editing) {
      case EDIT_TARGET:
        program[editing_segment].target = editnumber_val;
        break;
    }
    int sel = segment_menu.selection;
    setup_segment_menu(editing_segment);
    segment_menu.selection = sel;
    was_editing = 0;
    redraw = 1;
  }

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
      edit_number(EDIT_TARGET, program[editing_segment].target, 1, "Edit target:");
      redraw = 1;
    } else if (sel == 2) { // time
      // TODO: enter a mode where you can edit the time with up/down
    } else if (sel == 3) { // add/save
      int added_segment = 0;
      if (editing_segment == nsegments) { // add
        added_segment = 1;
        nsegments++;
      }
      save_config();
      redraw = 1;
      setup_program_menu();
      program_menu.selection = added_segment ? nsegments+1 : editing_segment+1;
      mode = PROGRAM;
    } else { // remove
      nsegments--;
      // TODO: shift other segments along
      save_config();
      redraw = 1;
      setup_program_menu();
      program_menu.selection = editing_segment+1;
      mode = PROGRAM;
    }
  }
  if (buttonpress[CANCEL]) {
    redraw = 1;
    mode = PROGRAM;
  }
}

void editnumber_display() {
  static char redraw = 1;

  // TODO: when button is held down, move the value faster

  if (buttonpress[UP]) {
    editnumber_val += editnumber_scale;
    redraw = 1;
  }
  if (buttonpress[DOWN]) {
    editnumber_val -= editnumber_scale;
    redraw = 1;
  }

  if (redraw) {
    ssd1306_clearScreen();
    ssd1306_drawRect(4, 4, 123, 59);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_printFixed(8,8, editnumber_msg, STYLE_NORMAL);
    ssd1306_printFixed(8,24, ftoa(editnumber_val / editnumber_scale), STYLE_NORMAL);
    if (editnumber_val != editnumber_initial) {
      ssd1306_printFixed(8,40, "was", STYLE_NORMAL);
      ssd1306_printFixed(32,40, ftoa(editnumber_initial / editnumber_scale), STYLE_NORMAL);
    }
    redraw = 0;
  }

  if (buttonpress[OK]) {
    // return to previous mode
    mode = editnumber_mode;
    redraw = 1;
  }
  if (buttonpress[CANCEL]) {
    // undo changes and return to previous mode
    editnumber_val = editnumber_initial;
    mode = editnumber_mode;
    redraw = 1;
  }
}

void edit_number(char edittype, float val, float scale, char *msg) {
  was_editing = edittype;
  editnumber_initial = val;
  editnumber_val = val;
  editnumber_scale = scale;
  editnumber_mode = mode;
  editnumber_msg = msg;
  mode = EDITNUMBER;
}

void setup_program_menu() {
  static char buf[MAXSEGMENTS][24];
  
  program_menu_items[0] = "Run";
  for (int i = 0; i < nsegments; i++) {
    sprintf(buf[i], "%d. %s %d %s", i+1, program[i].type == RAMP ? "ramp" : "step", program[i].target, timetoa(program[i].duration*1000));
    program_menu_items[i+1] = buf[i];
  }
  program_menu_items[nsegments+1] = "Add segment";
  program_menu_items[nsegments+2] = "Clear program";
  
  ssd1306_createMenu(&program_menu, program_menu_items, nsegments+3);
}

void setup_setup_menu() {
  static char bufkp[16], bufti[16], buftd[16], bufcycletime[24], bufmintime[24], bufshowstate[16], bufmanual[24];
  
  sprintf(bufkp, "Kp: %s", ftoa(k_p));
  sprintf(bufti, "Ti: %s", timetoa(t_i*1000));
  sprintf(buftd, "Td: %s", timetoa(t_d*1000));
  sprintf(bufcycletime, "Cyc time: %s", timetoa(cycle_time));
  sprintf(bufmintime, "Min time: %s", timetoa(min_time));
  sprintf(bufshowstate, "Show state: off");
  sprintf(bufmanual, "Manual control: %s", manual_control ? "on" : "off");

  setup_menu_items[0] = bufkp;
  setup_menu_items[1] = bufti;
  setup_menu_items[2] = buftd;
  setup_menu_items[3] = bufcycletime;
  setup_menu_items[4] = bufmintime;
  setup_menu_items[5] = bufshowstate;
  setup_menu_items[6] = bufmanual;
  setup_menu_items[7] = "Reset all";
  
  ssd1306_createMenu(&setup_menu, setup_menu_items, 8);
}

void setup_segment_menu(int seg) {
  static char buftype[16], buftarget[16], buftime[16];
  
  sprintf(buftype, "Type:    %s", program[seg].type == RAMP ? "ramp" : "step");
  sprintf(buftarget, "Target:  %d", program[seg].target);
  sprintf(buftime, "Time:    %s", timetoa(program[seg].duration*1000));
  
  segment_menu_items[0] = buftype;
  segment_menu_items[1] = buftarget;
  segment_menu_items[2] = buftime;
  segment_menu_items[3] = seg == nsegments ? "Add" : "Save";
  segment_menu_items[4] = "Remove";

  editing_segment = seg;
  
  ssd1306_createMenu(&segment_menu, segment_menu_items, 4+(seg<nsegments));
}

// format "f" as a string, return pointer to static buffer
// formatted as integer if >= 10, or 1 decimal place if < 10
char *ftoa(float f) {
  static char buf[16];
  char *p = buf;
  
  if (f < 0) {
    *p++ = '-';
    f = -f;
  }

  if (f >= 10) {
    sprintf(p, "%d", (int)f);
  } else {
    sprintf(p, "%d.%d", (int)f, ((int)(f*10))%10);
  }

  return buf;
}

// format "ms" as a string, return pointer to static buffer
char *timetoa(unsigned long ms) {
  static char buf[16];

  if (ms >= 86400000) {
    sprintf(buf, "%lud%02luh", ms/86400000, (ms%86400000)/3600000);
  } else if (ms >= 3600000) {
    sprintf(buf, "%luh%02lum", ms/3600000, (ms%3600000)/60000);
  } else if (ms >= 60000) {
    sprintf(buf, "%lum%02lus", ms/60000, (ms%60000)/1000);
  } else {
    sprintf(buf, "%ss", ftoa(((float)ms)/1000));
  }
  
  return buf;
}
