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

#pragma once

#include <vp/vp.hpp>
#include <types.hpp>

#define MMU_TLB_NB_ENTRIES 256
#define MMU_TLB_ENTRIES_MASK 0xff

#define MMU_PGSHIFT 12

#define MMU_MODE_SV39 8

#define MMU_PTE_ATTR  0xFFC0000000000000

#define MMU_PTE_PPN_SHIFT 10

struct Pte
{
    union
    {
        struct
        {
            unsigned int v: 1;
            unsigned int r: 1;
            unsigned int w: 1;
            unsigned int x: 1;
            unsigned int u: 1;
            unsigned int g: 1;
            unsigned int a: 1;
            unsigned int d: 1;
        };
        iss_reg_t raw;
    };
};

class Mmu
{
public:
    Mmu(Iss &iss);

    void build();
    void reset(bool active);

    inline iss_addr_t insn_virt_to_phys(iss_addr_t virt_addr);
    inline iss_addr_t load_virt_to_phys(iss_addr_t virt_addr);
    inline iss_addr_t store_virt_to_phys(iss_addr_t virt_addr);
    iss_addr_t virt_to_phys_miss(iss_addr_t virt_addr);

    bool satp_update(iss_reg_t value);

private:
    bool read_pte(iss_addr_t pte_addr);
    bool walk_pgtab(iss_addr_t virt_addr);
    bool handle_pte();
    static void handle_pte_stub(void *__this, vp::clock_event *event);
    void raise_exception();

    Iss &iss;
    vp::trace trace;
    iss_reg_t satp;
    iss_reg_t pt_base;
    iss_reg_t asid;
    iss_reg_t mode;
    int nb_levels;
    int pte_size;
    int vpn_width;
    iss_addr_t tlb_insn_tag[MMU_TLB_NB_ENTRIES];
    iss_addr_t tlb_phys_addr[MMU_TLB_NB_ENTRIES];

    int current_level;
    int current_vpn_bit;
    iss_addr_t current_virt_addr;
    Pte pte_value;
    bool access_is_load = false;
    bool access_is_store = false;
};