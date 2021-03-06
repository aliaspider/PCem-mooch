#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define printf pclog

/*Memory*/
uint8_t *ram,*vram;

uint32_t rammask;

int readlookup[256],readlookupp[256];
uintptr_t *readlookup2;
int readlnext;
int writelookup[256],writelookupp[256];
uintptr_t *writelookup2;
int writelnext;

extern int mmu_perm;

#define readmemb(a) ((readlookup2[(a)>>12]==-1)?readmembl(a):*(uint8_t *)(readlookup2[(a) >> 12] + (a)))
#define readmemw(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF || (((s)+(a))&0xFFF)>0xFFE)?readmemwl(s,a):*(uint16_t *)(readlookup2[(uint32_t)((s)+(a))>>12]+(uint32_t)((s)+(a))))
#define readmeml(s,a) ((readlookup2[(uint32_t)((s)+(a))>>12]==-1 || (s)==0xFFFFFFFF || (((s)+(a))&0xFFF)>0xFFC)?readmemll(s,a):*(uint32_t *)(readlookup2[(uint32_t)((s)+(a))>>12]+(uint32_t)((s)+(a))))

//#define writememb(a,v) if (writelookup2[(a)>>12]==0xFFFFFFFF) writemembl(a,v); else ram[writelookup2[(a)>>12]+((a)&0xFFF)]=v
//#define writememw(s,a,v) if (writelookup2[((s)+(a))>>12]==0xFFFFFFFF || (s)==0xFFFFFFFF) writememwl(s,a,v); else *((uint16_t *)(&ram[writelookup2[((s)+(a))>>12]+(((s)+(a))&0xFFF)]))=v
//#define writememl(s,a,v) if (writelookup2[((s)+(a))>>12]==0xFFFFFFFF || (s)==0xFFFFFFFF) writememll(s,a,v); else *((uint32_t *)(&ram[writelookup2[((s)+(a))>>12]+(((s)+(a))&0xFFF)]))=v
//#define readmemb(a) ((isram[((a)>>16)&255] && !(cr0>>31))?ram[a&0xFFFFFF]:readmembl(a))
//#define writememb(a,v) if (isram[((a)>>16)&255] && !(cr0>>31)) ram[a&0xFFFFFF]=v; else writemembl(a,v)

//void writememb(uint32_t addr, uint8_t val);
uint8_t readmembl(uint32_t addr);
void writemembl(uint32_t addr, uint8_t val);
uint8_t readmemb386l(uint32_t seg, uint32_t addr);
void writememb386l(uint32_t seg, uint32_t addr, uint8_t val);
uint16_t readmemwl(uint32_t seg, uint32_t addr);
void writememwl(uint32_t seg, uint32_t addr, uint16_t val);
uint32_t readmemll(uint32_t seg, uint32_t addr);
void writememll(uint32_t seg, uint32_t addr, uint32_t val);
uint64_t readmemql(uint32_t seg, uint32_t addr);
void writememql(uint32_t seg, uint32_t addr, uint64_t val);

uint8_t *getpccache(uint32_t a);

uint32_t mmutranslatereal(uint32_t addr, int rw);

void addreadlookup(uint32_t virt, uint32_t phys);
void addwritelookup(uint32_t virt, uint32_t phys);


/*IO*/
uint8_t  inb(uint16_t port);
void outb(uint16_t port, uint8_t  val);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t val);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t val);

FILE *romfopen(char *fn, char *mode);
extern int shadowbios,shadowbios_write;
extern int cache;
extern int mem_size;
extern int readlnum,writelnum;
extern int memwaitstate;


/*Processor*/
#define EAX regs[0].l
#define ECX regs[1].l
#define EDX regs[2].l
#define EBX regs[3].l
#define ESP regs[4].l
#define EBP regs[5].l
#define ESI regs[6].l
#define EDI regs[7].l
#define AX regs[0].w
#define CX regs[1].w
#define DX regs[2].w
#define BX regs[3].w
#define SP regs[4].w
#define BP regs[5].w
#define SI regs[6].w
#define DI regs[7].w
#define AL regs[0].b.l
#define AH regs[0].b.h
#define CL regs[1].b.l
#define CH regs[1].b.h
#define DL regs[2].b.l
#define DH regs[2].b.h
#define BL regs[3].b.l
#define BH regs[3].b.h

typedef union
{
        uint32_t l;
        uint16_t w;
        struct
        {
                uint8_t l,h;
        } b;
} x86reg;

x86reg regs[8];
uint16_t flags,eflags;
uint32_t /*cs,ds,es,ss,*/oldds,oldss,pc,olddslimit,oldsslimit,olddslimitw,oldsslimitw;
//uint16_t msw;

extern int ins,output;
extern int cycdiff;

typedef struct
{
        uint32_t base;
        uint32_t limit;
        uint8_t access;
        uint16_t seg;
        uint32_t limit_low, limit_high;
        int checked; /*Non-zero if selector is known to be valid*/
} x86seg;

x86seg gdt,ldt,idt,tr;
x86seg _cs,_ds,_es,_ss,_fs,_gs;
x86seg _oldds;

extern x86seg *ea_seg;

uint32_t pccache;
uint8_t *pccache2;
/*Segments -
  _cs,_ds,_es,_ss are the segment structures
  CS,DS,ES,SS is the 16-bit data
  cs,ds,es,ss are defines to the bases*/
//uint16_t CS,DS,ES,SS;
#define CS _cs.seg
#define DS _ds.seg
#define ES _es.seg
#define SS _ss.seg
#define FS _fs.seg
#define GS _gs.seg
#define cs _cs.base
#define ds _ds.base
#define es _es.base
#define ss _ss.base
#define seg_fs _fs.base
#define gs _gs.base

#define CPL ((_cs.access>>5)&3)

void loadseg(uint16_t seg, x86seg *s);
void loadcs(uint16_t seg);

union
{
        uint32_t l;
        uint16_t w;
} CR0;

#define cr0 CR0.l
#define msw CR0.w

uint32_t cr2, cr3, cr4;
uint32_t dr[8];

#define C_FLAG  0x0001
#define P_FLAG  0x0004
#define A_FLAG  0x0010
#define Z_FLAG  0x0040
#define N_FLAG  0x0080
#define T_FLAG  0x0100
#define I_FLAG  0x0200
#define D_FLAG  0x0400
#define V_FLAG  0x0800
#define NT_FLAG 0x4000
#define VM_FLAG 0x0002 /*In EFLAGS*/

#define WP_FLAG 0x10000 /*In CR0*/

#define IOPL ((flags>>12)&3)

#define IOPLp ((!(msw&1)) || (CPL<=IOPL))
//#define IOPLp 1

//#define IOPLV86 ((!(msw&1)) || (CPL<=IOPL))
extern int cycles;
extern int cycles_lost;
extern int is486;
extern uint8_t opcode;
extern int insc;
extern int fpucount;
extern float mips,flops;
extern int clockrate;
extern int cgate16;
extern int CPUID;

extern int cpl_override;

/*Timer*/
typedef struct PIT
{
        uint32_t l[3];
        int c[3];
        uint8_t m[3];
        uint8_t ctrl,ctrls[3];
        int wp,rm[3],wm[3];
        uint16_t rl[3];
        int thit[3];
        int delay[3];
        int rereadlatch[3];
        int gate[3];
        int out[3];
        int running[3];
        int enabled[3];
        int newcount[3];
        int count[3];
        int using_timer[3];
        int initial[3];
        int latched[3];
        int disabled[3];
} PIT;

PIT pit;
void setpitclock(float clock);
int pitcount;

float pit_timer0_freq();



/*DMA*/
typedef struct DMA
{
        uint16_t ab[4],ac[4];
        uint16_t cb[4];
        int cc[4];
        int wp;
        uint8_t m,mode[4];
        uint8_t page[4];
        uint8_t stat;
        uint8_t command;
} DMA;

DMA dma,dma16;


/*PPI*/
typedef struct PPI
{
        int s2;
        uint8_t pa,pb;
} PPI;

PPI ppi;
extern int key_inhibit;


/*PIC*/
typedef struct PIC
{
        uint8_t icw1,mask,ins,pend,mask2;
        int icw;
        uint8_t vector;
        int read;
} PIC;

PIC pic,pic2;
extern int pic_intpending;
int intcount;


int disctime;
char discfns[2][256];
int driveempty[2];

#define MDA ((gfxcard==GFX_MDA || gfxcard==GFX_HERCULES) && (romset<ROM_TANDY || romset>=ROM_IBMAT))
#define VGA ((gfxcard>=GFX_TVGA || romset==ROM_ACER386) && romset!=ROM_PC1640 && romset!=ROM_PC1512 && romset!=ROM_TANDY && romset!=ROM_PC200)
#define PCJR (romset == ROM_IBMPCJR)
#define AMIBIOS (romset==ROM_AMI386 || romset==ROM_AMI486 || romset == ROM_WIN486)

int GAMEBLASTER, GUS, SSI2001, voodoo_enabled;
extern int AMSTRAD, AT, is386, PCI, TANDY;

enum
{
        ROM_IBMPC = 0,  /*301 keyboard error, 131 cassette (!!!) error*/
        ROM_IBMXT,      /*301 keyboard error*/
        ROM_IBMPCJR,
        ROM_GENXT,      /*'Generic XT BIOS'*/
        ROM_DTKXT,
        ROM_EUROPC,
        ROM_OLIM24,
        ROM_TANDY,
        ROM_PC1512,
        ROM_PC200,
        ROM_PC1640,
        ROM_PC2086,
        ROM_PC3086,        
        ROM_AMIXT,      /*XT Clone with AMI BIOS*/
	ROM_LTXT,
	ROM_LXT3,
	ROM_PX386,
        ROM_DTK386,
        ROM_PXXT,
        ROM_JUKOPC,
        ROM_TANDY1000HX,
        ROM_TANDY1000SL2,
        ROM_IBMAT,
        ROM_CMDPC30,
        ROM_AMI286,
        ROM_DELL200,
        ROM_MISC286,
        ROM_IBMAT386,
        ROM_ACER386,
        ROM_MEGAPC,
        ROM_AMI386,
        ROM_AMI486,
        ROM_WIN486,
        ROM_PCI486,
        ROM_SIS496,
        ROM_430VX,
        ROM_ENDEAVOR,
        ROM_REVENGE,
        ROM_IBMPS1_2011,
        ROM_DESKPRO_386,

        ROM_DTK486,     /*DTK PKM-0038S E-2 / SiS 471 / Award BIOS / SiS 85C471*/
        ROM_VLI486SV2G, /*ASUS VL/I-486SV2G / SiS 471 / Award BIOS / SiS 85C471*/
        ROM_R418,       /*Rise Computer R418 / SiS 496/497 / Award BIOS / SMC FDC37C665*/
	ROM_PLATO,      /*Intel Premiere/PCI II / 430NX / AMI BIOS / SMC FDC37C665*/
        ROM_MB500N,     /*PC Partner MB500N / 430FX / Award BIOS / SMC FDC37C665*/
	ROM_ACERM3A,    /*Acer M3A / 430HX / Acer BIOS / SMC FDC37C932FR*/
	ROM_ACERV35N,   /*Acer V35N / 430HX / Acer BIOS / SMC FDC37C932FR*/
        ROM_P55T2P4,    /*ASUS P/I-P55T2P4 / 430HX / Award BIOS / Winbond W8387FF*/
        ROM_P55VA,      /*Epox P55-VA / 430VX / Award BIOS / SMC FDC37C932FR*/
	
        ROM_MAX
};

extern int romspresent[ROM_MAX];

int hasfpu;
int romset;

enum
{
        GFX_CGA = 0,
	GFX_NEW_CGA,
        GFX_MDA,
        GFX_HERCULES,
        GFX_EGA,        /*Using IBM EGA BIOS*/
	GFX_COMPAQ_EGA,	/*Compaq EGA*/
	GFX_SUPER_EGA,	/*Using Chips & Technologies SuperEGA BIOS*/
        GFX_TVGA,       /*Using Trident TVGA8900D BIOS*/
        GFX_ET4000,     /*Tseng ET4000*/
        GFX_ET4000W32,  /*Tseng ET4000/W32p (Diamond Stealth 32)*/
        GFX_BAHAMAS64,  /*S3 Vision864 (Paradise Bahamas 64)*/
        GFX_N9_9FX,     /*S3 764/Trio64 (Number Nine 9FX)*/
        GFX_VIRGE,      /*S3 Virge*/
        GFX_TGUI9440,   /*Trident TGUI9440*/
        GFX_VGA,        /*IBM VGA*/        
	GFX_COMPAQ_VGA,	/*Compaq/Paradise VGA*/
        GFX_VGAEDGE16,  /*ATI VGA Edge-16 (18800-1)*/
        GFX_VGACHARGER, /*ATI VGA Charger (28800-5)*/
        GFX_OTI067,     /*Oak OTI-067*/
        GFX_MACH64GX,   /*ATI Graphics Pro Turbo (Mach64)*/
        GFX_CL_GD5429,  /*Cirrus Logic CL-GD5429*/
        GFX_VIRGEDX,    /*S3 Virge/DX*/
        GFX_PHOENIX_TRIO32, /*S3 732/Trio32 (Phoenix)*/
        GFX_PHOENIX_TRIO64, /*S3 764/Trio64 (Phoenix)*/
        GFX_RIVATNT,
        GFX_INCOLOR,
        GFX_RIVA128,
        
        GFX_MAX
};

extern int gfx_present[GFX_MAX];

int gfxcard;

int cpuspeed;


/*Video*/
void (*pollvideo)();
void pollega();
int readflash;
uint8_t hercctrl;
int slowega,egacycles,egacycles2;
extern uint8_t gdcreg[16];
extern int egareads,egawrites;
extern int cga_comp;
extern int vid_resize;
extern int vid_api;
extern int winsizex,winsizey;
extern int chain4;

uint8_t readvram(uint16_t addr);
void writevram(uint16_t addr, uint8_t val);
void writevramgen(uint16_t addr, uint8_t val);

uint8_t readtandyvram(uint16_t addr);
void writetandy(uint16_t addr, uint8_t val);
void writetandyvram(uint16_t addr, uint8_t val);

extern int et4k_b8000;
extern int changeframecount;
extern uint8_t changedvram[(8192*1024)/1024];

void writeega_chain4(uint32_t addr, uint8_t val);
extern uint32_t svgarbank,svgawbank;

/*Serial*/
extern int mousedelay;


/*Sound*/
uint8_t spkstat;

float spktime;
int rtctime;
int soundtime,gustime,gustime2,vidtime;
int ppispeakon;
float CGACONST;
float MDACONST;
float VGACONST1,VGACONST2;
float RTCCONST;
int gated,speakval,speakon;

#define SOUNDBUFLEN (48000/10)


/*Sound Blaster*/
/*int sbenable,sblatchi,sblatcho,sbcount,sb_enable_i,sb_count_i;
int16_t sbdat;*/
void setsbclock(float clock);

#define SADLIB    1     /*No DSP*/
#define SB1       2     /*DSP v1.05*/
#define SB15      3     /*DSP v2.00*/
#define SB2       4     /*DSP v2.01 - needed for high-speed DMA*/
#define SBPRO     5     /*DSP v3.00*/
#define SBPRO2    6     /*DSP v3.02 + OPL3*/
#define SB16      7     /*DSP v4.05 + OPL3*/
#define SADGOLD   8     /*AdLib Gold*/
#define SND_WSS   9     /*Windows Sound System*/
#define SND_PAS16 10    /*Pro Audio Spectrum 16*/

int sbtype;

int clocks[3][12][4];
int at70hz;

char pcempath[512];


/*Hard disc*/

typedef struct
{
        FILE *f;
        int spt,hpc; /*Sectors per track, heads per cylinder*/
        int tracks;
} PcemHDC;

PcemHDC hdc[4];

/*Keyboard*/
int keybsenddelay;


/*CD-ROM*/
extern int cdrom_drive;
extern int old_cdrom_drive;
extern int idecallback[3];
extern int cdrom_enabled;

#define CD_STATUS_EMPTY		0
#define CD_STATUS_DATA_ONLY	1
#define CD_STATUS_PLAYING	2
#define CD_STATUS_PAUSED	3
#define CD_STATUS_STOPPED	4

extern uint32_t atapi_get_cd_volume(int channel);


void pclog(const char *format, ...);
extern int nmi;

extern int times;


extern float isa_timing, bus_timing;

extern int frame;


uint8_t *vramp;

uint64_t timer_read();
extern uint64_t timer_freq;


void loadconfig(char *fn);

extern int infocus;

void onesec();

void resetpc_cad();

extern int start_in_fullscreen;
