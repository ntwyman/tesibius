/* ========================================
 *
 * Copyright Copyright Jonny Reckless & Nick Twyman, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * Implementation based on:
 *     https://www.midi.org/specifications/item/table-1-summary-of-midi-message
 * and http://mumia.art.pte.hu/mami/tartalom/2016_elokeszito/tananyagok/midi/szeml%C3%A9ltet%C3%A9s/mbytes.html
 * I think the current implementation is simplistic, because i think you 
 * ========================================
*/
#include <project.h>
#include "scheduler.h"
#include "midi.h"
#include "controls.h"
#include "debug.h"

#define CONTROL_BIT 0x0080 // Midi control bytes have the top bit set/
#define NOTE_OFF 0x08
#define NOTE_ON 0x09
#define POLY_PRESSURE 0x0A
#define CONTROL_CHANGE 0x0B
#define PROGRAM_CHANGE 0x0C
#define CHANNEL_PRESSURE 0x0D
#define PITCH_BEND 0x0E
#define SYSTEM_MESSAGE 0x0F

enum MidiState_
{
    STATE_WAITING,  // Starting up
    SKIP_DATA_BYTES, // Ignore data bytes
    CC_FIRST_BYTE,
    CC_NEXT_BYTE,
    PC_DATA_BYTE,
    STATE_ERROR // Ignore until the next control byte
};

struct MidiTask_
{
    Task base_task;
    enum MidiState_ state;
    uint32 control;
    int listening;
};

static struct MidiTask_ midi_task_;

CY_ISR(MidiDataReadyIsr)
{
    ScheduleTask(&midi_task_.base_task);
    isr_uart_midi_Disable();
}

static void
MidiTaskRunner_(Task* task)
{
    struct MidiTask_* self = (struct MidiTask_*)task;
    for (;;)
    {
        uint32 rx = uart_midi_UartGetByte();
        if (rx & uart_debug_UART_RX_ERROR_MASK)  // Reached the end
        {
            isr_uart_debug_ClearPending();
            isr_uart_midi_Enable();  // re-enable the interrupt to wake us up when more data arrives
            break;
        }
        DBG_PRINTF("Received midi byte %02x\r\n", (uint16)(rx & 0xff));
        if (rx & CONTROL_BIT)
        {
            uint32 top_nibble = (rx >> 4) & 0x000f;            
            uint32 bottom_nibble = rx & 0x000f;
            switch (top_nibble)
            {
                case NOTE_OFF:
                case NOTE_ON:
                case POLY_PRESSURE:
                case PITCH_BEND:
                case CHANNEL_PRESSURE:
                    self->state = SKIP_DATA_BYTES;
                    break;                
                case CONTROL_CHANGE:
                    DBG_PRINTF("CONTROL_CHANGE: Channel %02x\r\n", bottom_nibble);
                    self->listening = (bottom_nibble == MIDI_CHANNEL);                     
                    self->state = CC_FIRST_BYTE;
                    break;
                case PROGRAM_CHANGE:
                    DBG_PRINTF("PROGRAM_CHANGE: Channel %02x\r\n", bottom_nibble);
                    self->listening = (bottom_nibble == MIDI_CHANNEL);                     
                    self->state = PC_DATA_BYTE;
                    break;
                case SYSTEM_MESSAGE:
                default:
                    DBG_PRINTF("Unexpected top Nibble %02x\r\n", (uint8)top_nibble);
                    self->state = STATE_ERROR;
            }
        }
        else  // It's a data byte
        {
            switch (self->state)
            {
                case STATE_WAITING:
                    DBG_PRINTF("SW: Unexpected data byte - setting state to Midi.Error\r\n");
                    self->state = STATE_ERROR;
                    break;
                case STATE_ERROR:
                    DBG_PRINTF("SE: Data byte seen in error state - dropping\r\n");
                    break;
                case SKIP_DATA_BYTES:
                    DBG_PRINTF("STDB: Skipping byte\r\n");
                    break;
                case CC_FIRST_BYTE:
                    DBG_PRINTF("CCFB: Control Change - Control %02x\r\n", (uint8)rx);
                    self->control = rx;
                    self->state = CC_NEXT_BYTE;
                    break;
                case CC_NEXT_BYTE:
                    DBG_PRINTF("CCFB: Control Change - Control %02x, Value %02x\r\n", (uint8)self->control, (uint8)rx);
                    self->state = CC_FIRST_BYTE;
                    break;
                case PC_DATA_BYTE:
                    DBG_PRINTF("PCDB: Program Change - Value %02x\r\n", (uint8)rx);
                    if (rx < NUM_PRESETS)
                    {
                        LoadPreset((int)rx);
                    }
                    break;
                default:
                    DBG_PRINTF("????: Data byte in unexpected state %04x, Value %02x\r\n", (uint8)self->control, (uint8)rx);
            }
        }
    }
}

void
MidiInit()
{
    InitTask(&midi_task_.base_task, MidiTaskRunner_);
    midi_task_.state = STATE_WAITING;
    midi_task_.control = 0;
    midi_task_.listening = 0;
    isr_uart_midi_StartEx(MidiDataReadyIsr);
    uart_midi_Start();
}
/* [] END OF FILE */