#include <napa/module-internal.h>
#include <napa/thread-local-storage.h>

#include <napa-log.h>

using namespace napa;
using namespace napa::module;

// Global instance of IsolateData.
IsolateData& GetIsolateData() {
    static IsolateData isolateData;
    return isolateData;
}

IsolateData::IsolateData() {
    for (auto& tlsIndex : _tlsIndexes) {
        tlsIndex = tls::Alloc();
    }
}

void* IsolateData::Get(IsolateDataId isolateDataId) {
    auto slotId = static_cast<size_t>(isolateDataId);
    NAPA_ASSERT(slotId < GetIsolateData()._tlsIndexes.size(), "slot id out of range");

    auto tlsIndex = GetIsolateData()._tlsIndexes[static_cast<size_t>(IsolateDataId::ISOLATE)];
    auto isolate = v8::Isolate::GetCurrent();
    auto current = static_cast<v8::Isolate*>(tls::GetValue(tlsIndex));
    if (current != isolate) {
        return nullptr;
    }

    tlsIndex = GetIsolateData()._tlsIndexes[slotId];
    return tls::GetValue(tlsIndex);
}

void IsolateData::Set(IsolateDataId isolateDataId,
                      void* data) {
    auto slotId = static_cast<size_t>(isolateDataId);
    NAPA_ASSERT(slotId < GetIsolateData()._tlsIndexes.size(), "slot id out of range");

    auto tlsIndex = GetIsolateData()._tlsIndexes[static_cast<size_t>(IsolateDataId::ISOLATE)];
    auto isolate = v8::Isolate::GetCurrent();
    auto current = static_cast<v8::Isolate*>(tls::GetValue(tlsIndex));
    if (current != isolate) {
        tls::SetValue(tlsIndex, isolate);
    }

    tlsIndex = GetIsolateData()._tlsIndexes[slotId];
    tls::SetValue(tlsIndex, data);
}