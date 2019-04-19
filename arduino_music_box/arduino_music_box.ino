//configuration of the sample
uint16_t s_noteFreqEx[ 12 ] = { 0xC83F, 0xBD02, 0xB265, 0xA863, 0x9EEF, 0x9604, 0x8D98, 0x85A5, 0x7E25, 0x7711, 0x7062, 0x6A13 };
#define HXMIDIPLAYER_CHANNELS_COUNT         5
#define PLAYER_FREQ_SHR               13
#define PLAYER_COUNTER_MAX_VAL        27262976
bool chanused[HXMIDIPLAYER_CHANNELS_COUNT];
byte playednotes[HXMIDIPLAYER_CHANNELS_COUNT];
#include "musicbox.h"
#include "synth.h"
ISR(TIMER2_COMPA_vect) {
  //handle adding of the sample counter and mixing of the channels
  Player_TimerFunc();
}
void setup() {
  pinMode(6, 1);
  pinMode(13, 1);
  TCCR2A = B00000010;
  TCCR2B = B00000010;
  OCR2A = 111;
  TIMSK2 = 0x02;
  //timer2 set to CTC mode with a COMPA vect of 18khz
  TCCR0A = B10000011;
  TCCR0B = 1;
  //timer0 set to fast pwm at 64khz
  Serial.begin(31250);
  //begin the serial port to midi baudrate
}
void handlechans(byte mididata) {
  uint32_t maxValue;
  byte big;
  //check if the note has already been played on any of the channels
  //if true replay it on the same channel and return
  for (int a = 0; a != HXMIDIPLAYER_CHANNELS_COUNT; a++) {
    if (mididata == playednotes[a]) {
      s_playerState.m_channelState[ a ].m_counter = 0;
      s_playerState.m_channelState[ a ].m_counterAdd = Player_GetNoteFreqAdd( mididata );
      return;
    }
  }
  //check which channel is free so that we can play a note on it
  for (int a = 0; a != HXMIDIPLAYER_CHANNELS_COUNT; a++) {
    if (!chanused[a]) {
      s_playerState.m_channelState[ a ].m_counter = 0;
      s_playerState.m_channelState[ a ].m_counterAdd = Player_GetNoteFreqAdd( mididata );
      chanused[a] = 1;
      playednotes[a] = mididata;
      return;
    }
   //check which note is the oldest that has been played
    if (s_playerState.m_channelState[ a ].m_counter > maxValue) {
      maxValue = s_playerState.m_channelState[ a ].m_counter;
      big = a;
    }
  }
  //if no channel is free overwrite the oldest one
  s_playerState.m_channelState[ big ].m_counter = 0;
  s_playerState.m_channelState[ big ].m_counterAdd = Player_GetNoteFreqAdd( mididata );
  chanused[big] = 1;
  playednotes[big] = mididata;

}
byte aa;
byte midibyte[3];
bool prev;
void loop() {
  while (Serial.available()) {
    //midi stuff
    midibyte[aa] = Serial.read();
    if (midibyte[aa] > 0x7F && aa != 0) {
      midibyte[0] = midibyte[aa];
      aa = 0;
    }
    aa++;
    if (aa == 3) {
      aa = 0;
      if ((midibyte[0] & 0xf0) == 0x90 && midibyte[2] != 0 && (midibyte[0] & 0x0f) != 0x09)handlechans(midibyte[1]);
    }
  }
  if(prev!=chanused[0]){
    digitalWrite(13,chanused[0]);
    prev=chanused[0];
  }

}
