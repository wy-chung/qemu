/*
 * Common definitions for the softmmu tlb
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef EXEC_TLB_COMMON_H
#define EXEC_TLB_COMMON_H 1

#define CPU_TLB_ENTRY_BITS 5 // tlb entry size in 2^n

/* Minimalized TLB entry for use by TCG fast path. */
typedef union CPUTLBEntryFast {
    struct {
        uint64_t addr_read;	// accessed by tlb_read_type(MMU_DATA_LOAD)
        uint64_t addr_write;	// accessed by tlb_read_type(MMU_DATA_STORE)
        uint64_t addr_code;	// accessed by tlb_read_type(MMU_INST_FETCH)
        /*
         * Addend to virtual address to get host address.  IO accesses
         * use the corresponding iotlb value.
         */
        uintptr_t addend;
    };
    /*
     * Padding to get a power of two size, as well as index
     * access to addr_{read,write,code}.
     */
    uint64_t addr_idx[(1 << CPU_TLB_ENTRY_BITS) / sizeof(uint64_t)];
} CPUTLBEntryFast;

QEMU_BUILD_ASSERT(sizeof(CPUTLBEntryFast) == (1 << CPU_TLB_ENTRY_BITS));

/*
 * Data elements that are per MMU mode, accessed by the fast path.
 * The structure is aligned to aid loading the pair with one insn.
 */
typedef struct CPUTLBDescFast {
    // mask is also referenced by offsetof in prepare_host_addr()
    uintptr_t mask; // Contains (n_entries - 1) << CPU_TLB_ENTRY_BITS. n_entries is 2^n
    /* The array of tlb entries itself. */
    CPUTLBEntryFast *table; // array size is stored in mask
} CPUTLBDescFast QEMU_ALIGNED(2 * sizeof(void *));

#endif /* EXEC_TLB_COMMON_H */
