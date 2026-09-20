#ifndef SMCF_REGISTER_H
#define SMCF_REGISTER_H

#define MGI_GRP_ID          (0x000)                 /* RO */
#define MGI_DATA_INFO       (0x008)                 /* RO */
#define MGI_FEAT0           (0x010)                 /* RO */
#define MGI_FEAT1           (0x018)                 /* RO */
#define MGI_SMP_EN          (0x030)                 /* RW */
#define MGI_SMP_CFG         (0x038)                 /* RW */
#define MGI_SMP_PER         (0x040)                 /* RW */
#define MGI_SMP_DLY         (0x048)                 /* RW */
#define MGI_MON_REQ         (0x060)                 /* RW */
#define MGI_MON_STAT        (0x070)                 /* RO */
#define MGI_MODE_BCAST      (0x080)                 /* RW */
#define MGI_MODE_REQ(n)     (0x090 + (8 * (n)))     /* RW */
#define MGI_MODE_STAT(n)    (0x0C0 + (8 * (n)))     /* RO */
#define MGI_IRQ_STAT        (0x100)                 /* RW */
#define MGI_IRQ_MASK        (0x110)                 /* RW */
#define MGI_TRG_MASK        (0x140)                 /* RW */
#define MGI_ERR_CODE        (0x150)                 /* RO */
#define MGI_WREN            (0x160)                 /* RW */
#define MGI_WRCFG           (0x168)                 /* RW */
#define MGI_WADDR0          (0x170)                 /* RW */
#define MGI_WADDR1          (0x174)                 /* RW */
#define MGI_RADDR0          (0x180)                 /* RW */
#define MGI_RADDR1          (0x184)                 /* RW */
#define MGI_DISCON_ID       (0x190)                 /* RO */
#define MGI_CON_STAT        (0x198)                 /* RO */
#define MGI_CMD_SEND0       (0x1B0)                 /* RW */
#define MGI_CMD_SEND1       (0x1B8)                 /* RW */
#define MGI_CMD_RECV0       (0x1C0)                 /* RO */
#define MGI_CMD_RECV1       (0x1C8)                 /* RO */
#define MGI_ATYP(n)         (0x200 + (16 * (n)))    /* RW */
#define MGI_AVAL_LOW(n)     (0x208 + (16 * (n)))    /* RW */
#define MGI_AVAL_HIGH(n)    (0x20C + (16 * (n)))    /* RW */
#define MGI_DATA(n)         (0x700 + (4 * (n)))     /* RO */
#define MGI_DVLD            (0xF00)                 /* RO */
#define MGI_TAG0            (0xF10)                 /* RO */
#define MGI_TAG1            (0xF14)                 /* RO */
#define MGI_TAG2            (0xF18)                 /* RO */
#define MGI_TAG3            (0xF1C)                 /* RO */
#define MGI_SMPID_START     (0xF40)                 /* RO */
#define MGI_SMPID_END       (0xF48)                 /* RO */
#define MGI_IIDR            (0xFC0)                 /* RO */
#define MGI_AIDR            (0xFC8)                 /* RO */

typedef union {
    uint32_t value;
    struct {
        uint32_t grp_id     : 12;   /* Monitor Group Interface Identifier Value */
        uint32_t res0       : 4;
        uint32_t mon_num    : 5;   /* Specifies the number of Monitors in the MGI.
                                      The number of monitors in the MGI is MON_NUM + 1. */
        uint32_t res1       : 11;
    } bits;
} smcf_mgi_grp_id_t;

#endif  //!SMCF_REGISTER_H
