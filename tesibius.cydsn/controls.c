#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "project.h"
#include "debug.h"
#include "hardware_if.h"
#include "controls.h"
#include "leds.h"

#define TOP_ROW_FLASH_ADDR  (CY_FLASH_BASE + CY_FLASH_SIZE - CY_FLASH_SIZEOF_ROW)
#define PRESET_MAGIC        0xDEAFBABE


typedef struct 
{
    uint8 gainChannel; // 1
    KnobValues knobs; // 9 bytes
} AmpState;
static AmpState _state;

typedef union
{
    struct
    {
        uint32      magic;
        AmpState    presets[NUM_PRESETS];
    };
    uint8 row_data[CY_FLASH_SIZEOF_ROW]; // Row is 128 bytes, presets are 10 bytes + 4 byte magic = 84 bytes
} PresetStore;

static const PresetStore CYCODE factoryPresets = 
{
    .magic   = PRESET_MAGIC,
    .presets = 
    {//                     gain  bite  bass  mid   treb  pres  levl  revb  mast
        { .knobs.values = { 0x7b, 0x80, 0xf3, 0xbe, 0xd9, 0x80, 0xff, 0x00, 0x17 }, .gainChannel = 0x00 },
        { .knobs.values = { 0x7b, 0x80, 0xe6, 0xe4, 0xe1, 0x80, 0x33, 0x74, 0x11 }, .gainChannel = 0x00 },
        { .knobs.values = { 0x9c, 0x80, 0xa7, 0xff, 0xff, 0x80, 0x00, 0xdf, 0x1b }, .gainChannel = 0x01 },
        { .knobs.values = { 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20 }, .gainChannel = 0x01 },
        { .knobs.values = { 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20 }, .gainChannel = 0x00 },
        { .knobs.values = { 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20 }, .gainChannel = 0x00 },
        { .knobs.values = { 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20 }, .gainChannel = 0x01 },
        { .knobs.values = { 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x20 }, .gainChannel = 0x01 },
    },
};
static const PresetStore * const flashPresets = (const PresetStore *)(TOP_ROW_FLASH_ADDR);
static int currentPreset = -1;


static const char* CYCODE knobNames[]=
{
    "GAIN", "BITE", "BASS", "MID", "TREBLE",
    "PRESENCE", "LEVEL", "REVERB", "MASTER"
};

void InitControls(void)
{
    memset(&_state, 0, sizeof(_state));
    InitLEDs(LED_CHANNEL_1);
    HalSetGain(_state.gainChannel);
}

void SetControl(Control id, uint8_t value)
{
    assert( (id < NUM_CTLS) && (id >= 0));
    HalSetControl(id, value);
    _state.knobs.values[id] = value; // Track current settings.
 
}

void ControlsJiffy(uint32_t jiffyCount)
{
    KnobValues knobs;
    bool haveChanged = HalGetKnobs(&knobs);
    if (haveChanged)
    {
        
        for (int i = CTL_GAIN; i< NUM_CTLS; i++)
        {
            if (_state.knobs.values[i] != knobs.values[i])
            {
                DBG_PRINTF("Control has changed - (ctl - %s, value - %u, jiffy - %u\r\n",
                    knobNames[i], knobs.values[i], jiffyCount);
                SetControl(i, knobs.values[i]);
            }
        }
    }
}

uint8 GetGainChannel()
{
    return _state.gainChannel;
}

void SetGainChannel(unsigned int gainChannel)
{
    HalSetGain(gainChannel);
    int leds = (gainChannel == 0 ? LED_CHANNEL_1 : LED_CHANNEL_2);
    SetLEDs(leds, LED_CHANNEL_1 | LED_CHANNEL_2);
    _state.gainChannel = gainChannel;
}
void ToggleGainChannel()
{
    SetGainChannel(_state.gainChannel ^ 0x01);
}

#define PRESET_LEDS (LED_PRESET_1 | LED_PRESET_2 | LED_PRESET_3 | LED_PRESET_4 | LED_PRESET_5 | LED_PRESET_6 | LED_PRESET_7 | LED_PRESET_8)

void SavePreset(int preset)
{
    assert(preset>=0 && preset<NUM_PRESETS);
    const PresetStore *pCurrent = flashPresets;
    if (pCurrent->magic != PRESET_MAGIC)
    {
        pCurrent = & factoryPresets;
    }
    PresetStore newPresets = *pCurrent;
    newPresets.presets[preset] = _state;
    const uint32 status = CySysFlashWriteRow(CY_FLASH_NUMBER_ROWS - 1, newPresets.row_data);
    if (status != CY_SYS_FLASH_SUCCESS)
    {
        DBG_PRINTF("FLASH write failed!\r\n");
    }
    else
    {
        DBG_PRINTF("Saved preset %d\r\n", preset + 1);
        currentPreset = preset;
        SetLEDs(LED_PRESET_1<<preset, PRESET_LEDS);
        // RunAnimation(SetChannel);
    }
}

void LoadPreset(int preset)
{
    assert(preset>=0 && preset<NUM_PRESETS);
    const PresetStore *pStore = flashPresets;
    if (pStore->magic != PRESET_MAGIC)
    {
        pStore = &factoryPresets;
    }
   
    /*
     * Quiet things up before we load new settings. 
     * Will set master volume at the end
    */
    SetControl(CTL_MASTER, 0);

    const AmpState *pNewState = &pStore->presets[preset];
    SetGainChannel(pNewState->gainChannel);
    for (int ctlIndex=0; ctlIndex< NUM_CTLS; ctlIndex++)
    {
        SetControl(ctlIndex, pNewState->knobs.values[ctlIndex]);
    }
    currentPreset = preset;
    SetLEDs(LED_PRESET_1<<preset, PRESET_LEDS);
}