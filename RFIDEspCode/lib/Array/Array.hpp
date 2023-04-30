#include <cstddef>

template<typename T, std::size_t N>
class Array{
    T data[N];

    using iterator = T*;

    public:

    T &         operator[](size_t index)        {return data[index];}
    const T &   operator[](size_t index) const  {return data[index];}

    iterator        begin()         {return data;}
    const iterator  begin() const   {return data;}
    iterator        end()           {return data+N;}
    const iterator  end() const     {return data+N;}

    bool contains(const T & ell){
        for (const T & value: *this){
            if (value == ell) return true;
        }
        return false;
    }

    const size_t length() const {return N;}

    T * pdata() { return data; }
};