#include "MemoryUsageHelper.h"
#include "CorePlatform/LogHelper.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include <mach/mach.h>

static vm_size_t GetUsedMemory() 
{
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, 
            (task_info_t)&info, &size);
    return (kerr == KERN_SUCCESS) ? info.resident_size : 0; // size in bytes 
}

static vm_size_t GetFreeMemory()
{
    mach_port_t host_port = mach_host_self();
    mach_msg_type_number_t host_size 
            = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    vm_size_t pagesize;  
    vm_statistics_data_t vm_stat;  
    host_page_size(host_port, &pagesize); 
    (void) host_statistics(host_port, HOST_VM_INFO, 
            (host_info_t)&vm_stat, &host_size);
    return vm_stat.free_count * pagesize;
}

void MemoryUsageHelper::LogMemoryUsage()
{
    // compute memory usage and log if different by >= 100k
    static long prevMemUsage = 0;
    long curMemUsage = GetUsedMemory();
    long memUsageDiff = curMemUsage - prevMemUsage;
    //if (memUsageDiff > 100000 || memUsageDiff < -100000) 
    {
        prevMemUsage = curMemUsage;
        string msg = StringHelper::format("Memory used %7.1f (%+5.0f), free %7.1f kb",
                curMemUsage/1024.0f, memUsageDiff/1024.0f, GetFreeMemory()/1024.0f);
        MY_LOG_WARNING_F(msg.c_str());
    }
}

#endif
