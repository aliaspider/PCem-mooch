extern int cpu, cpu_manufacturer;

/*808x class CPUs*/
#define CPU_8088 0
#define CPU_8086 1

/*286 class CPUs*/
#define CPU_286 2

/*386 class CPUs*/
#define CPU_386SX  3
#define CPU_386DX  4
#define CPU_486SLC 5
#define CPU_486DLC 6

/*486 class CPUs*/
#define CPU_i486SX  7
#define CPU_Am486SX 8
#define CPU_Cx486S  9
#define CPU_i486DX  10
#define CPU_Am486DX 11
#define CPU_Cx486DX 12
#define CPU_Cx5x86  13

/*586 class CPUs*/
#define CPU_WINCHIP 14
#define CPU_PENTIUM 15
#define CPU_PENTIUMMMX 16
#define CPU_K6 17
#define CPU_K6_2 18
#define CPU_K6_3 19



#define MANU_INTEL 0
#define MANU_AMD   1
#define MANU_CYRIX 2
#define MANU_IDT   3

extern int timing_rr;
extern int timing_mr, timing_mrl;
extern int timing_rm, timing_rml;
extern int timing_mm, timing_mml;
extern int timing_bt, timing_bnt;

extern int timing_int, timing_int_rm, timing_int_v86, timing_int_pm, timing_int_pm_outer;
extern int timing_iret_rm, timing_iret_v86, timing_iret_pm, timing_iret_pm_outer;
extern int timing_call_rm, timing_call_pm, timing_call_pm_gate, timing_call_pm_gate_inner;
extern int timing_retf_rm, timing_retf_pm, timing_retf_pm_outer;
extern int timing_jmp_rm, timing_jmp_pm, timing_jmp_pm_gate;


typedef struct
{
        char name[32];
        int cpu_type;
        int speed;
        int rspeed;
        int multi;
        int pci_speed;
        uint32_t edx_reset;
        uint32_t cpuid_model;
        uint16_t cyrix_id;
        int cpu_flags;
} CPU;

extern CPU cpus_8088[];
extern CPU cpus_8086[];
extern CPU cpus_286[];
extern CPU cpus_i386[];
extern CPU cpus_Am386[];
extern CPU cpus_486SDLC[];
extern CPU cpus_i486[];
extern CPU cpus_Am486[];
extern CPU cpus_Cx486[];
extern CPU cpus_WinChip[];
extern CPU cpus_Pentium5V[];
extern CPU cpus_PentiumS5[];
extern CPU cpus_Pentium[];
extern CPU cpus_K6[];

extern CPU cpus_pcjr[];
extern CPU cpus_pc1512[];
extern CPU cpus_ibmat[];
extern CPU cpus_acer[];

extern int cpu_iscyrix;
extern int cpu_16bitbus;
extern int cpu_busspeed;
extern int cpu_multi;

extern int cpu_hasrdtsc;
extern int cpu_hasMSR;
extern int cpu_hasMMX;
extern int cpu_hasCR4;

#define CR4_TSD  (1 << 2)
#define CR4_DE   (1 << 3)
#define CR4_MCE  (1 << 6)
#define CR4_PCE  (1 << 8)

extern uint64_t cpu_CR4_mask;

#define CPU_SUPPORTS_DYNAREC 1
#define CPU_REQUIRES_DYNAREC 2

extern uint64_t tsc;

void cyrix_write(uint16_t addr, uint8_t val, void *priv);
uint8_t cyrix_read(uint16_t addr, void *priv);

extern int is8086;

void cpu_RDMSR();
void cpu_WRMSR();

extern int cpu_use_dynarec;

extern int xt_cpu_multi;
