#pragma once
#include <deque>

namespace Virtuoso
{
    namespace GameFoundations
    {

        /// Object Pool (Dynamic)
        /// Has the following properties
        /// Some collection of objects of the type, initialized to some "initialized" state when the pool is created or expanded
        /// acquire moves objects OUT of the pool (leaving them in an uninitialized state) and release returns them INTO the pool
        /// Since this is the dynamic version of the pool, you can tell it an expansion size when it runs out of objects or give it a reserve size
        /// This is useful for eg. particles or rockets or other things where you want to pay the initialization cost once (eg for PhysicsActors)
        /// and repurpose them as objects are created or destroyed
        template<typename T, typename Initializer, std::size_t ExpansionSize = 1>
        class ObjectPoolDynamic
        {
        public:

            ObjectPoolDynamic()
            {
            }

            ObjectPoolDynamic(const Initializer& initFunctor, int reserveSize = 0)
                : init(initFunctor)
            {
                static_assert(std::is_move_constructible_v<T>, "T must be move-constructible");

                if (reserveSize)
                {
                    reserve(reserveSize);
                }
            }

            bool acquire(T& outObj)
            {
                if (m_nextAvailable >= size())
                {
                    expandPool();
                    assert(m_nextAvailable < size());
                }

                outObj = std::move(m_pool[m_nextAvailable++]);
                return true;
            }

            void release(T&& obj)
            {
                assert(m_nextAvailable > 0);
                m_pool[--m_nextAvailable] = std::move(obj);
            }

            inline void reserve(int reserveSize)
            {
                while (m_pool.size() < reserveSize)
                {
                    expandPool();
                }
            }

            inline int size() const
            {
                return m_pool.size();
            }

            //private:
            std::deque<T> m_pool;
            size_t m_nextAvailable=0;
            Initializer init;

        private:

            inline void expandPool()
            {
                for (std::size_t i = 0; i < ExpansionSize; ++i)
                {
                    m_pool.push_back(std::move(init()));
                }
            }
        };

    }
}

