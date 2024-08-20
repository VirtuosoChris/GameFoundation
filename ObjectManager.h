#pragma once

namespace Virtuoso
{
    namespace GameFoundations
    {
        template <typename ObjectType,
            template <typename, typename = std::allocator<ObjectType>> class ObjectContainer = std::deque
        >
        class ObjectManager
        {
            struct VersionRecord
            {
                std::uint8_t version=0;
                bool valid = false;

                bool operator==(const std::uint8_t& v)
                {
                    return valid && (v == version);
                }
            };

            ObjectContainer<ObjectType> objects;
            std::vector<VersionRecord> versions;
            std::stack<uint32_t> freeIndices;

        public:

            std::size_t size() { return objects.size() - freeIndices.size(); }

            void clear()
            {
                objects.clear();
                versions.clear();
                freeIndices = std::stack<uint32_t>();
            }

            struct Handle
            {
                union
                {
                    struct
                    {
                        int32_t index : 24;
                        uint32_t version : 8;
                    };
                    uint32_t value;
                };

                operator uint32_t() const
                {
                    return value;
                }

                Handle(uint32_t v)
                {
                    value = v;
                }

                Handle(int32_t idx, uint32_t ver) : index(idx), version(ver)
                {
                }
            };

            Handle insertObject(ObjectType&& object)
            {
                if (freeIndices.empty())
                {
                    objects.push_back(std::move(object));
                    versions.push_back({ 1,true }); // start with version "1", otherwise the handle for version 0, index 0 just looks like null
                    return { (int32_t)(objects.size() - 1), 1u };
                }
                else
                {
                    uint32_t index = freeIndices.top();
                    freeIndices.pop();
                    objects[index] = std::move(object);
                    versions[index].version++;
                    versions[index].valid = true;
                    return { (int32_t)(index), versions[index].version };
                }
            }

            ObjectType* lookupObject(const Handle& handle)
            {
                if (handle.index < objects.size() && versions[handle.index] == handle.version)
                {
                    return &objects[handle.index];
                }

                return nullptr; // Handle is invalid or object was deleted
            }

            bool removeObject(const Handle& handle)
            {
                if (handle.index < objects.size() && versions[handle.index] == handle.version)
                {
                    versions[handle.index].valid = false;

                    freeIndices.push(handle.index);
                    return true;
                }

                return false;
            }

            class Iterator
            {
            private:
                ObjectManager<ObjectType, ObjectContainer>* manager;
                size_t current;

                void skipInvalid()
                {
                    while (current < manager->objects.size() && (!manager->versions[current].valid))
                    {
                        ++current;
                    }
                }

            public:
                Iterator(ObjectManager<ObjectType, ObjectContainer>* manager, size_t start) : manager(manager), current(start)
                {
                    skipInvalid();
                }

                Iterator& operator++()
                {
                    if (current < manager->objects.size())
                    {
                        ++current;
                        skipInvalid();
                    }

                    return *this;
                }

                bool operator!=(const Iterator& other) const
                {
                    return current != other.current;
                }

                ObjectType& operator*()
                {
                    return manager->objects[current];
                }
            };

            Iterator begin()
            {
                return Iterator(this, 0);
            }

            Iterator end()
            {
                return Iterator(this, objects.size());
            }
        };
    }
}

