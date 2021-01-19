/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __TESTBENCH_SPIM_VERIF_HPP__
#define __TESTBENCH_SPIM_VERIF_HPP__

#include <vp/vp.hpp>
#include "testbench.hpp"
#include "vp/time/time_engine.hpp"

#define SPIM_VERIF_CMD_BIT 56
#define SPIM_VERIF_CMD_WIDTH 8

#define SPIM_VERIF_CMD_INFO_BIT 32
#define SPIM_VERIF_CMD_INFO_WIDTH 24

#define SPIM_VERIF_CMD_ADDR_BIT 0
#define SPIM_VERIF_CMD_ADDR_WIDTH 32

#define SPIM_VERIF_FIELD_GET(value, bit, width) (((value) >> (bit)) & ((1UL << (width)) - 1))

typedef enum
{
    STATE_GET_CMD,
    STATE_WRITE_CMD,
    STATE_FULL_DUPLEX_CMD,
    STATE_READ_CMD
} Spim_verif_state_e;

typedef enum
{
    SPIM_VERIF_CMD_WRITE = 1,
    SPIM_VERIF_CMD_READ = 2,
    SPIM_VERIF_CMD_FULL_DUPLEX = 3,
    SPIM_VERIF_CMD_WRITE_QUAD = 4,
    SPIM_VERIF_CMD_READ_QUAD = 5,
} spim_cmd_e;

typedef enum
{
    SPIS_STATE_IDLE,
    SPIS_STATE_START_WAIT_CYCLES,
    SPIS_STATE_CS_START_PRE_START_CYCLES,
    SPIS_STATE_CS_START,
    SPIS_STATE_CS_START_POST_WAIT_CYCLES,
    SPIS_STATE_TRANSFER,
    SPIS_STATE_CS_END_WAIT_CYCLES,
    SPIS_STATE_CS_END,
    SPIS_STATE_END_WAIT_CYCLES
}
spis_state_e;

class Testbench;
class Spi;

class Spim_verif : public vp::time_engine_client
{
public:
    Spim_verif(Testbench *top, Spi *spi, vp::qspim_slave *itf, pi_testbench_req_spim_verif_setup_t *config);
    void cs_sync(int cs);
    void sync(int sck, int sdio0, int sdio1, int sdio2, int sdio3, int mask);
    void transfer(pi_testbench_req_spim_verif_transfer_t *transfer);
    int64_t exec();


protected:
    void handle_clk_high(int sdio0, int sdio1, int sdio2, int sdio3, int mask);
    void handle_clk_low(int sdio0, int sdio1, int sdio2, int sdio3, int mask);

private:
    void handle_read(uint64_t cmd, bool is_quad);
    void handle_write(uint64_t cmd, bool is_quad);
    void handle_full_duplex(uint64_t cmd);

    void exec_write(int sdio0, int sdio1, int sdio2, int sdio3);
    void exec_read();
    void exec_dump_single(int sdio0);
    void exec_dump_qpi(int sdio0, int sdio1, int sdio2, int sdio3);

    void handle_command(uint64_t cmd);

    vp::trace trace;
    vp::qspim_slave *itf;
    Spim_verif_state_e state = STATE_GET_CMD;

    uint64_t current_cmd = 0;
    int prev_sck = 0;
    int cmd_count = 0;
    int dummy_cycles = 0;
    bool wait_cs;
    int command_addr;
    int current_addr;
    int current_write_addr;
    int current_size;
    int current_write_size;
    unsigned char *data;
    int nb_bits;
    int nb_write_bits;
    uint32_t byte;
    bool verbose;
    unsigned int pending_write;
    int current_cs;
    FILE *tx_file;
    int tx_dump_bits;
    bool tx_dump_qpi;
    int tx_dump_byte;
    int mem_size;
    bool is_quad;
    bool is_master;

    Testbench *top;
    Spi *spi;

    int64_t slave_period;
    int slave_sck;
    int slave_wait_cycles;

    int          slave_pending_rx_bits;
    uint32_t     slave_pending_rx_byte;
    int          slave_pending_rx_addr;
    int          slave_pending_rx_size;

    int          slave_pending_tx_bits;
    uint32_t     slave_pending_tx_byte;
    int          slave_pending_tx_addr;
    int          slave_pending_tx_size;

    int          slave_pending_is_tx;
    int          slave_pending_is_rx;
    unsigned int slave_pending_miso;
    unsigned int slave_pending_mosi;

    spis_state_e slave_state;

};

#endif