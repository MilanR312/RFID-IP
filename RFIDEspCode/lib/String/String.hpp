
#include "Array.hpp"
//doesnt work kinda
template<std::size_t N>
class String: public Array<char , N>{

    public:
    String(const char (&arr)[N+1]){
        int index = 0;
        while(arr[index] != 0){
            (*this)[index] = arr[index];
            index++;
        }
    }

    template<std::size_t T>
    String<T> operator=(const char (&arr)[T]){
        int index = 0;
        while(arr[index] != 0){
            (*this)[index] = arr[index];
            index++;
        }
    }

    template<std::size_t L>
    bool contains(const String<L> & toCheck){
        if (L > N) return false;
        int correctAmount = 0;
        for(int i = 0; i < N; i++){
            if ((*this)[i] != toCheck[0]) continue;
            for (int j = 1; j < L; j++){
                i++;
                if ((*this)[i] != toCheck[j]) break;
                correctAmount++;
            }
            if (correctAmount == L) return true;
            correctAmount = 0;
        }
        return false;
    }
};