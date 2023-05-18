#include <iostream>
#include <bitset>
using namespace std;
int main()
{
    char c = 'H';
    std::string binary = std::bitset<8>(c).to_string(); // to binary
    std::cout << binary << "\n";
    unsigned long decimal = std::bitset<8>(binary).to_ulong();
    std::cout << decimal << "\n";
    return 0;
}