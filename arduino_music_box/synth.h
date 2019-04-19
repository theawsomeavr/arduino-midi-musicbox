typedef struct
{
  uint32_t    m_counter;      //sample index counter
  uint16_t    m_counterAdd;   //0 - off, 1 - drum, >0 - add value for counter
} TChannelState;

typedef struct
{

  uint16_t                            m_eventCounter;

  //255Hz counter
  //initally writeln ENVELOPE_SKIP_MAX
  //decreased every tick
  //when reaches 0, envelope index on all channels should increase
  uint8_t                             m_envelopeSkipCounter;

  ///Syntezer channels states
  TChannelState                       m_channelState[ HXMIDIPLAYER_CHANNELS_COUNT ];
} TPlayerState;

///=====================================================
///=====================================================
///Player state
static TPlayerState s_playerState =
{


};


///=====================================================
///=====================================================
uint16_t inline Player_GetNoteFreqAdd( uint8_t _noteNumber )
{
  uint8_t noteIndex;
  uint8_t noteDiv;
  noteIndex = 132 - 1 - _noteNumber;
  noteDiv =   noteIndex / 12;  //how many octaves down
  noteIndex = noteIndex - 12 * noteDiv;
  return s_noteFreqEx[ noteIndex ] >> noteDiv;
}
void Player_TimerFunc()
{
  uint8_t sample;
  uint8_t i;
  TChannelState* pState;

  sample = 0x80;
  
  pState = &s_playerState.m_channelState[ 0 ];

  for ( i = 0; i < HXMIDIPLAYER_CHANNELS_COUNT; i ++ )
  {
    if ( pState->m_counter < PLAYER_COUNTER_MAX_VAL )
    {
      pState->m_counter += pState->m_counterAdd;
    
    }
    else {
      chanused[i] = 0;
      playednotes[i] = 0;
    }
    sample += pgm_read_byte(s_sample + (pState->m_counter >> (PLAYER_FREQ_SHR -3)));


    pState++;
  }

  OCR0A = sample ;
}


