#pragma once

#include "pch.h"

namespace ecs3
{
    struct Id
    {
        Id(int index, int check)
            :index(index)
            , check(check)
        {
        }

        Id()
            :index(-1)
            , check(-1)
        {
        }

        Id(const Id& other)
            :index(other.index)
            , check(other.check)
        {
        }

        /*void debugString(Writer* out)
        {
        char tmp[32];
        sprintf(tmp, "(%i, %i)", (int) index, (int) check);
        out->write(tmp);
        }*/

        bool isValid() const
        {
            return index != -1 && check != -1;
        }

        bool operator==(const Id& other) const
        {
            bool same = index == other.index;
            if (same)
            {
                assert(check == other.check);
            }

            return same;
        }

        bool operator!=(const Id& other) const
        {
            return !(*this == other);
        }

        int index : 32;
        int check : 8;
    };

    const Id& invalid()
    {
        static const Id nullId(-1, -1);
        return nullId;
    }

    template <class Id>
    class PackedArrayIndex
    {
    public:
        struct IndexElement
        {
            int check : 8;
            int nextFree;
            int dataIndex;
        };

        std::vector<IndexElement> _index;
        std::vector<int> _dataToIndex;
        size_t _size;
        size_t _capacity;

        int32_t _firstFree;
        int32_t _lastFree;

    public:
        PackedArrayIndex(int n)
            :_index(n)
            , _dataToIndex(n)
            , _size(0)
            , _capacity(n)
        {
            static int nextInitCheck = 0;
            for (int i = 0; i < n; i++)
            {
                _index[i].check = nextInitCheck++;
                //_index[i].check = 999;
                _index[i].nextFree = i + 1;
                _index[i].dataIndex = -1;

                _dataToIndex[i] = -1;
            }

            _index[n - 1].nextFree = -1;
            _firstFree = 0;
            _lastFree = n - 1;
        }

        int getFreeElement()
        {
            if (_firstFree == -1)
            {
                return -1;
            }

            IndexElement& elem = _index[_firstFree];
            int ret = _firstFree;
            if (_firstFree == _lastFree)
            {
                _lastFree = -1;
            }
            _firstFree = elem.nextFree;
            return ret;
        }

        //TODO more clever arangement
        void putFreeElement(int index)
        {
            _index[index].nextFree = -1;

            if (_firstFree == -1)
            {
                _firstFree = index;
                _lastFree = index;
                return;
            }

            _index[_lastFree].nextFree = index;
            _lastFree = index;
        }

        int lookup(Id id) const
        {
            if (id.index >= _index.size())
            {
                return -1;
            }

            auto& elem = _index[id.index];

            assert(elem.check == id.check);

            if (elem.check != id.check)
            {
                return -1;
            }

            return elem.dataIndex;
        }

        Id reverseLookup(int pos) const
        {
            assert(pos >= 0 && pos <= _dataToIndex.size());
            const auto index = _dataToIndex[pos];
            if (index == -1)
            {
                return invalid<Id>();
            }

            const auto& elem = _index[index];
            return Id(index, elem.check);
        }

        const Id create()
        {
            const int dataIndex = (int)_size;

            const int elem = getFreeElement();
            if (elem == -1)
            {
                return invalid<Id>();
            }

            _index[elem].dataIndex = dataIndex;
            _size++;

            _dataToIndex[dataIndex] = elem;
            return Id(elem, _index[elem].check);
        }

        int remove(Id id)
        {
            assert(_index[id.index].check == id.check);

            _index[id.index].check++;
            putFreeElement(id.index);
            _size--;
            return _index[id.index].dataIndex;
        }

        void swapData(int a, int b)
        {
            if (a == b)
            {
                return;
            }

            const int indexA = _dataToIndex[a];
            const int indexB = _dataToIndex[b];
            int tmpA = _index[indexA].dataIndex;
            _index[indexA].dataIndex = _index[indexB].dataIndex;
            _index[indexB].dataIndex = tmpA;

            tmpA = _dataToIndex[a];
            _dataToIndex[a] = _dataToIndex[b];
            _dataToIndex[b] = tmpA;
        }

        int swapToEndAndRemove(Id id)
        {
            assert(_index[id.index].check == id.check);
            int dataIndex = _index[id.index].dataIndex;
            swapData(_index[id.index].dataIndex, (int)_size - 1);
            remove(id);
            return dataIndex;
        }

        Id getIdFromDataIndex(int dataIndex)
        {
            assert(dataIndex > 0 && dataIndex < _dataToIndex.size());
            int index = _dataToIndex[dataIndex];
            return Id(index, _index[index].check);
        }
    };

    class Data
    {
    public:
        uint8_t* _data;
        int _size = 0;
        int _elements = 0;
        int _elementSize = 0;

    public:
        Data()
        {
        }

        void init(int elementSize)
        {
            _elementSize = elementSize;
            _elements = 0;
            _size = elementSize * 1024;
            _data = (uint8_t*)malloc(_size);
        }

        void remove(int i)
        {
            assert(i > 0 && i < _elements);
            if (i < _elements - 1)
            {
                swap(i, _elements - 1);
            }

            _elements--;
        }

        void add(uint8_t* data)
        {
            assert((_elements + 1) * _elementSize < _size);
            
            uint8_t* p = _data + _elementSize * _elements;
            memcpy(p, data, _elementSize);
            _elements++;
        }

        uint8_t* getPointer(int i)
        {
            assert(i >= 0 && i < _elements);
            return _data + _elementSize * i;
        }

        void swap(uint8_t* a, uint8_t* b)
        {
            void* temp = alloca(_elementSize);
            memcpy(temp, a, _elementSize);
            memcpy(a, b, _elementSize);
            memcpy(b, a, _elementSize);
        }

        void swap(int a, int b)
        {
            assert(a >= 0 && a < _elements);
            assert(b >= 0 && b < _elements);
            uint8_t* pa = _data + a * _elementSize;
            uint8_t* pb = _data + b * _elementSize;
            swap(pa, pb);
        }
    };
}