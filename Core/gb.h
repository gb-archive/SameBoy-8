#ifndef GB_h
#define GB_h
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "apu.h"
#include "save_struct.h"
#include "symbol_hash.h"


#define GB_STRUCT_VERSION 9

enum {
    GB_REGISTER_AF,
    GB_REGISTER_BC,
    GB_REGISTER_DE,
    GB_REGISTER_HL,
    GB_REGISTER_SP,
    GB_REGISTERS_16_BIT /* Count */
};

/* Todo: Actually use these! */
enum {
    GB_CARRY_FLAG = 16,
    GB_HALF_CARRY_FLAG = 32,
    GB_SUBSTRACT_FLAG = 64,
    GB_ZERO_FLAG = 128,
};

#define GB_MAX_IR_QUEUE 256

enum {
    /* Joypad and Serial */
    GB_IO_JOYP       = 0x00, // Joypad (R/W)
    GB_IO_SB         = 0x01, // Serial transfer data (R/W)
    GB_IO_SC         = 0x02, // Serial Transfer Control (R/W)

    /* Missing */

    /* Timers */
    GB_IO_DIV        = 0x04, // Divider Register (R/W)
    GB_IO_TIMA       = 0x05, // Timer counter (R/W)
    GB_IO_TMA        = 0x06, // Timer Modulo (R/W)
    GB_IO_TAC        = 0x07, // Timer Control (R/W)

    /* Missing */

    GB_IO_IF         = 0x0f, // Interrupt Flag (R/W)

    /* Sound */
    GB_IO_NR10       = 0x10, // Channel 1 Sweep register (R/W)
    GB_IO_NR11       = 0x11, // Channel 1 Sound length/Wave pattern duty (R/W)
    GB_IO_NR12       = 0x12, // Channel 1 Volume Envelope (R/W)
    GB_IO_NR13       = 0x13, // Channel 1 Frequency lo (Write Only)
    GB_IO_NR14       = 0x14, // Channel 1 Frequency hi (R/W)
    GB_IO_NR21       = 0x16, // Channel 2 Sound Length/Wave Pattern Duty (R/W)
    GB_IO_NR22       = 0x17, // Channel 2 Volume Envelope (R/W)
    GB_IO_NR23       = 0x18, // Channel 2 Frequency lo data (W)
    GB_IO_NR24       = 0x19, // Channel 2 Frequency hi data (R/W)
    GB_IO_NR30       = 0x1a, // Channel 3 Sound on/off (R/W)
    GB_IO_NR31       = 0x1b, // Channel 3 Sound Length
    GB_IO_NR32       = 0x1c, // Channel 3 Select output level (R/W)
    GB_IO_NR33       = 0x1d, // Channel 3 Frequency's lower data (W)
    GB_IO_NR34       = 0x1e, // Channel 3 Frequency's higher data (R/W)

    /* Missing */

    GB_IO_NR41       = 0x20, // Channel 4 Sound Length (R/W)
    GB_IO_NR42       = 0x21, // Channel 4 Volume Envelope (R/W)
    GB_IO_NR43       = 0x22, // Channel 4 Polynomial Counter (R/W)
    GB_IO_NR44       = 0x23, // Channel 4 Counter/consecutive, Inital (R/W)
    GB_IO_NR50       = 0x24, // Channel control / ON-OFF / Volume (R/W)
    GB_IO_NR51       = 0x25, // Selection of Sound output terminal (R/W)
    GB_IO_NR52       = 0x26, // Sound on/off

    /* Missing */

    GB_IO_WAV_START  = 0x30, // Wave pattern start
    GB_IO_WAV_END    = 0x3f, // Wave pattern end

    /* Graphics */
    GB_IO_LCDC       = 0x40, // LCD Control (R/W)
    GB_IO_STAT       = 0x41, // LCDC Status (R/W)
    GB_IO_SCY        = 0x42, // Scroll Y (R/W)
    GB_IO_SCX        = 0x43, // Scroll X (R/W)
    GB_IO_LY         = 0x44, // LCDC Y-Coordinate (R)
    GB_IO_LYC        = 0x45, // LY Compare (R/W)
    GB_IO_DMA        = 0x46, // DMA Transfer and Start Address (W)
    GB_IO_BGP        = 0x47, // BG Palette Data (R/W) - Non CGB Mode Only
    GB_IO_OBP0       = 0x48, // Object Palette 0 Data (R/W) - Non CGB Mode Only
    GB_IO_OBP1       = 0x49, // Object Palette 1 Data (R/W) - Non CGB Mode Only
    GB_IO_WY         = 0x4a, // Window Y Position (R/W)
    GB_IO_WX         = 0x4b, // Window X Position minus 7 (R/W)
    // Has some undocumented compatibility flags written at boot.
    // Unfortunately it is not readable or writable after boot has finished, so research of this
    // register is quite limited. The value written to this register, however, can be controlled
    // in some cases.
    GB_IO_DMG_EMULATION = 0x4c,

    /* General CGB features */
    GB_IO_KEY1       = 0x4d, // CGB Mode Only - Prepare Speed Switch

    /* Missing */

    GB_IO_VBK        = 0x4f, // CGB Mode Only - VRAM Bank
    GB_IO_BIOS       = 0x50, // Write to disable the BIOS mapping

    /* CGB DMA */
    GB_IO_HDMA1      = 0x51, // CGB Mode Only - New DMA Source, High
    GB_IO_HDMA2      = 0x52, // CGB Mode Only - New DMA Source, Low
    GB_IO_HDMA3      = 0x53, // CGB Mode Only - New DMA Destination, High
    GB_IO_HDMA4      = 0x54, // CGB Mode Only - New DMA Destination, Low
    GB_IO_HDMA5      = 0x55, // CGB Mode Only - New DMA Length/Mode/Start

    /* IR */
    GB_IO_RP         = 0x56, // CGB Mode Only - Infrared Communications Port

    /* Missing */

    /* CGB Paletts */
    GB_IO_BGPI       = 0x68, // CGB Mode Only - Background Palette Index
    GB_IO_BGPD       = 0x69, // CGB Mode Only - Background Palette Data
    GB_IO_OBPI       = 0x6a, // CGB Mode Only - Sprite Palette Index
    GB_IO_OBPD       = 0x6b, // CGB Mode Only - Sprite Palette Data

    // 1 is written for DMG ROMs on a CGB. Does not appear to have an effect.
    GB_IO_DMG_EMULATION_INDICATION   = 0x6c, // (FEh) Bit 0 (Read/Write)

    /* Missing */

    GB_IO_SVBK       = 0x70, // CGB Mode Only - WRAM Bank
    GB_IO_UNKNOWN2   = 0x72, // (00h) - Bit 0-7 (Read/Write)
    GB_IO_UNKNOWN3   = 0x73, // (00h) - Bit 0-7 (Read/Write)
    GB_IO_UNKNOWN4   = 0x74, // (00h) - Bit 0-7 (Read/Write) - CGB Mode Only
    GB_IO_UNKNOWN5   = 0x75, // (8Fh) - Bit 4-6 (Read/Write)
    GB_IO_PCM_12     = 0x76, // Channels 1 and 2 amplitudes
    GB_IO_PCM_34     = 0x77, // Channels 3 and 4 amplitudes
    GB_IO_UNKNOWN8   = 0x7F, // Unknown, write only
};

#define LCDC_PERIOD 70224
#define CPU_FREQUENCY 0x400000
#define DIV_CYCLES (0x100)
#define INTERNAL_DIV_CYCLES (0x400)
#define FRAME_LENGTH 16742706 // in nanoseconds

typedef enum {
    GB_LOG_BOLD = 1,
    GB_LOG_DASHED_UNDERLINE = 2,
    GB_LOG_UNDERLINE = 4,
    GB_LOG_UNDERLINE_MASK =  GB_LOG_DASHED_UNDERLINE | GB_LOG_UNDERLINE
} GB_log_attributes;

struct GB_gameboy_s;
typedef struct GB_gameboy_s GB_gameboy_t;
typedef void (*GB_vblank_callback_t)(GB_gameboy_t *gb);
typedef void (*GB_log_callback_t)(GB_gameboy_t *gb, const char *string, GB_log_attributes attributes);
typedef char *(*GB_input_callback_t)(GB_gameboy_t *gb);
typedef uint32_t (*GB_rgb_encode_callback_t)(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b);
typedef void (*GB_infrared_callback_t)(GB_gameboy_t *gb, bool on, long cycles_since_last_update);

typedef struct {
    enum {
        GB_NO_MBC,
        GB_MBC1,
        GB_MBC2,
        GB_MBC3,
        GB_MBC4, // Does this exist???
        GB_MBC5,
    } mbc_type;
    bool has_ram;
    bool has_battery;
    bool has_rtc;
    bool has_rumble;
} GB_cartridge_t;

typedef struct {
    bool state;
    long delay;
} GB_ir_queue_item_t;

struct GB_breakpoint_s;
struct GB_watchpoint_s;

/* When state saving, each section is dumped independently of other sections.
   This allows adding data to the end of the section without worrying about future compatibility.
   Some other changes might be "safe" as well.
   This struct is not packed, but dumped sections exclusively use types that have the same alignment in both 32 and 64
   bit platforms. */

/* We make sure bool is 1 for cross-platform save state compatibility. */
/* Todo: We might want to typedef our own bool if this prevents SameBoy from working on specific platforms. */
_Static_assert(sizeof(bool) == 1, "sizeof(bool) != 1");

enum {
    GB_TIMA_RUNNING = 0,
    GB_TIMA_RELOADING = 1,
    GB_TIMA_RELOADED = 2
};

typedef struct GB_gameboy_s {
    GB_SECTION(header,
        /* The magic makes sure a state file is:
            - Indeed a SameBoy state file.
            - Has the same endianess has the current platform. */
        uint32_t magic;
        /* The version field makes sure we don't load save state files with a completely different structure.
           This happens when struct fields are removed/resized in an backward incompatible manner. */
        uint32_t version;
    );

    GB_SECTION(core_state,
        /* Registers */
        uint16_t pc;
        uint16_t registers[GB_REGISTERS_16_BIT];
        uint8_t ime;
        uint8_t interrupt_enable;
        uint8_t cgb_ram_bank;

        /* CPU and General Hardware Flags*/
        bool cgb_mode;
        bool is_cgb;
        bool cgb_double_speed;
        bool halted;
        bool stopped;
        bool boot_rom_finished;
        bool ime_toggle; /* ei (and di in CGB) have delayed effects.*/

        /* Misc state*/
        /* IR */
        bool infrared_input;
    );

    /* DMA and HDMA */
    GB_SECTION(dma,
        bool hdma_on;
        bool hdma_on_hblank;
        uint8_t hdma_steps_left;
        uint16_t hdma_cycles;
        uint16_t hdma_current_src, hdma_current_dest;

        uint8_t dma_steps_left;
        uint8_t dma_current_dest;
        uint16_t dma_current_src;
        int16_t dma_cycles;
        bool is_dma_restarting;
    );
    
    /* MBC */
    GB_SECTION(mbc,
        uint16_t mbc_rom_bank;
        uint8_t mbc_ram_bank;
        uint32_t mbc_ram_size;
        bool mbc_ram_enable;
        union {
            struct {
                uint8_t bank_low:5;
                uint8_t bank_high:3;
                uint8_t mode:1;
            } mbc1;

            struct {
                uint8_t rom_bank:4;
            } mbc2;

            struct {
                uint8_t rom_bank:7;
                uint8_t padding:1;
                uint8_t ram_bank:4;
            } mbc3;

            struct {
                uint8_t rom_bank_low;
                uint8_t rom_bank_high:1;
                uint8_t ram_bank:4;
            } mbc5;
        };
        uint16_t mbc_rom0_bank; /* For some MBC1 wirings. */
    );


    /* HRAM and HW Registers */
    GB_SECTION(hram,
        uint8_t hram[0xFFFF - 0xFF80];
        uint8_t io_registers[0x80];
    );

    /* Timing */
    GB_SECTION(timing,
        int64_t last_vblank;
        uint32_t display_cycles;
        uint32_t div_cycles;
        GB_PADDING(uint32_t, tima_cycles);
        GB_PADDING(uint32_t, dma_cycles);
        GB_aligned_double apu_cycles;
        uint8_t tima_reload_state; /* After TIMA overflows, it becomes 0 for 4 cycles before actually reloading. */
        uint16_t serial_cycles;
    );

    /* APU */
    GB_SECTION(apu,
        GB_apu_t apu;
    );

    /* RTC */
    GB_SECTION(rtc,
        union {
            struct {
                uint8_t seconds;
                uint8_t minutes;
                uint8_t hours;
                uint8_t days;
                uint8_t high;
            };
            uint8_t data[5];
        } rtc_real, rtc_latched;
        time_t last_rtc_second;
        bool rtc_latch;
    );

    /* Video Display */
    GB_SECTION(video,
        uint32_t vram_size; // Different between CGB and DMG
        uint8_t cgb_vram_bank;
        uint8_t oam[0xA0];
        uint8_t background_palletes_data[0x40];
        uint8_t sprite_palletes_data[0x40];
        uint32_t background_palletes_rgb[0x20];
        uint32_t sprite_palletes_rgb[0x20];
        int16_t previous_lcdc_x;
        uint8_t padding;
        bool effective_window_enabled;
        uint8_t effective_window_y;
        bool stat_interrupt_line;
        uint8_t effective_scx;
    );

    /* Unsaved data. This includes all pointers, as well as everything that shouldn't be on a save state */

    /* ROM */
    uint8_t *rom;
    uint32_t rom_size;
    const GB_cartridge_t *cartridge_type;
    enum {
        GB_STANDARD_MBC1_WIRING,
        GB_MBC1M_WIRING,
    } mbc1_wiring;

    /* Various RAMs */
    uint8_t *ram;
    uint8_t *vram;
    uint8_t *mbc_ram;

    /* I/O */
    uint32_t *screen;
    GB_sample_t *audio_buffer;
    bool keys[8];

    /* Audio Specific */
    unsigned int buffer_size;
    unsigned int sample_rate;
    unsigned int audio_position;
    bool audio_stream_started; // detects first copy request to minimize lag
    volatile bool audio_copy_in_progress;

    /* Callbacks */
    void *user_data;
    GB_log_callback_t log_callback;
    GB_input_callback_t input_callback;
    GB_input_callback_t async_input_callback;
    GB_rgb_encode_callback_t rgb_encode_callback;
    GB_vblank_callback_t vblank_callback;
    GB_infrared_callback_t infrared_callback;

    /* IR */
    long cycles_since_ir_change;
    long cycles_since_input_ir_change;
    GB_ir_queue_item_t ir_queue[GB_MAX_IR_QUEUE];
    size_t ir_queue_length;

    /*** Debugger ***/
    volatile bool debug_stopped;
    bool debug_fin_command, debug_next_command;

    /* Breakpoints */
    uint16_t n_breakpoints;
    struct GB_breakpoint_s *breakpoints;

    /* SLD (Todo: merge with backtrace) */
    bool stack_leak_detection;
    int debug_call_depth;
    uint16_t sp_for_call_depth[0x200]; /* Should be much more than enough */
    uint16_t addr_for_call_depth[0x200];

    /* Backtrace */
    unsigned int backtrace_size;
    uint16_t backtrace_sps[0x200];
    struct {
        uint16_t bank;
        uint16_t addr;
    } backtrace_returns[0x200];

    /* Watchpoints */
    uint16_t n_watchpoints;
    struct GB_watchpoint_s *watchpoints;

    /* Symbol tables */
    GB_symbol_map_t *bank_symbols[0x200];
    GB_reversed_symbol_map_t reversed_symbol_map;

    /* Misc */
    bool turbo;
    bool turbo_dont_skip;
    bool disable_rendering;
    uint32_t ram_size; // Different between CGB and DMG
    uint8_t boot_rom[0x900];
    bool vblank_just_occured; // For slow operations involving syscalls; these should only run once per vblank

} GB_gameboy_t;

#ifndef __printflike
/* Missing from Linux headers. */
#define __printflike(fmtarg, firstvararg) \
__attribute__((__format__ (__printf__, fmtarg, firstvararg)))
#endif

void GB_init(GB_gameboy_t *gb);
void GB_init_cgb(GB_gameboy_t *gb);
void GB_free(GB_gameboy_t *gb);
int GB_load_boot_rom(GB_gameboy_t *gb, const char *path);
int GB_load_rom(GB_gameboy_t *gb, const char *path);
int GB_save_battery(GB_gameboy_t *gb, const char *path);
void GB_load_battery(GB_gameboy_t *gb, const char *path);
int GB_save_state(GB_gameboy_t *gb, const char *path);
int GB_load_state(GB_gameboy_t *gb, const char *path);
void GB_run(GB_gameboy_t *gb);
void GB_set_pixels_output(GB_gameboy_t *gb, uint32_t *output);
void GB_set_vblank_callback(GB_gameboy_t *gb, GB_vblank_callback_t callback);
void GB_set_log_callback(GB_gameboy_t *gb, GB_log_callback_t callback);
void GB_log(GB_gameboy_t *gb, const char *fmt, ...) __printflike(2, 3);
void GB_attributed_log(GB_gameboy_t *gb, GB_log_attributes attributes, const char *fmt, ...) __printflike(3, 4);
void GB_set_input_callback(GB_gameboy_t *gb, GB_input_callback_t callback);
void GB_set_async_input_callback(GB_gameboy_t *gb, GB_input_callback_t callback);
void GB_set_sample_rate(GB_gameboy_t *gb, unsigned int sample_rate);
void GB_set_rgb_encode_callback(GB_gameboy_t *gb, GB_rgb_encode_callback_t callback);
void GB_set_infrared_callback(GB_gameboy_t *gb, GB_infrared_callback_t callback);
void GB_set_infrared_input(GB_gameboy_t *gb, bool state);
void GB_queue_infrared_input(GB_gameboy_t *gb, bool state, long cycles_after_previous_change);
#endif /* GB_h */
