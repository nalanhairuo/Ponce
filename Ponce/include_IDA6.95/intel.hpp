/*
 *      Interactive disassembler (IDA).
 *      Copyright (c) 1990-2015 Hex-Rays
 *      ALL RIGHTS RESERVED.
 *
 */

#ifndef _INTEL_HPP
#define _INTEL_HPP
#include <typeinf.hpp>
#include <allins.hpp>

#pragma pack(push, 4)

//---------------------------------
// not in ash.uflag for kernel (out struc/union descriptions)
inline bool tasm_ideal(void)  { return (ash.flag2 & AS2_IDEALDSCR) != 0; }

//---------------------------------
// Intel 80x86 cmd.auxpref bits
#define aux_lock        0x0001
#define aux_rep         0x0002
#define aux_repne       0x0004
#define aux_use32       0x0008  // segment type is 32-bits
#define aux_use64       0x0010  // segment type is 64-bits
#define aux_large       0x0020  // offset field is 32-bit (16-bit is not enough)
#define aux_short       0x0040  // short (byte) displacement used
#define aux_sgpref      0x0080  // a segment prefix byte is not used
#define aux_oppref      0x0100  // operand size prefix byte is not used
#define aux_adpref      0x0200  // address size prefix byte is not used
#define aux_basess      0x0400  // SS based instruction
#define aux_natop       0x0800  // operand size is not overridden by prefix
#define aux_natad       0x1000  // addressing mode is not overridden by prefix
#define aux_fpemu       0x2000  // FP emulator instruction
#define aux_vexpr       0x4000  // VEX-encoded instruction
#define aux_bnd         0x8000  // MPX-encoded instruction

//---------------------------------
// Intel 80x86 cmd.auxpref2 bits
#define aux2_evex       0x01    // EVEX-encoded instruction
#define aux2_xop        0x02    // XOP-encoded instruction
#define aux2_xacquire   0x04    // HLE prefix hints
#define aux2_xrelease   0x08    // HLE prefix hints

//---------------------------------
// operand types and other customization:
#define o_trreg         o_idpspec0      // IDP specific type
#define o_dbreg         o_idpspec1      // IDP specific type
#define o_crreg         o_idpspec2      // IDP specific type
#define o_fpreg         o_idpspec3      // IDP specific type
#define o_mmxreg        o_idpspec4      // IDP specific type
#define o_xmmreg        o_idpspec5      // xmm register
#define o_ymmreg        o_idpspec5+1    // ymm register
#define o_zmmreg        o_idpspec5+2    // zmm register
#define o_kreg          o_idpspec5+3    // opmask register

// 04.10.97: For o_mem,o_near,o_far we keep segment information as
// segrg - number of segment register to use
// if it is == SEGREG_IMM, then the segment was specified as an immediate
// value, look at segsel.

#define segrg           specval_shorts.high
#define SEGREG_IMM      0xFFFF          // this value of segrg means that
                                        // segment selector value is in
                                        // "segsel":
#define segsel          specval_shorts.low
#define hasSIB          specflag1
#define sib             specflag2
#define rex             insnpref        // REX byte for 64-bit mode, or bits from the VEX byte if vexpr()

// Op6 is used for opmask registers in EVEX.
// specflags from Op6 are used to extend cmd.
#define auxpref2        Op6.specflag1   // 8 bits for extended auxpref
#define evex_flags      Op6.specflag2   // bits from the EVEX byte if evexpr()

#define cr_suff         specflag1       // o_crreg: D suffix for cr registers (used for CR8D)

// bits in cmd.evex_flags:
const int EVEX_R = 0x01;           // High-16 register specifier modifier
const int EVEX_L = 0x02;           // Vector length/RC
const int EVEX_z = 0x04;           // Zeroing/Merging
const int EVEX_b = 0x08;           // Broadcast/RC/SAE Context
const int EVEX_V = 0x10;           // High-16 NDS/VIDX register specifier

// bits in cmd.rex:
const int REX_W = 8;               // 64-bit operand size
const int REX_R = 4;               // modrm reg field extension
const int REX_X = 2;               // sib index field extension
const int REX_B = 1;               // modrm r/m, sib base, or opcode reg fields extension
const int VEX_L = 0x80;            // 256-bit operation (YMM register)

typedef short regnum_t;

enum RegNo
{
  R_none = -1,
  R_ax = 0,
  R_cx = 1,
  R_dx = 2,
  R_bx = 3,
  R_sp = 4,
  R_bp = 5,
  R_si = 6,
  R_di = 7,
  R_r8 = 8,
  R_r9 = 9,
  R_r10 = 10,
  R_r11 = 11,
  R_r12 = 12,
  R_r13 = 13,
  R_r14 = 14,
  R_r15 = 15,

  R_al = 16,
  R_cl = 17,
  R_dl = 18,
  R_bl = 19,
  R_ah = 20,
  R_ch = 21,
  R_dh = 22,
  R_bh = 23,

  R_spl = 24,
  R_bpl = 25,
  R_sil = 26,
  R_dil = 27,

  R_ip = 28,

  R_es = 29,    // 0
  R_cs = 30,    // 1
  R_ss = 31,    // 2
  R_ds = 32,    // 3
  R_fs = 33,
  R_gs = 34,

  R_cf = 35,    // main cc's
  R_zf = 36,
  R_sf = 37,
  R_of = 38,

  R_pf = 39,    // additional cc's
  R_af = 40,
  R_tf = 41,
  R_if = 42,
  R_df = 43,

  R_efl = 44,   // eflags

  // the following registers will be used in the disassembly
  // starting from ida v5.7

  R_st0 = 45,   // floating point registers (not used in disassembly)
  R_st1 = 46,
  R_st2 = 47,
  R_st3 = 48,
  R_st4 = 49,
  R_st5 = 50,
  R_st6 = 51,
  R_st7 = 52,
  R_fpctrl = 53,// fpu control register
  R_fpstat = 54,// fpu status register
  R_fptags = 55,// fpu tags register

  R_mm0 = 56,   // mmx registers
  R_mm1 = 57,
  R_mm2 = 58,
  R_mm3 = 59,
  R_mm4 = 60,
  R_mm5 = 61,
  R_mm6 = 62,
  R_mm7 = 63,

  R_xmm0 = 64,  // xmm registers
  R_xmm1 = 65,
  R_xmm2 = 66,
  R_xmm3 = 67,
  R_xmm4 = 68,
  R_xmm5 = 69,
  R_xmm6 = 70,
  R_xmm7 = 71,
  R_xmm8 = 72,
  R_xmm9 = 73,
  R_xmm10 = 74,
  R_xmm11 = 75,
  R_xmm12 = 76,
  R_xmm13 = 77,
  R_xmm14 = 78,
  R_xmm15 = 79,
  R_mxcsr = 80,

  R_ymm0  = 81, // AVX 256-bit registers
  R_ymm1  = 82,
  R_ymm2  = 83,
  R_ymm3  = 84,
  R_ymm4  = 85,
  R_ymm5  = 86,
  R_ymm6  = 87,
  R_ymm7  = 88,
  R_ymm8  = 89,
  R_ymm9  = 90,
  R_ymm10 = 91,
  R_ymm11 = 92,
  R_ymm12 = 93,
  R_ymm13 = 94,
  R_ymm14 = 95,
  R_ymm15 = 96,

  R_bnd0 = 97, // MPX registers
  R_bnd1 = 98,
  R_bnd2 = 99,
  R_bnd3 = 100,

  R_xmm16 = 101, // AVX-512 extended XMM registers
  R_xmm17 = 102,
  R_xmm18 = 103,
  R_xmm19 = 104,
  R_xmm20 = 105,
  R_xmm21 = 106,
  R_xmm22 = 107,
  R_xmm23 = 108,
  R_xmm24 = 109,
  R_xmm25 = 110,
  R_xmm26 = 111,
  R_xmm27 = 112,
  R_xmm28 = 113,
  R_xmm29 = 114,
  R_xmm30 = 115,
  R_xmm31 = 116,

  R_ymm16 = 117, // AVX-512 extended YMM registers
  R_ymm17 = 118,
  R_ymm18 = 119,
  R_ymm19 = 120,
  R_ymm20 = 121,
  R_ymm21 = 122,
  R_ymm22 = 123,
  R_ymm23 = 124,
  R_ymm24 = 125,
  R_ymm25 = 126,
  R_ymm26 = 127,
  R_ymm27 = 128,
  R_ymm28 = 129,
  R_ymm29 = 130,
  R_ymm30 = 131,
  R_ymm31 = 132,

  R_zmm0 = 133, // AVX-512 ZMM registers
  R_zmm1 = 134,
  R_zmm2 = 135,
  R_zmm3 = 136,
  R_zmm4 = 137,
  R_zmm5 = 138,
  R_zmm6 = 139,
  R_zmm7 = 140,
  R_zmm8 = 141,
  R_zmm9 = 142,
  R_zmm10 = 143,
  R_zmm11 = 144,
  R_zmm12 = 145,
  R_zmm13 = 146,
  R_zmm14 = 147,
  R_zmm15 = 148,
  R_zmm16 = 149,
  R_zmm17 = 150,
  R_zmm18 = 151,
  R_zmm19 = 152,
  R_zmm20 = 153,
  R_zmm21 = 154,
  R_zmm22 = 155,
  R_zmm23 = 156,
  R_zmm24 = 157,
  R_zmm25 = 158,
  R_zmm26 = 159,
  R_zmm27 = 160,
  R_zmm28 = 161,
  R_zmm29 = 162,
  R_zmm30 = 163,
  R_zmm31 = 164,

  R_k0 = 165, // AVX-512 opmask registers
  R_k1 = 166,
  R_k2 = 167,
  R_k3 = 168,
  R_k4 = 169,
  R_k5 = 160,
  R_k6 = 171,
  R_k7 = 172,

  R_last,
};

inline bool is_segreg(int r) { return r >= R_es && r <= R_gs; }
inline bool is_fpureg(int r) { return r >= R_st0 && r <= R_st7; }
inline bool is_mmxreg(int r) { return r >= R_mm0 && r <= R_mm7; }
inline bool is_xmmreg(int r) { return r >= R_xmm0 && r <= R_xmm15; }
inline bool is_ymmreg(int r) { return r >= R_ymm0 && r <= R_ymm15; }

int cvt_to_wholereg(int _reg, bool allow_high_byte_regs); // byte reg -> whole reg
int calc_dbg_reg_index(const char *name);

//-------------------------------------------------------------------------
// is conditional branch?
inline bool insn_jcc(void)
{
  switch ( cmd.itype )
  {
    case NN_ja:
    case NN_jae:
    case NN_jb:
    case NN_jbe:
    case NN_jc:
    case NN_je:
    case NN_jg:
    case NN_jge:
    case NN_jl:
    case NN_jle:
    case NN_jna:
    case NN_jnae:
    case NN_jnb:
    case NN_jnbe:
    case NN_jnc:
    case NN_jne:
    case NN_jng:
    case NN_jnge:
    case NN_jnl:
    case NN_jnle:
    case NN_jno:
    case NN_jnp:
    case NN_jns:
    case NN_jnz:
    case NN_jo:
    case NN_jp:
    case NN_jpe:
    case NN_jpo:
    case NN_js:
    case NN_jz:
      return true;
  }
  return false;
}

//-------------------------------------------------------------------------
inline bool insn_default_opsize_64(void)
{
  if ( insn_jcc() )
    return true;
  switch ( cmd.itype )
  {
    // use ss
    case NN_pop:
    case NN_popf:
    case NN_popfq:
    case NN_push:
    case NN_pushf:
    case NN_pushfq:
    case NN_retn:
    case NN_retf:
    case NN_retnq:
    case NN_retfq:
    case NN_call:
    case NN_callfi:
    case NN_callni:
    case NN_enter:
    case NN_enterq:
    case NN_leave:
    case NN_leaveq:

    // near branches
    case NN_jcxz:
    case NN_jecxz:
    case NN_jrcxz:
    case NN_jmp:
    case NN_jmpni:
    case NN_jmpshort:
    case NN_loop:
    case NN_loopq:
    case NN_loope:
    case NN_loopqe:
    case NN_loopne:
    case NN_loopqne:
      return true;
  }
  return false;
}

inline bool mode16(void)  { return (cmd.auxpref & (aux_use32|aux_use64)) == 0; } // 16-bit mode?
inline bool mode32(void)  { return (cmd.auxpref & aux_use32) != 0; } // 32-bit mode?
inline bool mode64(void)  { return (cmd.auxpref & aux_use64) != 0; } // 64-bit mode?
inline bool natad(void)   { return (cmd.auxpref & aux_natad) != 0; } // natural address size (no prefixes)?
inline bool natop(void)   { return (cmd.auxpref & aux_natop) != 0; } // natural operand size (no prefixes)?
inline bool vexpr(void)   { return (cmd.auxpref & aux_vexpr) != 0; } // VEX encoding used
inline bool evexpr(void)  { return (cmd.auxpref2 & aux2_evex) != 0; } // EVEX encoding used
inline bool xopexpr(void) { return (cmd.auxpref2 & aux2_xop) != 0; } // XOP encoding used

inline bool ad16(void)          // is current addressing 16-bit?
{
  int p = cmd.auxpref & (aux_use32|aux_use64|aux_natad);
  return p == aux_natad || p == aux_use32;
}

inline bool ad32(void)          // is current addressing 32-bit?
{
  int p = cmd.auxpref & (aux_use32|aux_use64|aux_natad);
  return p == (aux_natad|aux_use32)
      || p == 0
      || p == aux_use64;
}

inline bool ad64(void)          // is current addressing 64-bit?
{
#ifdef __EA64__
  int p = cmd.auxpref & (aux_use32|aux_use64|aux_natad);
  return p == (aux_natad|aux_use64);
#else
  return false;
#endif
}

inline bool op16(void)          // is current operand size 16-bit?
{
  int p = cmd.auxpref & (aux_use32|aux_use64|aux_natop);
  return p == aux_natop                                 // 16-bit segment, no prefixes
      || p == aux_use32                                 // 32-bit segment, 66h
      || p == aux_use64 && (cmd.rex & REX_W) == 0;      // 64-bit segment, 66h, no rex.w
}

inline bool op32(void)          // is current operand size 32-bit?
{
  int p = cmd.auxpref & (aux_use32|aux_use64|aux_natop);
  return p == 0                                         // 16-bit segment, 66h
      || p == (aux_use32|aux_natop)                     // 32-bit segment, no prefixes
      || p == (aux_use64|aux_natop) && (cmd.rex & REX_W) == 0; // 64-bit segment, 66h, no rex.w
}

inline bool op64(void)          // is current operand size 64-bit?
{
#ifdef __EA64__
  return mode64()
      && ((cmd.rex & REX_W) != 0
       || natop() && insn_default_opsize_64()); // 64-bit segment, rex.w or insns-64
#else
  return false;
#endif
}

inline bool op256(void)        // is VEX.L == 1 or EVEX.L'L == 01?
{
  return (cmd.rex & VEX_L) != 0
      && (vexpr()
       || xopexpr()
       || evexpr() && (cmd.evex_flags & EVEX_L) == 0);
}

inline bool op512(void)        // is EVEX.L'L == 10?
{
  return evexpr() && (cmd.rex & VEX_L) == 0 && (cmd.evex_flags & EVEX_L) != 0;
}

inline bool is_vsib(void)  // does instruction use VSIB variant of the sib byte?
{
  switch ( cmd.itype )
  {
    case NN_vgatherdps:
    case NN_vgatherdpd:
    case NN_vgatherqps:
    case NN_vgatherqpd:
    case NN_vpgatherdd:
    case NN_vpgatherdq:
    case NN_vpgatherqd:
    case NN_vpgatherqq:

    case NN_vscatterdps:
    case NN_vscatterdpd:
    case NN_vscatterqps:
    case NN_vscatterqpd:
    case NN_vpscatterdd:
    case NN_vpscatterdq:
    case NN_vpscatterqd:
    case NN_vpscatterqq:

    case NN_vgatherpf0dps:
    case NN_vgatherpf0qps:
    case NN_vgatherpf0dpd:
    case NN_vgatherpf0qpd:
    case NN_vgatherpf1dps:
    case NN_vgatherpf1qps:
    case NN_vgatherpf1dpd:
    case NN_vgatherpf1qpd:

    case NN_vscatterpf0dps:
    case NN_vscatterpf0qps:
    case NN_vscatterpf0dpd:
    case NN_vscatterpf0qpd:
    case NN_vscatterpf1dps:
    case NN_vscatterpf1qps:
    case NN_vscatterpf1dpd:
    case NN_vscatterpf1qpd:
      return true;
  }
  return false;
}

inline regnum_t vsib_index_fixreg(regnum_t index)
{
  switch ( cmd.itype )
  {
    case NN_vscatterdps:
    case NN_vscatterqps:
    case NN_vscatterqpd:
    case NN_vpscatterdd:
    case NN_vpscatterqd:
    case NN_vpscatterqq:

    case NN_vpgatherdd:
    case NN_vpgatherqd:
    case NN_vpgatherqq:
    case NN_vgatherdps:
    case NN_vgatherqps:
    case NN_vgatherqpd:
      if ( index > 15 )
        index += op512() ? R_zmm0 : op256() ? (R_ymm16 - 16) : (R_xmm16 - 16);
      else
        index += op512() ? R_zmm0 : op256() ? R_ymm0 : R_xmm0;
      break;

    case NN_vscatterdpd:
    case NN_vpscatterdq:

    case NN_vgatherdpd:
    case NN_vpgatherdq:
      if ( index > 15 )
        index += op512() ? (R_ymm16 - 16) : (R_xmm16 - 16);
      else
        index += op512() ? R_ymm0 : R_xmm0;
      break;

    case NN_vgatherpf0dps:
    case NN_vgatherpf0qps:
    case NN_vgatherpf0qpd:
    case NN_vgatherpf1dps:
    case NN_vgatherpf1qps:
    case NN_vgatherpf1qpd:

    case NN_vscatterpf0dps:
    case NN_vscatterpf0qps:
    case NN_vscatterpf0qpd:
    case NN_vscatterpf1dps:
    case NN_vscatterpf1qps:
    case NN_vscatterpf1qpd:
      index += R_zmm0;
      break;

    case NN_vgatherpf0dpd:
    case NN_vgatherpf1dpd:
    case NN_vscatterpf0dpd:
    case NN_vscatterpf1dpd:
      if ( index > 15 )
        index += R_ymm16 - 16;
      else
        index += R_ymm0;
      break;
  }
  return index;
}

inline int sib_base(const op_t &x)                    // get extended sib base
{
  int base = x.sib & 7;
#ifdef __EA64__
  if ( cmd.rex & REX_B )
    base |= 8;
#endif
  return base;
}

inline regnum_t sib_index(const op_t &x)                   // get extended sib index
{
  regnum_t index = regnum_t((x.sib >> 3) & 7);
#ifdef __EA64__
  if ( (cmd.rex & REX_X) != 0 )
    index |= 8;
#endif
  if ( is_vsib() )
  {
    if ( (cmd.evex_flags & EVEX_V) != 0 )
      index |= 16;
    index = vsib_index_fixreg(index);
  }
  return index;
}

inline int sib_scale(const op_t &x)
{
  int scale = (x.sib >> 6) & 3;
  return scale;
}

// get the base register of the operand with a displacement
inline int x86_base(const op_t &x)
{
  return x.hasSIB ? sib_base(x) : x.phrase;
}

const int INDEX_NONE = 4;       // no index register is present
// get the index register of the operand with a displacement
inline int x86_index(const op_t &x)
{
  return x.hasSIB ? sib_index(x) : INDEX_NONE;
}

// get the scale factor of the operand with a displacement
inline int x86_scale(const op_t &x)
{
  return x.hasSIB ? sib_scale(x) : 0;
}

// does the operand have a displacement?
inline int has_displ(const op_t &x)
{
  return x.type == o_displ || x.type == o_mem && x.hasSIB;
}

// does the operand refer to stack? (sp or bp based)
bool is_stack_ref(const op_t &x, int breg);

// return addressing width in form of dt_... constant
inline char address_dtyp(void)
{
  return char(ad64() ? dt_qword : ad32() ? dt_dword : dt_word);
}

// return operand width in form of dt_... constant
inline char operand_dtyp(void)
{
  return char(op64() ? dt_qword : op32() ? dt_dword : op16() ? dt_word : dt_byte);
}

inline bool is_io_insn(void)
{
  return cmd.itype == NN_ins
      || cmd.itype == NN_outs
      || cmd.itype == NN_out
      || cmd.itype == NN_in;
}

//---------------------------------
extern netnode intel_node;              // name: "$ vmm functions"
                                        // supvals -- for vmm functions
                                        // altvals -- for int ah values
                                        //           also for register values
                                        //           of indirect register calls
                                        // supvals('f') - frame info
                                        // charval('p') - postponed npurged info
                                        // blob('s') - push info
                                        // supval('b') - fbase register num and value
                                        // altval('r') - if 'ret' is not a function return
                                        // address of the jump target. This info is
                                        // useful for the 'push offset; ret' pairs.
                                        // If the jump target is unknown, use BADADDR.
                                        // charval('P') - is callee a ptr?
                                        // blob('F')    - finally_block_t
                                        // altval('h')  - function owning SEH handler at current ea

const char fbase_tag    = 'b';
const char frame_tag    = 'f';
const char purge_tag    = 'p';
const char ret_tag      = 'r';
const char pushinfo_tag = 's';
const char is_ptr_tag   = 'P';
const char finally_tag  = 'F';
const char handler_tag  = 'h';

struct fbase_reg_t
{
  ea_t value;
  ea_t minea;
  int16 reg;
  int16 unused;
};

// fbase reg is a register used to access data for the current function
// it is usually initialized by __i686_get_pc_thunk() function
bool get_fbase_reg(ea_t ea, fbase_reg_t *fbr=NULL);
void set_fbase_reg(ea_t ea, ea_t minea, int reg, ea_t value);

// Some 'ret' insns do not return from the function but are used for short jumps
// (for example: push off; ret). The following functions mark such 'ret' instructions.
inline bool get_ret_target(ea_t ea, ea_t *target)
{
  if ( intel_node.supval(ea, target, sizeof(ea_t), ret_tag) != sizeof(ea_t) )
    return false;
  if ( target != NULL )
    *target += ea;
  return true;
}

inline void set_ret_target(ea_t ea, ea_t target)
{
  intel_node.altset(ea, target-ea, ret_tag);
}

inline void del_ret_target(ea_t ea)
{
  intel_node.altdel(ea, ret_tag);
}

extern uint32 idpflags;                 // name: "$ idpflags"

#define AFIDP_PUSH      0x0001          // push seg; push num; is converted to offset
#define AFIDP_NOP       0x0002          // db 90h after jmp is converted to nop

#define AFIDP_MOVOFF    0x0004          // mov     reg, numoff  <- convert to offset
                                        // mov     segreg, immseg

#define AFIDP_MOVOFF2   0x0008          // mov     z, numoff    <- convert to offset
                                        // mov     z, immseg
                                        // where z - o_mem, o_displ
#define AFIDP_ZEROINS   0x0010          // allow zero opcode instructions:
                                        //      add [bx+si], al  (16bit)
                                        //      add [eax], al    (32bit)
                                        //      add [rax], al    (64bit)

#define AFIDP_BRTTI     0x0020          // Advanced analysis of Borlands RTTI
#define AFIDP_UNKRTTI   0x0040          // -"- with 'unknown_libname'
#define AFIDP_EXPFUNC   0x0080          // for PE? bc(ms?) - expanding
                                        // function (exception subblock)
#define AFIDP_DIFBASE   0x0100          // Allow references with different segment bases
#define AFIDP_NOPREF    0x0200          // Don't display superfluous prefixes
#define AFIDP_NOVXD     0x0400          // Don't interpret int 20 as VxDcall
#define AFIDP_NOFPEMU   0x0800          // Disable FPU emulation instructions
#define AFIDP_SHOWRIP   0x1000          // Explicit RIP-addressing
#define AFIDP_NOSEH     0x2000          // Disable SEH/EH analysis
#define AFIDP_INT3STOP  0x4000          // int 3 may stop code flow
                                        //      call <func>
                                        //      int 3 <- this is likely a no-return guard

inline bool should_af_push(void)     { return (idpflags & AFIDP_PUSH) != 0; }
inline bool should_af_nop(void)      { return (idpflags & AFIDP_NOP) != 0; }
inline bool should_af_movoff(void)   { return (idpflags & AFIDP_MOVOFF) != 0; }
inline bool should_af_movoff2(void)  { return (idpflags & AFIDP_MOVOFF2) != 0; }
inline bool should_af_zeroins(void)  { return (idpflags & AFIDP_ZEROINS) != 0; }
inline bool should_af_brtti(void)    { return (idpflags & AFIDP_BRTTI) != 0; }
inline bool should_af_urtti(void)    { return (idpflags & AFIDP_UNKRTTI) != 0; }
inline bool should_af_fexp(void)     { return (idpflags & AFIDP_EXPFUNC) != 0; }
inline bool should_af_difbase(void)  { return (idpflags & AFIDP_DIFBASE) != 0; }
inline bool should_af_nopref(void)   { return (idpflags & AFIDP_NOPREF) != 0; }
inline bool should_af_vxd(void)      { return (idpflags & AFIDP_NOVXD) == 0; }
inline bool should_af_fpemu(void)    { return (idpflags & AFIDP_NOFPEMU) == 0; }
inline bool should_af_showrip(void)  { return (idpflags & AFIDP_SHOWRIP) != 0; }
inline bool should_af_seh(void)      { return (idpflags & AFIDP_NOSEH) == 0; }
inline bool should_af_int3stop(void) { return (idpflags & AFIDP_INT3STOP) != 0; }

// the second operand of lea instruction should not be treated as memory reference
// unless there is cs: prefix or the user has specified 'offset' flag
// in other cases lea is used for arbirary calculations
inline bool is_arith_lea(const op_t &x)
{
  return cmd.itype == NN_lea && x.segrg != R_cs && !isOff(uFlag, x.n);
}

// the following operand types are ignored for imul's third operand
inline bool is_forbidden_imul_flag(const op_t &x, flags_t F)
{
  if ( x.n == 2 && cmd.Op2.shown() )
    return isOff1(F) || isStroff1(F) || isStkvar1(F);
  return false;
}

inline bool is_push_ecx(uchar b)
{
  return b == 0x51; // push ecx
}

inline bool is_push_eax(uchar b)
{
  return b == 0x50; // push eax
}

inline bool is_push_edx(uchar b)
{
  return b == 0x52; // push edx
}

inline bool is_push_ebx(uchar b)
{
  return b == 0x53; // push ebx
}

inline bool is_volatile_reg(int r)
{
  return r != R_bx
      && r != R_bp
      && r != R_si
      && r != R_di
      && r != R_r12
      && r != R_r13
      && r != R_r14
      && r != R_r15;
}

//------------------------------------------------------------------
struct pushreg_t
{
  ea_t     ea;    // instruction ea
  sval_t   off;   // offset from the frame top (sp delta)
  sval_t   width; // register width (or number of allocated bytes)
  regnum_t reg;   // register number (R_none means stack space allocation)
  uint16   flags; // additional flags
#define PRF_NONE   0x0000 // Entry describes a push or an allocation
#define PRF_MOVE   0x0001 // Entrz describes a register save by a move instruction
#define PRF_SPILL  0x0002 // Indicates that entry is located before local stack region
#define PRF_MASK (PRF_MOVE | PRF_SPILL)
};

struct pushinfo_t
{
  enum { PUSHINFO_VERSION = 4 };
  int flags;
#define PINF_SEHCALL    0x0001  // call to SEH_prolog is present
#define PINF_SEHMAN     0x0002  // Manual SEH setup
#define PINF_COOKIE     0x0004  // Has security cookie
#define PINF_ALIGNED    0x0008  // Lvars are align stred (visual studio)
#define PINF_VARARG     0x0010  // Vararg prolog (currently used for gcc64)
#define PINF_BPOFF      0x0020  // xmm_stkoff/reg_stkoff are from rbp (otherwise from rsp)
#define PINF_HAVE_SSIZE 0x0040  // pushinfo_t structure contains its own size (field 'cb')
#define PINF_PSI_FLAGS  0x0080  // pushreg_t structure contains flags field
  qvector<pushreg_t> psi;       // stack allocation instructions
  ssize_t bpidx;                // index into psi
  uint32 spoiled;               // bitmask of spoiled registers at the end of prolog

  eavec_t prolog_insns;         // additional prolog instruction addresses
                                // (in addition to instructions from psi)

  typedef qvector<eavec_t> pop_info_t;
  pop_info_t pops;              // pop insns for pushs (indexes shifted by one)
                                // in other words, this is epilog instructions
                                // index 0: epilog insns not linked to a push insn
                                // 1..psi.size(): epilog insns for each push insn
                                // usually there will be only one pop for each push.
                                // but there might be several pops for each push.
                                // (because the function has several returns)

  int eh_type;                  // function has exception handling
                                // low 16 bits: type, high 16 bits: version
#define EH_NONE    0            // no EH found
#define EH_VCSEH   1            // SEH (__except_handlerN, __SEH_prologN)
#define EH_VCCPPEH 2            // MSVC C++ EH (_EH_prolog[N])
  int seh_ver()
  {
    if ( (eh_type & 0xFFFF) == EH_VCSEH )
      return (eh_type >> 16) & 0xFFFF;
    return 0;
  }
  int eh_ver()
  {
    if ( (eh_type & 0xFFFF) == EH_VCCPPEH )
      return (eh_type >> 16) & 0xFFFF;
    return 0;
  }

  ea_t eh_info;                 // for SEH: scopetable address, for C++ EH: __ehhandler address

  // for gcc64 vararg (see PINF_VARARG):
  sval_t xmm_stkoff;            // offset from ebp to xmm savearea
  sval_t reg_stkoff;            // offset from ebp to gpreg savearea
                                // these 2 offsets are either from rsp or rbp
                                // see PINF_BPOFF for that
  int xmm_nsaved;               // number of saved xmm regs
  int reg_nsaved;               // number of saved general purpose regs

  int cb;                       // size of this structure

  pushinfo_t(void)
    : flags(PINF_HAVE_SSIZE | PINF_PSI_FLAGS), bpidx(-1), spoiled(0),
      eh_type(EH_NONE), eh_info(BADADDR),
      xmm_stkoff(0), reg_stkoff(0), xmm_nsaved(0), reg_nsaved(0),
      cb(sizeof(pushinfo_t))
  {
  }
  int verify(func_t *pfn);
  void save_to_idb(ea_t ea);
  bool restore_from_idb(ea_t ea);
};

enum spec_func_type_t
{
  SF_NONE,
  SF_EH_PROLOG,
  SF_SEH_PROLOG,
  SF_SEH_EPILOG,
  SF_ALLOCA,
  SF_CHK,
  SF_SYSINIT,
  SF_EH_EPILOG,
  SF_LSTRCATN,
};

spec_func_type_t get_spec_func_type(ea_t callee, sval_t *p_delta);

inline int pc_shadow_area_size()
{
  return (inf.is_64bit() && default_compiler() == COMP_MS) ? 4 * 8 : 0;
}

struct regval_t;
typedef const regval_t &idaapi getreg_t(const char *name, const regval_t *regvalues);

// Structure where information about a mmx/xmm/ymm type is returned
struct mmtype_t
{
  const char *name;
  const type_t *type;
  const type_t *fields;
  tinfo_t tif;
};

//----------------------------------------------------------------------
// The following events are supported by the PC module in the ph.notify() function
namespace pc_module_t
{
  enum event_codes_t
  {
    set_difbase = processor_t::loader,
                        // set AFIDP_DIFBASE flag
                        // in: int onoff
                        // Returns: nothing
    restore_pushinfo,   // Restore function prolog info from the database
                        // in: ea_t func_start
                        //     pushinfo_t *pi
                        // Returns: 2-ok, otherwise-failed
    save_pushinfo,      // Save function prolog info to the database
                        // in: ea_t func_start
                        //     pushinfo_t *pi
                        // Returns: 2-ok, otherwise-failed
    prolog_analyzed,    // This event is generated by the PC module
                        // at the end of prolog analysis. Plugins may
                        // hook to it and improve the analysis.
                        // in: ea_t first_past_prolog_insn
                        //     pushinfo_t *pi
                        // Returns: 1-ok, 2-ok but do not automatically verify epilog
    verify_epilog,      // Verify function epilog
                        // in: pushinfo_t *pi
                        //     int *answer
                        // cmd structure must be filled with the first epilog insn
                        // number of verified epilog instructions will be in the 'answer'
                        // returns: 2-ok, otherwise-failed
    find_reg_value,     // Find a register value by looking backwards from cmd.ea
                        // in: int regnum
                        //     sval_t *value
                        // returns: 2-found, 0-not found, otherwise not implemented
                        // This function spoils cmd
    dbgtools_path,      // Returns the configuration value of the debugging tools path (from IDA.CFG)
                        // in: char *path
                        //     size_t path_size
                        // returns: 1-if value is set, 0-if value not set in IDA.CFG
                        // This function spoils cmd
    get_mmtype,         // Retrieve mmx/xmm/ymm related type
                        // in: const char *name
                        //     const mmtype_t **ptr
                        // Returns: number of found types+1
                        //          -1-type exists but is wrong
                        // If name==NULL, initialize all mmx/xmm/ymm types
    is_get_pc_thunk,    // Detect get_pc_thunk calls
                        // in: const insn_t *ins
                        //     RegNo *p_reg,
                        //     ea_t *p_end
                        // returns: 2-found, -1-not found, 1-not implemented
  };
}

//-------------------------------------------------------------------------
// Don't use the following define's with underscores at the start!
#define _PT_486p        0x00000001
#define _PT_486r        0x00000002
#define _PT_386p        0x00000004
#define _PT_386r        0x00000008
#define _PT_286p        0x00000010
#define _PT_286r        0x00000020
#define _PT_086         0x00000040
#define _PT_586p        0x00000080      // Pentium real mode
#define _PT_586r        0x00000100      // Pentium protected mode
#define _PT_686r        0x00000200      // Pentium Pro real
#define _PT_686p        0x00000400      // Pentium Pro protected
#define _PT_mmx         0x00000800      // MMX extensions
#define _PT_pii         0x00001000      // Pentium II
#define _PT_3d          0x00002000      // 3DNow! extensions
#define _PT_piii        0x00004000      // Pentium III
#define _PT_k7          0x00008000      // AMD K7
#define _PT_p4          0x00010000      // Pentium 4
#define _PT_sse3        0x00020000      // SSE3 + SSSE3
#define _PT_sse4        0x00040000      // SSE4.1 + SSE4.2

//
//   The following values mean 'is XXX processor or better?'
//

#define PT_sse4          _PT_sse4
#define PT_sse3         (_PT_sse3 | _PT_sse4 )
#define PT_p4           ( PT_sse3 | _PT_p4   )
#define PT_k7           ( PT_p4   | _PT_k7   )
#define PT_piii         ( PT_k7   | _PT_piii )
#define PT_k62          ( PT_piii | _PT_3d   )
#define PT_3d            _PT_3d
#define PT_pii          ( PT_piii | _PT_pii  )
#define PT_mmx          (_PT_mmx  | _PT_3d   )
#define PT_686p         ( PT_pii  | _PT_686p )
#define PT_686r         ( PT_686p | _PT_686r )
#define PT_586p         ( PT_686r | _PT_586p )
#define PT_586r         ( PT_586p | _PT_586r )
#define PT_486p         ( PT_586r | _PT_486p )
#define PT_486r         ( PT_486p | _PT_486r )
#define PT_386p         ( PT_486r | _PT_386p )
#define PT_386r         ( PT_386p | _PT_386r )
#define PT_286p         ( PT_386r | _PT_286p )
#define PT_286r         ( PT_286p | _PT_286r )
#define PT_086          ( PT_286r | _PT_086  )

//
//   The following values mean 'is exactly XXX processor?'
//

#define PT_ismmx        (_PT_mmx            )
#define PT_is686        (_PT_686r | _PT_686p)
#define PT_is586        (_PT_586r | _PT_586p)
#define PT_is486        (_PT_486r | _PT_486p)
#define PT_is386        (_PT_386r | _PT_386p)
#define PT_is286        (_PT_286r | _PT_286p)
#define PT_is086        (_PT_086)

//---------------------------------------------------------------------
inline bool isProtected(uint32 type)
{
  return (type
        & (_PT_286p
         | _PT_386p
         | _PT_486p
         | _PT_586p
         | _PT_686p
         | _PT_pii)) != 0;
}

inline bool isAMD(uint32 type)   { return (type & PT_k7  ) != 0; }
inline bool isp4 (uint32 type)   { return (type & PT_p4  ) != 0; }
inline bool isp3 (uint32 type)   { return (type & PT_piii) != 0; }
inline bool is3dnow(uint32 type) { return (type & PT_3d  ) != 0; }
inline bool ismmx(uint32 type)   { return (type & PT_mmx ) != 0; }
inline bool isp2 (uint32 type)   { return (type & PT_pii ) != 0; }
inline bool is686(uint32 type)   { return (type & PT_686r) != 0; }
inline bool is586(uint32 type)   { return (type & PT_586r) != 0; }
inline bool is486(uint32 type)   { return (type & PT_486r) != 0; }
inline bool is386(uint32 type)   { return (type & PT_386r) != 0; } // is 386 or better ?
inline bool is286(uint32 type)   { return (type & PT_286r) != 0; } // is 286 or better ?

#pragma pack(pop)
#endif // _INTEL_HPP
