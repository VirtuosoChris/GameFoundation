#pragma once

namespace Virtuoso
{
    namespace GameFoundations
    {
        /// Object Pool (Fixed Size)
        /// Has the following properties
        /// Some collection of objects of the type, initialized to some "initialized" state when the pool is created
        /// acquire moves objects OUT of the pool (leaving them in an uninitialized state) and release returns them INTO the pool
        /// This is useful for eg. particles or rockets or other things where you want to pay the initialization cost once (eg for PhysicsActors)
        /// and repurpose them as objects are created or destroyed
        /// Where you also have a known maximum quantity you will want
        template<typename T, size_t PoolSize>
        class ObjectPool
        {
        public:
            ObjectPool()
                : m_nextAvailable(0)
            {
                static_assert(std::is_move_constructible_v<T>, "T must be move-constructible");
            }

            bool acquire(T& outObj)
            {
                if (m_nextAvailable < PoolSize)
                {
                    outObj = std::move(m_pool[m_nextAvailable++]);
                    return true;
                }

                return false;
            }

            void release(T&& obj)
            {
                assert(m_nextAvailable > 0);
                m_pool[--m_nextAvailable] = std::move(obj);
            }

            //private:
            std::array<T, PoolSize> m_pool;
            size_t m_nextAvailable;
        };

    }
}