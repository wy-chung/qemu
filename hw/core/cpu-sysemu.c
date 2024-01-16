/*
 * QEMU CPU model (system emulation specific)
 *
 * Copyright (c) 2012-2014 SUSE LINUX Products GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see
 * <http://www.gnu.org/licenses/gpl-2.0.html>
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/core/cpu.h"
#include "hw/core/sysemu-cpu-ops.h"

/**
 * cpu_paging_enabled:
 * @cpu: The CPU whose state is to be inspected.
 *
 * Returns: %true if paging is enabled, %false otherwise.
 */
bool cpu_paging_enabled(const CPUState *cpu)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (cc->sysemu_ops->get_paging_enabled) {
        return cc->sysemu_ops->get_paging_enabled(cpu);
#if defined(WYC)
        return x86_cpu_get_paging_enabled();
#endif
    }

    return false;
}

/**
 * cpu_get_memory_mapping:
 * @cpu: The CPU whose memory mappings are to be obtained.
 * @list: Where to write the memory mappings to.
 * @errp: Pointer for reporting an #Error.
 */
void cpu_get_memory_mapping(CPUState *cpu, MemoryMappingList *list,
                            Error **errp)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (cc->sysemu_ops->get_memory_mapping) {
        cc->sysemu_ops->get_memory_mapping(cpu, list, errp);
#if defined(WYC)
        x86_cpu_get_memory_mapping();
#endif
        return;
    }

    error_setg(errp, "Obtaining memory mappings is unsupported on this CPU.");
}

/**
 * cpu_get_phys_page_attrs_debug:
 * @cpu: The CPU to obtain the physical page address for.
 * @addr: The virtual address.
 * @attrs: Updated on return with the memory transaction attributes to use
 *         for this access.
 *
 * Obtains the physical page corresponding to a virtual one, together
 * with the corresponding memory transaction attributes to use for the access.
 * Use it only for debugging because no protection checks are done.
 *
 * Returns: Corresponding physical page address or -1 if no page found.
 */
hwaddr cpu_get_phys_page_attrs_debug(CPUState *cpu, vaddr addr,
                                     MemTxAttrs *attrs)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (cc->sysemu_ops->get_phys_page_attrs_debug) {
        return cc->sysemu_ops->get_phys_page_attrs_debug(cpu, addr, attrs);
#if defined(WYC)
        return x86_cpu_get_phys_page_attrs_debug();
#endif
    }
    /* Fallback for CPUs which don't implement the _attrs_ hook */
    *attrs = MEMTXATTRS_UNSPECIFIED;
    return cc->sysemu_ops->get_phys_page_debug(cpu, addr); // get_phys_page_debug is null
}

/**
 * cpu_get_phys_page_debug:
 * @cpu: The CPU to obtain the physical page address for.
 * @addr: The virtual address.
 *
 * Obtains the physical page corresponding to a virtual one.
 * Use it only for debugging because no protection checks are done.
 *
 * Returns: Corresponding physical page address or -1 if no page found.
 */
hwaddr cpu_get_phys_page_debug(CPUState *cpu, vaddr addr)
{
    MemTxAttrs attrs = {};

    return cpu_get_phys_page_attrs_debug(cpu, addr, &attrs);
}

/** cpu_asidx_from_attrs:
 * @cpu: CPU
 * @attrs: memory transaction attributes
 *
 * Returns the address space index specifying the CPU AddressSpace
 * to use for a memory access with the given transaction attributes.
 */
int cpu_asidx_from_attrs(CPUState *cpu, MemTxAttrs attrs)
{
    int ret = 0;

    if (cpu->cc->sysemu_ops->asidx_from_attrs) {
        ret = cpu->cc->sysemu_ops->asidx_from_attrs(cpu, attrs);
#if defined(WYC)
        ret = x86_asidx_from_attrs(cpu, attrs);
#endif
        assert(ret < cpu->num_ases && ret >= 0);
    }
    return ret;
}

/**
 * cpu_write_elf32_qemunote:
 * @f: pointer to a function that writes memory to a file
 * @cpu: The CPU whose memory is to be dumped
 * @cpuid: ID number of the CPU
 * @opaque: pointer to the CPUState struct
 */
int cpu_write_elf32_qemunote(WriteCoreDumpFunction f, CPUState *cpu,
                             void *opaque)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (!cc->sysemu_ops->write_elf32_qemunote) {
        return 0;
    }
    return (*cc->sysemu_ops->write_elf32_qemunote)(f, cpu, opaque);
#if defined(WYC)
    return x86_cpu_write_elf32_qemunote();
#endif
}

/**
 * cpu_write_elf32_note:
 * @f: pointer to a function that writes memory to a file
 * @cpu: The CPU whose memory is to be dumped
 * @cpuid: ID number of the CPU
 * @opaque: pointer to the CPUState struct
 */
int cpu_write_elf32_note(WriteCoreDumpFunction f, CPUState *cpu,
                         int cpuid, void *opaque)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (!cc->sysemu_ops->write_elf32_note) {
        return -1;
    }
    return (*cc->sysemu_ops->write_elf32_note)(f, cpu, cpuid, opaque);
#if defined(WYC)
    return x86_cpu_write_elf32_note();
#endif
}

/**
 * cpu_write_elf64_qemunote:
 * @f: pointer to a function that writes memory to a file
 * @cpu: The CPU whose memory is to be dumped
 * @cpuid: ID number of the CPU
 * @opaque: pointer to the CPUState struct
 */
int cpu_write_elf64_qemunote(WriteCoreDumpFunction f, CPUState *cpu,
                             void *opaque)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (!cc->sysemu_ops->write_elf64_qemunote) {
        return 0;
    }
    return (*cc->sysemu_ops->write_elf64_qemunote)(f, cpu, opaque);
#if defined(WYC)
    return x86_cpu_write_elf64_qemunote();
#endif
}

/**
 * cpu_write_elf64_note:
 * @f: pointer to a function that writes memory to a file
 * @cpu: The CPU whose memory is to be dumped
 * @cpuid: ID number of the CPU
 * @opaque: pointer to the CPUState struct
 */
int cpu_write_elf64_note(WriteCoreDumpFunction f, CPUState *cpu,
                         int cpuid, void *opaque)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (!cc->sysemu_ops->write_elf64_note) {
        return -1;
    }
    return (*cc->sysemu_ops->write_elf64_note)(f, cpu, cpuid, opaque);
#if defined(WYC)
    return x86_cpu_write_elf64_note();
#endif
}

/**
 * cpu_virtio_is_big_endian:
 * @cpu: CPU

 * Returns %true if a CPU which supports runtime configurable endianness
 * is currently big-endian.
 */
bool cpu_virtio_is_big_endian(CPUState *cpu)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);

    if (cc->sysemu_ops->virtio_is_big_endian) { // false
        return cc->sysemu_ops->virtio_is_big_endian(cpu);
    }
    return target_words_bigendian();
}

/**
 * cpu_get_crash_info:
 * @cpu: The CPU to get crash information for
 *
 * Gets the previously saved crash information.
 * Caller is responsible for freeing the data.
 */
GuestPanicInformation *cpu_get_crash_info(CPUState *cpu)
{
    CPUClass *cc = CPU_GET_CLASS(cpu);
    GuestPanicInformation *res = NULL;

    if (cc->sysemu_ops->get_crash_info) {
        res = cc->sysemu_ops->get_crash_info(cpu);
#if defined(WYC)
        res = x86_cpu_get_crash_info();
#endif
    }
    return res;
}
