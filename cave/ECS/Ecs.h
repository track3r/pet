#pragma once

template<size_t indexBits, size_t checkBits, int tag>
struct Id
{
    Id(int32_t index, int32_t check)
    :index(index)
    ,check(check)
    {
    }
    
    Id()
    :index(-1)
    ,check(-1)
    {
    }
    
    Id(const Id& other)
    :index(other.index)
    ,check(other.check)
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
    
    constexpr static int thisIndexBits = indexBits;
    constexpr static int thisCheckBits = checkBits;
    
    int32_t index : indexBits;
    int32_t check : checkBits;
};

template<class Id>
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
        int32_t check : Id::thisCheckBits;
        int32_t nextFree: Id::thisIndexBits;
        int32_t dataIndex;
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
    ,_dataToIndex(n)
    ,_size(0)
    ,_capacity(n)
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
    
    bool has(Id id) const
    {
        return lookup(id) != -1;
    }
    
    int lookup(Id id) const
    {
        //#ifdef check indices
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
    
    Id reverseLookup(int pos)
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

using std::swap;

template <class T>
class Data
{
public:
    std::vector<T> _data;
    int _size = 0;
    
public:
    Data(int n)
    :_data(n)
    {
    }
    
    void remove(int i)
    {
        assert(i > 0 && i < _data.size());
        if (i < _size - 1)
        {
            ::swap(_data[i], _data[_size - 1]);
        }
        
        _size--;
    }
    
    void add(const T& t)
    {
        _data[_size++] = t;
        //_data.push_back(t);
    }
    
    T* getPointer(int i)
    {
        assert(i >= 0 && i < _size);
        return &_data[i];
    }
    
    const T* getPointer(int i) const
    {
        return &_data[i];
    }
    
    void swap(int a, int b)
    {
        ::swap(_data[a], _data[b]);
    }
};

template <class T, class Id>
class DataWriter
{
public:
    const PackedArrayIndex<Id>& _index;
    Data<T>& _data;
    
    DataWriter(Data<T>& data, const PackedArrayIndex<Id>& index)
    :_index(index)
    ,_data(data)
    {
        
    }
public:
    void write(Id destination, const T& data)
    {
        assert(destination != invalid<Id>());
        int idx = _index.lookup(destination);
        assert(idx != -1);
        memcpy(_data.getPointer(idx), &data, sizeof(T));
    }
};

template <class T, class Id>
DataWriter<T, Id> make_dataWriter(Data<T>& data, const PackedArrayIndex<Id>& index)
{
    return DataWriter<T, Id>(data, index);
}


typedef Id<20, 12, 0> ComponentId;
typedef Id<20, 12, -1> EntityId;

class System
{
public:
    virtual void update() = 0;
    virtual void removeComponent(ComponentId id) = 0;
    virtual ComponentId createComponent() = 0;
    //virtual void debugComponent(ComponentId id, Writer* out) = 0;
};

struct Vec4
{
    float x,y,z;
};

