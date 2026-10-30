// SPDX-License-Identifier: BSD-2-Clause-Patent
// Optional write-combined CPU mapping of the firmware scanout (boot-arg offline-legacy-fb-wc=1, default off).
// The scanout lies outside the kernel's DRAM, so the default kIOMapInhibitCache mapping is Device-nGnRnE and every pixel
// store waits for completion. kIOMapWriteCombineCache is Normal non-cacheable on ARM: no cache line is allocated and stores
// merge in the write buffer; the caller's dsb after a frame makes them visible to the display DMA.
// The mapping comes from a private descriptor over the provider's range. A device pager keeps one cache mode for its
// object and later default-cache mappings (a GPU client's scanout import, for example) inherit the last explicit mode;
// a private descriptor has its own pager, so the provider's descriptor and every mapping made from it stay uncached.
#pragma once
#include <IOKit/IOService.h>
#include <IOKit/IODeviceMemory.h>
#include <IOKit/IOMemoryDescriptor.h>
#include <pexpert/pexpert.h>
static inline bool native_scanout_write_combine_requested(void) {
    unsigned value=0;PE_parse_boot_argn("offline-legacy-fb-wc",&value,sizeof(value));return value==1;
}
static inline IOMemoryMap *native_scanout_write_combined_map(IOService *provider) {
    IODeviceMemory *range=provider?provider->getDeviceMemoryWithIndex(0):nullptr;
    if(!range || !range->getLength())return nullptr;
    IOMemoryDescriptor *own=IOMemoryDescriptor::withPhysicalAddress(range->getPhysicalAddress(),range->getLength(),kIODirectionInOut);
    if(!own)return nullptr;
    IOMemoryMap *map=own->map(kIOMapAnywhere|kIOMapWriteCombineCache);
    own->release();
    if(map && (!map->getVirtualAddress() || (map->getMapOptions()&kIOMapCacheMask)!=kIOMapWriteCombineCache))OSSafeReleaseNULL(map);
    return map;
}
