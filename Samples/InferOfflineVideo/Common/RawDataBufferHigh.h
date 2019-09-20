
#ifndef RawDataBuffer_H_
#define RawDataBuffer_H_

#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/data_type.h"
#include <memory>

struct RawDataBufferHigh {
    std::shared_ptr<uint8_t> data;
    uint32_t len_of_byte; // buffer size
};

template<class Archive>
void serialize(Archive& ar, RawDataBufferHigh& data) {
    ar(data.len_of_byte);
    if (data.len_of_byte > 0 && data.data.get() == nullptr) {
        HIAI_SHARED_PTR_RESET(data.data, new(std::nothrow) uint8_t[data.len_of_byte],\
                "serialize RawDataBuffer reset fail");
    }
    ar(cereal::binary_data(data.data.get(), data.len_of_byte *
                                            sizeof(uint8_t)));
}

inline void GetTransSearPtr(void* dataPtr, std::string& structStr, uint8_t*& buffer, uint32_t& bufferSize)
{
    RawDataBufferHigh* engineTrans = (RawDataBufferHigh*)dataPtr;
    structStr = std::string((const char*)dataPtr, sizeof(RawDataBufferHigh));
    buffer = (uint8_t*)engineTrans->data.get();
    bufferSize = engineTrans->len_of_byte;
}

inline std::shared_ptr<void> GetTransDearPtr(const char* ctrlPtr, const uint32_t& ctrlLen, const unsigned char* dataPtr, const uint32_t& dataLen)
{
    std::shared_ptr<RawDataBufferHigh> engineTrans = std::make_shared<RawDataBufferHigh>();
    engineTrans->len_of_byte = ((RawDataBufferHigh*)ctrlPtr)->len_of_byte;
    engineTrans->data.reset((uint8_t*)dataPtr, hiai::Graph::ReleaseDataBuffer);
    return std::static_pointer_cast<void>(engineTrans);
}

#if 0

HIAI_REGISTER_DATA_TYPE("RawDataBufferHigh", RawDataBufferHigh);

#else

HIAI_REGISTER_SERIALIZE_FUNC("RawDataBufferHigh", RawDataBufferHigh, GetTransSearPtr, GetTransDearPtr);

#endif

#endif