#pragma once 

namespace PyramidUtils {
    class Serialize {
    public:
        static inline const auto UtilRecord = _byteswap_ulong('PUTI');

        static void Save(SKSE::SerializationInterface* a_intfc);
		static void Load(SKSE::SerializationInterface* a_intfc);
		static void Revert(SKSE::SerializationInterface* a_intfc);
    };
}