#pragma once

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

template <typename T, std::size_t LEN, std::size_t IDX = LEN - 1>
struct StdArrayHash
{
    std::size_t operator()(const std::array<T, LEN>& v) const noexcept
    {
        std::hash<T> hasher;
        std::size_t h = hasher(v[IDX]);

        StdArrayHash<T, LEN, IDX - 1> h2;

        hash_combine(h, h2(v));

        return h;
    }
};

template<typename T, std::size_t LEN>
struct StdArrayHash<T, LEN, 0u>
{
    std::size_t operator()(const std::array<T, LEN>& v) const noexcept
    {
        std::hash<T> hasher;
        std::size_t h = hasher(v[0]);
        return h;
    }
};

template<class T, size_t N>
struct std::hash<std::array<T, N>>
{
    std::size_t operator() (const std::array<T, N>& key) const
    {
        constexpr bool unroll = N > 16;
        if (unroll)
        {
            StdArrayHash<T, N> h;
            return h(key);
        }
        else
        {
            std::hash<T> hasher;
            std::size_t result = hasher(key[0]);
            for (std::size_t i = 1; i < N; i++)
            {
                hash_combine(result, hasher(key[i]));
            }
            return result;
        }
    }
};

inline std::size_t hash_string_view(const std::string_view& str)
{
    std::hash<char> hasher;
    std::size_t result = 0;

    for (char c : str) {
        hash_combine(result, hasher(c));
    }

    return result;
}