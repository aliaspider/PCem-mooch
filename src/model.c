#include "ibm.h"
#include "cpu.h"
#include "model.h"
#include "io.h"

#include "acer386sx.h"
#include "acerm3a.h"
#include "ali1429.h"
#include "amstrad.h"
#include "compaq.h"
#include "device.h"
#include "dma.h"
#include "fdc.h"
#include "fdc37c665.h"
#include "fdc37c932fr.h"
#include "gameport.h"
#include "headland.h"
#include "i430fx.h"
#include "i430hx.h"
#include "i430lx.h"
#include "i430nx.h"
#include "i430vx.h"
#include "ide.h"
#include "intel.h"
#include "intel_flash.h"
#include "jim.h"
#include "keyboard_amstrad.h"
#include "keyboard_at.h"
#include "keyboard_olim24.h"
#include "keyboard_pcjr.h"
#include "keyboard_xt.h"
#include "lpt.h"
#include "memregs.h"
#include "mouse_ps2.h"
#include "mouse_serial.h"
#include "neat.h"
#include "nmi.h"
#include "nvr.h"
#include "olivetti_m24.h"
#include "pc87306.h"
#include "pci.h"
#include "pic.h"
#include "piix.h"
#include "pit.h"
#include "ps1.h"
#include "serial.h"
#include "sis496.h"
#include "sis85c471.h"
#include "sio.h"
#include "sound_pssj.h"
#include "sound_sn76489.h"
#include "tandy_eeprom.h"
#include "tandy_rom.h"
#include "um8669f.h"
// #include "um8881f.h"
#include "w83877f.h"
#include "wd76c10.h"
#include "xtide.h"

void            xt_init();
void          pcjr_init();
void       tandy1k_init();
void    tandy1ksl2_init();
void           ams_init();
void        europc_init();
void        olim24_init();
void            at_init();
void    deskpro386_init();
void           ps1_init();
void       at_neat_init();
void  at_acer386sx_init();
void    at_wd76c10_init();
void    at_ali1429_init();
void   at_headland_init();
// void    at_um8881f_init();
void     at_sis496_init();
void     at_i430vx_init();
void     at_batman_init();
void   at_endeavor_init();

void     at_dtk486_init();
void       at_r418_init();
void      at_plato_init();
void     at_mb500n_init();
void    at_acerm3a_init();
void   at_acerv35n_init();
void    at_p55t2p4_init();
void      at_p55va_init();

int model;

int AMSTRAD, AT, PCI, TANDY;

MODEL models[] =
{
        {"IBM PC",              ROM_IBMPC,       { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0,  64, 640, 64,     xt_init},
        {"IBM XT",              ROM_IBMXT,       { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
        {"IBM PCjr",            ROM_IBMPCJR,     { "",      cpus_pcjr,    "",    NULL,         "",      NULL},         1, 0, 128, 640, 128,  pcjr_init},
        {"Generic XT clone",    ROM_GENXT,       { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
        {"AMI XT clone",        ROM_AMIXT,       { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
        {"DTK XT clone",        ROM_DTKXT,       { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},        
        {"VTech Laser Turbo XT",ROM_LTXT,        { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
	{"VTech Laser XT3",	ROM_LXT3,   	 { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
        {"Phoenix XT clone",    ROM_PXXT,        { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
        {"Juko XT clone",       ROM_JUKOPC,      { "",      cpus_8088,    "",    NULL,         "",      NULL},         0, 0, 128, 640, 64,     xt_init},
        {"Tandy 1000",          ROM_TANDY,       { "",      cpus_8088,    "",    NULL,         "",      NULL},         1, 0, 128, 640, 128, tandy1k_init},
        {"Tandy 1000 HX",       ROM_TANDY1000HX, { "",      cpus_8088,    "",    NULL,         "",      NULL},         1, 0, 256, 640, 128, tandy1k_init},
        {"Tandy 1000 SL/2",     ROM_TANDY1000SL2,{ "",      cpus_8086,    "",    NULL,         "",      NULL},         1, 0, 512, 768, 128, tandy1ksl2_init},
        {"Amstrad PC1512",      ROM_PC1512,      { "",      cpus_pc1512,  "",    NULL,         "",      NULL},         1, 0, 512, 640, 128,     ams_init},
        {"Sinclair PC200",      ROM_PC200,       { "",      cpus_8086,    "",    NULL,         "",      NULL},         1, 0, 512, 640, 128,     ams_init},
        {"Euro PC",             ROM_EUROPC,      { "",      cpus_8086,    "",    NULL,         "",      NULL},         0, 0, 512, 640, 128,  europc_init},
        {"Olivetti M24",        ROM_OLIM24,      { "",      cpus_8086,    "",    NULL,         "",      NULL},         1, 0, 128, 640, 128,  olim24_init},
        {"Amstrad PC1640",      ROM_PC1640,      { "",      cpus_8086,    "",    NULL,         "",      NULL},         1, 0, 640, 640, 0,       ams_init},
        {"Amstrad PC2086",      ROM_PC2086,      { "",      cpus_8086,    "",    NULL,         "",      NULL},         1, 0, 640, 640, 0,       ams_init},
        {"Amstrad PC3086",      ROM_PC3086,      { "",      cpus_8086,    "",    NULL,         "",      NULL},         1, 0, 640, 640, 0,       ams_init},
        {"IBM AT",              ROM_IBMAT,       { "",      cpus_ibmat,   "",    NULL,         "",      NULL},         0, 1,   1,  16, 1,        at_init},
        {"Commodore PC 30 III", ROM_CMDPC30,     { "",      cpus_286,     "",    NULL,         "",      NULL},         0, 1,   1,  16, 1,        at_init},
        {"AMI 286 clone",       ROM_AMI286,      { "",      cpus_286,     "",    NULL,         "",      NULL},         0, 1,   1,  16, 1,   at_neat_init},        
        {"DELL System 200",     ROM_DELL200,     { "",      cpus_286,     "",    NULL,         "",      NULL},         0, 1,   1,  16, 1,        at_init},
        {"IBM PS/1 model 2011", ROM_IBMPS1_2011, { "",      cpus_286,     "",    NULL,         "",      NULL},         1, 1,   1,  16, 1,       ps1_init},
        {"Compaq Deskpro 386",  ROM_DESKPRO_386, { "Intel", cpus_i386,    "AMD", cpus_Am386,   "Cyrix", cpus_486SDLC}, 0, 1,   1,  15, 1,     deskpro386_init},
        {"Acer 386SX25/N",      ROM_ACER386,     { "Intel", cpus_acer,    "",    NULL,         "",      NULL},         1, 1,   1,  16, 1,   at_acer386sx_init},
        {"DTK 386SX clone",     ROM_DTK386,      { "Intel", cpus_i386,    "AMD", cpus_Am386,   "Cyrix", cpus_486SDLC}, 0, 1,   1,  16, 1,        at_neat_init},
        {"Phoenix 386 clone",   ROM_PX386,       { "Intel", cpus_i386,    "AMD", cpus_Am386,   "Cyrix", cpus_486SDLC}, 0, 1,   1,  16, 1,             at_init},
        {"Amstrad MegaPC",      ROM_MEGAPC,      { "Intel", cpus_i386,    "AMD", cpus_Am386,   "Cyrix", cpus_486SDLC}, 1, 1,   1,  16, 1,     at_wd76c10_init},
        {"AMI 386 clone",       ROM_AMI386,      { "Intel", cpus_i386,    "AMD", cpus_Am386,   "Cyrix", cpus_486SDLC}, 0, 1,   1, 256, 1,    at_headland_init},
        {"AMI 486 clone",       ROM_AMI486,      { "Intel", cpus_i486,    "AMD", cpus_Am486,   "Cyrix", cpus_Cx486},   0, 1,   1, 256, 1,     at_ali1429_init},
        {"AMI WinBIOS 486",     ROM_WIN486,      { "Intel", cpus_i486,    "AMD", cpus_Am486,   "Cyrix", cpus_Cx486},   0, 1,   1, 256, 1,     at_ali1429_init},
/*        {"AMI WinBIOS 486 PCI", ROM_PCI486,    { "Intel", cpus_i486,    "AMD", cpus_Am486, "Cyrix", cpus_Cx486},   0, 1,  1, 256, 1, at_um8881f_init},*/
        {"DTK PKM-0038S E-2",   ROM_DTK486,      { "Intel", cpus_i486,    "AMD", cpus_Am486,   "Cyrix", cpus_Cx486},   0, 1,   1, 256, 1,      at_dtk486_init},
        {"Award SiS 496/497",   ROM_SIS496,      { "Intel", cpus_i486,    "AMD", cpus_Am486,   "Cyrix", cpus_Cx486},   0, 1,   1, 256, 1,      at_sis496_init},
        {"Rise Computer R418",  ROM_R418,        { "Intel", cpus_i486,    "AMD", cpus_Am486,   "Cyrix", cpus_Cx486},   0, 1,   1, 256, 1,        at_r418_init},
        {"Intel Premiere/PCI",  ROM_REVENGE,     { "Intel", cpus_Pentium5V, "",  NULL,         "",      NULL},         0, 1,   1, 128, 1,      at_batman_init},
        {"Intel Premiere/PCI II",ROM_PLATO,      { "Intel", cpus_PentiumS5, "",  NULL,         "",      NULL},         0, 1,   1, 128, 1,       at_plato_init},
        {"Intel Advanced/EV",   ROM_ENDEAVOR,    { "Intel", cpus_PentiumS5,"IDT", cpus_WinChip, "",      NULL},         0, 1,   1, 128, 1,   at_endeavor_init},
        {"PC Partner MB500N",   ROM_MB500N,      { "Intel", cpus_PentiumS5,"IDT", cpus_WinChip, "",      NULL},         0, 1,   1, 128, 1,     at_mb500n_init},
        {"Acer M3a",            ROM_ACERM3A,     { "Intel", cpus_Pentium, "IDT", cpus_WinChip, "AMD",   cpus_K6},         0, 1,   1, 512, 1,     at_acerm3a_init},
        {"Acer V35N",           ROM_ACERV35N,    { "Intel", cpus_Pentium, "IDT", cpus_WinChip, "AMD",   cpus_K6},         0, 1,   1, 512, 1,    at_acerv35n_init},
        {"ASUS P/I-P55T2P4",    ROM_P55T2P4,     { "Intel", cpus_Pentium, "IDT", cpus_WinChip, "AMD",   cpus_K6},         0, 1,   1, 512, 1,     at_p55t2p4_init},
        {"Award 430VX PCI",     ROM_430VX,       { "Intel", cpus_Pentium, "IDT", cpus_WinChip, "AMD",   cpus_K6},         0, 1,   1, 256, 1,      at_i430vx_init},
        {"Epox P55-VA",         ROM_P55VA,       { "Intel", cpus_Pentium, "IDT", cpus_WinChip, "AMD",   cpus_K6},         0, 1,   1, 256, 1,       at_p55va_init},
        {"", -1, {"", 0, "", 0, "", 0}, 0,0,0, 0}
};

int model_count()
{
        return (sizeof(models) / sizeof(MODEL)) - 1;
}

int model_getromset()
{
        return models[model].id;
}

int model_getmodel(int romset)
{
	int c = 0;
	
	while (models[c].id != -1)
	{
		if (models[c].id == romset)
			return c;
		c++;
	}
	
	return 0;
}

char *model_getname()
{
        return models[model].name;
}

void common_init()
{
        dma_init();
        fdc_add();
        lpt_init();
        pic_init();
        pit_init();
        serial1_init(0x3f8, 4);
        serial2_init(0x2f8, 3);
        device_add(&gameport_device);
}

void xt_init()
{
        common_init();
        pit_set_out_func(1, pit_refresh_timer_xt);
        keyboard_xt_init();
        mouse_serial_init();
        xtide_init();
	nmi_init();
}

void pcjr_init()
{
        fdc_add_pcjr();
        pic_init();
        pit_init();
        pit_set_out_func(0, pit_irq0_timer_pcjr);
        serial1_init(0x2f8, 3);
        keyboard_pcjr_init();
        device_add(&sn76489_device);
	nmi_mask = 0x80;
}

void tandy1k_init()
{
        TANDY = 1;
        common_init();
        keyboard_tandy_init();
        mouse_serial_init();
        if (romset == ROM_TANDY)
                device_add(&sn76489_device);
        else
                device_add(&ncr8496_device);
        xtide_init();
	nmi_init();
	if (romset != ROM_TANDY)
                device_add(&tandy_eeprom_device);
}
void tandy1ksl2_init()
{
//        TANDY = 1;
        common_init();
        keyboard_tandy_init();
        mouse_serial_init();
        device_add(&pssj_device);
        xtide_init();
	nmi_init();
        device_add(&tandy_rom_device);
        device_add(&tandy_eeprom_device);
}

void ams_init()
{
        AMSTRAD = 1;
        common_init();
        amstrad_init();
        keyboard_amstrad_init();
        nvr_init();
        xtide_init();
	nmi_init();
	fdc_set_dskchg_activelow();
}

void europc_init()
{
        common_init();
        jim_init();
        keyboard_xt_init();
        mouse_serial_init();
        xtide_init();
	nmi_init();
}

void olim24_init()
{
        common_init();
        keyboard_olim24_init();
        nvr_init();
        olivetti_m24_init();
        xtide_init();
	nmi_init();
}

void at_init()
{
        common_init();
        pit_set_out_func(1, pit_refresh_timer_at);
        dma16_init();
        ide_init();
        keyboard_at_init();
        if (models[model].init == at_init)
           mouse_serial_init();
        nvr_init();
        pic2_init();
        AT = 1;
}

void deskpro386_init()
{
        at_init();
        mouse_serial_init();
        compaq_init();
}

void ps1_init()
{
        AT = 1;
        common_init();
        pit_set_out_func(1, pit_refresh_timer_at);
        dma16_init();
        ide_init();
        keyboard_at_init();
        mouse_ps2_init();
        nvr_init();
        pic2_init();
        ps1mb_init();
        fdc_set_dskchg_activelow();
}

void at_neat_init()
{
        at_init();
        mouse_serial_init();
        neat_init();
}

void at_acer386sx_init()
{
        at_init();
        mouse_ps2_init();
        acer386sx_init();
}

void at_wd76c10_init()
{
        at_init();
        mouse_ps2_init();
        wd76c10_init();
}

void at_headland_init()
{
        at_init();
        headland_init();
        mouse_serial_init();
}

void at_ali1429_init()
{
        at_init();
        ali1429_init();
        mouse_serial_init();
}

/* void at_um8881f_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        um8881f_init();
} */

void at_dtk486_init()
{
        at_init();
	memregs_init();
        mouse_serial_init();
	sis85c471_init();
}

void at_sis496_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        device_add(&sis496_device);
}

void at_r418_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        fdc37c665_init();
        device_add(&sis496_device);
}

void at_batman_init()
{
        at_init();
        mouse_ps2_init();
        pci_init(PCI_CONFIG_TYPE_2, 0xd, 0x10);
        i430lx_init();
	sio_init(1);
        fdc37c665_init();
        intel_batman_init();
}

void at_plato_init()
{
        at_init();
	mouse_ps2_init();
        pci_init(PCI_CONFIG_TYPE_2, 0xd, 0x10);
        i430nx_init();
	sio_init(1);
	fdc37c665_init();
	/* It seems it uses the same interface as Batman. */
        intel_batman_init();
        // device_add(&intel_flash_bxt_ami_device);
}

void at_endeavor_init()
{
        at_init();
        mouse_ps2_init();
        pci_init(PCI_CONFIG_TYPE_1, 0xd, 0x10);
        i430fx_init();
        piix_init(7);
        pc87306_init();
        intel_endeavor_init();
        device_add(&intel_flash_bxt_ami_device);
}

void at_mb500n_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        i430fx_init();
        piix_init(7);
        fdc37c665_init();
        intel_endeavor_init();
        device_add(&intel_flash_bxt_device);
}

void at_acerm3a_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0xd, 0x10);
        i430hx_init();
        piix3_init(7);
        fdc37c932fr_init();
        acerm3a_io_init();
        device_add(&intel_flash_bxb_device);
}

void at_acerv35n_init()
{
        at_init();
        mouse_ps2_init();
        pci_init(PCI_CONFIG_TYPE_1, 0xd, 0x10);
        i430hx_init();
        piix3_init(7);
        fdc37c932fr_init();
        acerm3a_io_init();
        device_add(&intel_flash_bxb_device);
}

void at_p55t2p4_init()
{
        at_init();
        mouse_ps2_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        i430hx_init();
        piix3_init(7);
        w83877f_init();
        device_add(&intel_flash_bxt_device);
}

void at_i430vx_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        i430vx_init();
        piix3_init(7);
        um8669f_init();
        device_add(&intel_flash_bxt_device);
}

void at_p55va_init()
{
        at_init();
        mouse_serial_init();
        pci_init(PCI_CONFIG_TYPE_1, 0, 31);
        i430vx_init();
        piix3_init(7);
        fdc37c932fr_init();
        device_add(&intel_flash_bxt_device);
}

void model_init()
{
        pclog("Initting as %s\n", model_getname());
        AMSTRAD = AT = PCI = TANDY = 0;
        io_init();
        
	fdc_update_is_nsc(0);
        models[model].init();
}
