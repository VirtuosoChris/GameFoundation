#pragma once

template <typename T, std::size_t Capacity>
class RingBuffer
{
    std::size_t count = 0;
    std::size_t cursor = 0;
    using Base = std::array<T, Capacity>;

    Base buffer;

    std::size_t wraparound_negative(int x) const
    {
        assert(x < 0);
        return  (((-x) + 1) * Capacity + x) % Capacity;
    }

    std::size_t wraparound(int x) const
    {
        return x < 0 ? wraparound_negative(x) : (x % Capacity);
    }

    std::size_t index_of_element(std::size_t i) const
    {
        return wraparound(int(cursor) - int(count) + (int)(i));
    }

    std::size_t start_index() const
    {
        return wraparound(int(cursor) - int(count));
    }

    std::size_t last_index() const
    {
        return wraparound(cursor - 1);
    }

public:

    struct iterator
    {
        std::size_t index = 0;
        RingBuffer<T, Capacity>& parent;

        iterator(RingBuffer<T, Capacity>& p, std::size_t idx = 0) : parent(p), index(idx) {}

        iterator operator++(int)
        {
            assert(index < parent.size());
            auto temp = *this;
            this->operator++();
            return temp;
        }

        iterator operator++()
        {
            assert(index < parent.size());
            index++;
            return *this;
        }

        iterator operator--(int)
        {
            assert(index > 0);
            auto temp = *this;
            this->operator--();
            return temp;
        }

        iterator operator--()
        {
            assert(index > 0);
            index--;
            return *this;
        }

        T& operator*()
        {
            return parent[parent.index_of_element(index)];
        }

        const T& operator*() const
        {
            return parent[parent.index_of_element(index)];
        }

        bool operator==(const iterator& other) const
        {
            return index == other.index;
        }

        bool operator!=(const iterator& other) const
        {
            return index != other.index;
        }
    };

    void push_back(const T& val)
    {
        buffer[cursor] = val;
        if (count < Capacity) count++;
        cursor = (cursor + 1) % Capacity;
    }

    void push_back(T&& val)
    {
        buffer[cursor] = std::move(val);
        if (count < Capacity) count++;
        cursor = (cursor + 1) % Capacity;
    }

    void pop_back()
    {
        assert(!empty());
        cursor = wraparound(cursor - 1);
        count--;
    }

    T& front()
    {
        assert(!empty());
        return buffer[start_index()];
    }

    T& back() {
        assert(!empty());
        return buffer[last_index()];
    }

    const T& front() const
    {
        assert(!empty());
        return buffer[start_index()];
    }

    const T& back() const
    {
        assert(!empty());
        return buffer[last_index()];
    }

    T& operator[](const std::size_t& i)
    {
        assert(i >= 0);
        assert(i < count);
        return buffer[index_of_element(i)];
    }

    const T& operator[](const std::size_t& i) const
    {
        assert(i >= 0);
        assert(i < count);
        return buffer[index_of_element(i)];
    }

    std::size_t size() const
    {
        return count;
    }

    bool full() const
    {
        return count == Capacity;
    }

    bool empty() const
    {
        return count == 0;
    }

    void clear()
    {
        count = 0;
        cursor = 0;
    }

    std::size_t capacity() const
    {
        return Capacity;
    }

    iterator begin()
    {
        return iterator(*this, 0);
    }

    iterator end()
    {
        return iterator(*this, count);
    }
};

