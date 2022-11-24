#ifndef mima_h
#define mima_h

#include <stdint.h>
#include <stdarg.h>
#include "log.h"

typedef uint32_t    mima_register;
typedef uint32_t    mima_word;

typedef uint8_t     mima_flag;
typedef uint8_t     mima_bool;

#define mima_words  768000000/4

#define mima_true   1
#define mima_false  0

#define mima_char_input     0xc000001
#define mima_integer_input  0xc000002
#define mima_char_output    0xc000003
#define mima_integer_output 0xc000004

typedef enum _mima_instruction_type
{
    ADD = 0, AND, OR, XOR, LDV, LDIV, STV, LDC, JMP, JMN, EQL, HLT = 0xF0, NOT, RAR, RRN
} mima_instruction_type;

typedef struct _mima_instruction
{
    mima_instruction_type   op_code;
    uint32_t                value;
    mima_bool               extended;
} mima_instruction;

typedef struct _mima_control_unit
{
    mima_register   IR;
    mima_register   IAR;
    mima_flag       TRA;
    mima_flag       RUN;
} mima_control_unit;

typedef struct _mima_memory_unit
{
    mima_register    SIR;
    mima_register    SAR;
    mima_word        *memory;
} mima_memory_unit;

typedef struct _mima_processing_unit
{
    const mima_register     ONE;
    mima_register           ACC;
    mima_register           X;
    mima_register           Y;
    mima_register           Z;
    mima_instruction_type   ALU;
    uint8_t                 MICRO_CYCLE;
    //      ALU;
} mima_processing_unit;

// forward declaration
struct _mima_label;
struct _mima_breakpoint;
struct _mima_t;
typedef void (*mima_io_callback_fun)(struct _mima_t *mima, mima_register *);

typedef struct _mima_io_callback
{
    mima_io_callback_fun    func;
    mima_register           address;
} mima_io_callback;

typedef struct _mima_t
{
    mima_control_unit       control_unit;
    mima_memory_unit        memory_unit;
    mima_processing_unit    processing_unit;

    mima_instruction        current_instruction;

    mima_io_callback        *stv_callbacks;
    uint32_t                stv_callbacks_count;
    uint32_t                stv_callbacks_capacity;

    mima_io_callback        *ldv_callbacks;
    uint32_t                ldv_callbacks_count;
    uint32_t                ldv_callbacks_capacity;

    struct _mima_label      *mima_labels;
    struct _mima_breakpoint *mima_breakpoints;

    const char              *source_file;
} mima_t;


mima_t mima_init();
void mima_delete(mima_t *mima);

mima_bool mima_compile_f(mima_t *mima, const char *file_name);
mima_bool mima_compile_s(mima_t *mima, char *source_code);

void mima_run(mima_t *mima, mima_bool interactive);
void mima_run_micro_instruction_steps(mima_t *mima, char *micro_steps);
void mima_run_micro_instruction_step(mima_t *mima);
void mima_run_instruction_steps(mima_t *mima, char *steps);
void mima_run_instruction_step(mima_t *mima);

void mima_set_run(mima_t*, mima_bool run);

void mima_micro_instruction_step(mima_t *mima);

mima_instruction mima_instruction_decode(mima_t *mima);
mima_instruction mima_instruction_decode_mem(mima_word mem);

mima_bool mima_sar_external(mima_t *mima);

// ADD, AND, OR, XOR, EQL
void mima_instruction_common(mima_t *mima);
void mima_instruction_LDV(mima_t *mima);
void mima_instruction_STV(mima_t *mima);
void mima_instruction_HLT(mima_t *mima);
void mima_instruction_LDC(mima_t *mima);
void mima_instruction_JMP(mima_t *mima);
void mima_instruction_JMN(mima_t *mima);
void mima_instruction_NOT(mima_t *mima);
void mima_instruction_RAR(mima_t *mima);
void mima_instruction_RRN(mima_t *mima);

const char *mima_get_instruction_name(mima_instruction_type instruction);

void mima_print_state(mima_t *mima);
void mima_print_memory_at(mima_t *mima, mima_register address, uint32_t count);
void mima_dump_memory_as_text_at(mima_t* mima, mima_register address, char* text);
void mima_print_memory_unit_state(mima_t *mima);
void mima_print_control_unit_state(mima_t *mima);
void mima_print_processing_unit_state(mima_t *mima);

mima_bool mima_register_IO_LDV_callback(mima_t *mima, uint32_t address, mima_io_callback_fun fun_ptr);
mima_bool mima_register_IO_STV_callback(mima_t *mima, uint32_t address, mima_io_callback_fun fun_ptr);

mima_bool mima_hit_active_breakpoint(mima_t *mima);

#endif // mima_h
