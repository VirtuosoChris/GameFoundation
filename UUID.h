#pragma once
#include <Windows.h>
#include <rpcdce.h>
#include <cassert>
#include <cstring>
#include <nlohmann/json.hpp>

namespace Virtuoso
{
    namespace GameFoundations
    {
        union UUID
        {
            std::pair < std::uint64_t, std::uint64_t> dwords;
            ::UUID uuid;

            UUID();

            UUID(std::uint64_t a, std::uint64_t b) : dwords({a,b}) { }

            std::size_t uniqueHash() const;

            UUID& operator=(const UUID& a) { dwords = a.dwords; return *this; }

            inline bool operator<(const Virtuoso::GameFoundations::UUID& a) const
            {
                return uniqueHash() < a.uniqueHash();
            }

            //bool operator==(const UUID& a) { return dwords == a.dwords; }

            NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(UUID, dwords);
        };
    }
}

namespace std
{
    template <>
    struct hash<Virtuoso::GameFoundations::UUID>
    {
        size_t operator()(const virtuoso::UUID& gc) const
        {
            return gc.uniqueHash();
        }
    };
}

inline bool operator==(const Virtuoso::GameFoundations::& a, const Virtuoso::GameFoundations::& b)
{
    return a.uniqueHash() == b.uniqueHash();
}

namespace Virtuoso
{
    namespace GameFoundations
    {
        inline UUID::UUID()
        {
            RPC_STATUS result = UuidCreate(&this->uuid);

            switch (result)
            {
            case RPC_S_OK:
                break;
            case RPC_S_UUID_LOCAL_ONLY:
                assert(0);
                break;
            case RPC_S_UUID_NO_ADDRESS:
                assert(0);
                break;
            }
        }

        inline std::uint64_t UUID::uniqueHash() const { return dwords.first ^ dwords.second; }
    }
}
