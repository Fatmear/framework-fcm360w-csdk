#ifndef _QL_SPI_H_
#define _QL_SPI_H_

#ifdef __cplusplus
extern "C"
{
#endif
/* 0x20 - spi transfer control register*/
#define QL_SPI_TRANSCTRL_RCNT(x) (((x) & 0x1FF) << 0)
#define QL_SPI_TRANSCTRL_WCNT(x) (((x) & 0x1FF) << 12)
#define QL_SPI_TRANSCTRL_DUALQUAD(x) (((x) & 0x3) << 22)
#define QL_SPI_TRANSCTRL_TRAMODE(x) (((x) & 0xF) << 24)

#define QL_SPI_TRANSCTRL_DUALQUAD_REGULAR QL_SPI_TRANSCTRL_DUALQUAD(0)
#define QL_SPI_TRANSCTRL_DUALQUAD_DUAL QL_SPI_TRANSCTRL_DUALQUAD(1)
#define QL_SPI_TRANSCTRL_DUALQUAD_QUAD QL_SPI_TRANSCTRL_DUALQUAD(2)

#define QL_SPI_TRANSCTRL_TRAMODE_WRCON QL_SPI_TRANSCTRL_TRAMODE(0) /* w/r at the same time */
#define QL_SPI_TRANSCTRL_TRAMODE_WO QL_SPI_TRANSCTRL_TRAMODE(1)    /* write only		*/
#define QL_SPI_TRANSCTRL_TRAMODE_RO QL_SPI_TRANSCTRL_TRAMODE(2)    /* read only		*/
#define QL_SPI_TRANSCTRL_TRAMODE_WR QL_SPI_TRANSCTRL_TRAMODE(3)    /* write, read		*/
#define QL_SPI_TRANSCTRL_TRAMODE_RW QL_SPI_TRANSCTRL_TRAMODE(4)    /* read, write		*/
#define QL_SPI_TRANSCTRL_TRAMODE_WDR QL_SPI_TRANSCTRL_TRAMODE(5)   /* write, dummy, read	*/
#define QL_SPI_TRANSCTRL_TRAMODE_RDW QL_SPI_TRANSCTRL_TRAMODE(6)   /* read, dummy, write	*/
#define QL_SPI_TRANSCTRL_TRAMODE_NONE QL_SPI_TRANSCTRL_TRAMODE(7)  /* none data */
#define QL_SPI_TRANSCTRL_TRAMODE_DW QL_SPI_TRANSCTRL_TRAMODE(8)    /* dummy, write	*/
#define QL_SPI_TRANSCTRL_TRAMODE_DR QL_SPI_TRANSCTRL_TRAMODE(9)    /* dummy, read	*/
#define QL_SPI_TRANSCTRL_CMDEN ((0x1) << 30)
#define QL_SPI_TRANSCTRL_CMDDIS ((0x0) << 30)
#define QL_SPI_TRANSCTRL_ADDREN ((0x1) << 29)
#define QL_SPI_TRANSCTRL_ADDRDIS ((0x0) << 29)
#define QL_SPI_TRANSCTRL_ADDRFMT(x) ((x) << 28)
#define QL_SPI_TRANSCTRL_DUMMY_CNT_1 (0 << 9)
#define QL_SPI_TRANSCTRL_DUMMY_CNT_2 (1 << 9)
#define QL_SPI_TRANSCTRL_DUMMY_CNT_3 (2 << 9)
#define QL_SPI_TRANSCTRL_DUMMY_CNT_4 (3 << 9)
    typedef enum
    {
        QL_SPI_MODE_STANDARD = 0,
        QL_SPI_MODE_DUAL,
        QL_SPI_MODE_QUAD,
        QL_SPI_MODE_QPI,
        QL_SPI_MODE_BI_DIRECTION_MISO,
        QL_SPI_MODE_UNKNOW
    } ql_spi_mode_e;

    typedef struct
    {
        unsigned int addr_len;
        unsigned int data_len;
        unsigned int spi_clk_pol;
        unsigned int spi_clk_pha;
        ql_spi_mode_e spi_trans_mode;
        unsigned char master_clk;
        unsigned int addr_pha_enable;
        unsigned int dummy_bit;
        unsigned int cmd_write;
        unsigned int cmd_read;
        unsigned int spi_dma_enable;
    } ql_spi_master_interface_config_st;
    typedef struct
    {
        unsigned int cmmand;
        unsigned int addr;
        unsigned int length;

    } ql_spi_transaction_st;
    typedef struct
    {
        unsigned int inten;
        unsigned int addr_len;
        unsigned int data_len;
        unsigned int spi_clk_pol;
        unsigned int spi_clk_pha;
        unsigned char slave_clk;
        unsigned int spi_slave_dma_enable;

    } ql_spi_slave_interface_config_t;

    void ql_spi_master_init(ql_spi_master_interface_config_st *spi_master_dev);
    void ql_spi_master_write(ql_spi_transaction_st *config, unsigned char *data);

    /**
    @brief  ql_spi_master_read
    @param[in]  config: communication configuration,
    @param[in]   data: The storage location of the read data,
    @return  -1-- receive data length is not between 0 and 512 ,
             0--spi transfer process comoletion.
*/
    int ql_spi_master_read(ql_spi_transaction_st *config, unsigned char *data);
    void ql_spi_slave_init(ql_spi_slave_interface_config_t *spi_master_dev);

    /**
    @brief  SPI slave transfer function.
    @param[in]  *bufptr: Data to be sent,
    @param[in]   len: the length of the data to be sent,
    @return  -1-- tx_fifo full.
              0-- sent compeletion.
    */
    int ql_spi_slave_write(unsigned char *data, unsigned int length);
    void ql_spi_slave_read(unsigned char *data, unsigned int length);
    void ql_spi_master_tranceive(ql_spi_transaction_st *trans_desc, uint8_t *tx_buf, uint8_t cmd_len, uint8_t *rx_buf);

    /**
     *@brief     Applied to SPI slave mode without using DMA
     *@return    0:     No data received;
                 else:  return data_len
     */
    int ql_spi_slave_read_wait(void);

    /*        ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓标准API↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓        */
    typedef enum
    {
        QL_SPI_ID1 = 0,
        QL_SPI_ID_MAX,
    } ql_spi_id_e;

    typedef enum
    {
        QL_SPI_MSB_FIRST = 0,
        QL_SPI_LSB_FIRST,
    } ql_spi_bit_order_e;

    typedef enum
    {
        QL_SPI_ROLE_SLAVE = 0, /**< SPI as Master */
        QL_SPI_ROLE_MASTER,    /**< SPI ad SLAVE */
    } ql_spi_role_e;

    typedef enum
    {
        QL_SPI_BIT_WIDTH_8BITS = 0,
        QL_SPI_BIT_WIDTH_16BITS,
    } ql_spi_bit_width_e;

    typedef enum
    {
        QL_SPI_4WIRE_MODE = 0, // with CS pin
        QL_SPI_3WIRE_MODE,     // without CS pin
    } ql_spi_wire_mode_e;

    typedef enum
    {
        QL_SPI_POL_PHA_MODE0 = 0, /**< CPOL_L, CPHA_L */
        QL_SPI_POL_PHA_MODE1,     /**< CPOL_L, CPHA_H */
        QL_SPI_POL_PHA_MODE2,     /**< CPOL_H, CPHA_L */
        QL_SPI_POL_PHA_MODE3,     /**< CPOL_H, CPHA_H */
    } ql_spi_pol_pha_mode_e;

    typedef struct
    {
        const void *send_buf; /**< the data need to send */
        uint32_t send_len;    /**< the data length to send */

        void *recv_buf;    /**< the data need to receive */
        uint32_t recv_len; /**< the data length need to receive */
    } ql_spi_message_s;

    typedef enum
    {
        QL_SPI_SUCCESS = 0,
        QL_SPI_EXECUTE_ERR,
        QL_SPI_INVALID_PARAM_ERR,
        QL_SPI_NOT_INIT_ERR,
    } ql_spi_errcode_e;

    typedef enum
    {
        isr,
    } ql_spi_isr_type_e;

    typedef struct
    {
        uint32_t clk;
        uint8_t dma;
        ql_spi_role_e role;
        ql_spi_pol_pha_mode_e pol_pha_mode;
        ql_spi_bit_order_e bit_order;
        ql_spi_wire_mode_e wire_mode;
        ql_spi_bit_width_e bit_width;
        unsigned int dummy_bit;/*fcm360w*/
        unsigned int cmd_write;
        unsigned int cmd_read;
    } ql_spi_config_s;

    /**
     * @brief interrupt isr function type.
     *
     */
    void (*ql_spi_isr_f)(void *param);

    /**
     * @brief
     *
     * @param spi_cfg
     * @param spi_msg
     * @return ql_spi_errcode_e
     */
    ql_spi_errcode_e ql_spi_init(ql_spi_id_e spi_id, ql_spi_config_s *spi_cfg);

    /**
     * @brief
     *
     * @param spi_id
     * @return ql_spi_errcode_e
     */
    ql_spi_errcode_e ql_spi_deinit(ql_spi_id_e spi_id);

    /**
     * @brief
     *
     * @param spi_cfg
     * @param spi_msg
     * @return ql_spi_errcode_e
     */
    ql_spi_errcode_e ql_spi_transfer(ql_spi_id_e spi_id, ql_spi_message_s *spi_msg);

    /**
     * @brief
     *
     * @param spi_id
     * @param data
     * @param size
     * @return ql_spi_errcode_e
     */
    ql_spi_errcode_e ql_spi_write_bytes(ql_spi_id_e spi_id, void *data, uint32_t size);

    /**
     * @brief
     *
     * @param spi_id
     * @param data
     * @param size
     * @return ql_spi_errcode_e or The remaining data length to be accepted
     */
    ql_spi_errcode_e ql_spi_read_bytes(ql_spi_id_e spi_id, void *data, uint32_t size);

    // /**
    //  * @brief
    //  *
    //  * @param spi_id
    //  * @param isr_type
    //  * @param isr_cb
    //  * @return ql_spi_errcode_e
    //  */
    // ql_spi_errcode_e ql_spi_register_isr(ql_spi_id_e spi_id, ql_spi_isr_type_e isr_type, ql_spi_isr_f isr_cb);

    // /**
    //  * @brief
    //  *
    //  * @param spi_id
    //  * @param isr_type
    //  * @param isr_cb
    //  * @return ql_spi_errcode_e
    //  */
    // ql_spi_errcode_e ql_spi_unregister_isr(ql_spi_id_e spi_id, ql_spi_isr_type_e isr_type);

#ifdef __cplusplus
} /*"C" */
#endif

#endif