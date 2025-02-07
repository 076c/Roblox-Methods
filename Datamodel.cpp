#include <Windows.h>
#include <iostream>
#include <vector>
#define REBASE(x) (x + (DWORD)GetModuleHandleA(nullptr))

namespace Roblox {
    uintptr_t Datamodel;
    std::vector<uintptr_t> Jobs;
}

namespace Offsets {
    const uintptr_t vm__fake_datamodel = 0x5C41ED0;
    const uintptr_t vm__get_scheduler = 0x2DF8100;

    const uintptr_t JobsName = 0x90;
    const uintptr_t JobsStart = 0x1c8;
    const uintptr_t JobsEnd = 0x1d0;

    const uintptr_t ScriptContext = 0x1f8;
    const uintptr_t Parent = 0x50;
}

using GetScheduler_t = uintptr_t(__fastcall*)();

void Scheduler()
{
    const GetScheduler_t GetScheduler = reinterpret_cast<GetScheduler_t>(REBASE(Offsets::vm__get_scheduler));
    const uintptr_t Scheduler = GetScheduler();

    std::vector<uintptr_t> Jobs;
    for (uintptr_t Job = Scheduler + Offsets::JobsStart; Job < Scheduler + Offsets::JobsEnd; Job += 0x10)
        Jobs.push_back(Job);
    Roblox::Jobs = Jobs;
}

std::vector<uintptr_t> GetJobs()
{
    Scheduler();

    return Roblox::Jobs;
}

uintptr_t GetJobByName(const std::string Name)
{
    for (const uintptr_t Job : GetJobs())
        if (Name.c_str() == (const char*)Job + Offsets::JobsName)
            return Job;
}

uintptr_t GetDatamodelByJob() /* Method 1: WaitingHybridScriptsJob */
{
    const uintptr_t WaitingHybridScriptsJob = GetJobByName("WaitingHybridScriptsJob");
    const uintptr_t script_context = WaitingHybridScriptsJob + Offsets::ScriptContext;

    const uintptr_t DataModel = script_context + Offsets::Parent;

    Roblox::Datamodel = DataModel;

    return DataModel;
}

uintptr_t GetDatamodelByDeleter() /* Method 2: Get by Fake DataModel */
{
    const uintptr_t FakeDataModel = reinterpret_cast<uintptr_t>(REBASE(Offsets::vm__fake_datamodel) + 0x8);
    const uintptr_t RealDataModel = reinterpret_cast<uintptr_t>(FakeDataModel + 0x1a8);

    Roblox::Datamodel = RealDataModel;

    return RealDataModel;
}

uintptr_t GetDatamodelByRenderJob()
{
    const uintptr_t RenderJob = GetJobByName("RenderJob"); /* Render Job for Roblox */
    const uintptr_t ptr1 = reinterpret_cast<uintptr_t>(RenderJob + 0xb0);
    const uintptr_t datamodel = reinterpret_cast<uintptr_t>(ptr1 + 0x1a8);

    Roblox::Datamodel = datamodel;
    return datamodel;
}
