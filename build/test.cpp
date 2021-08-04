#include <vector>
#include <string>
#include <assert.h>
#include <iostream> 
#include <fstream> 
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <limits>

int main()
{
    std::ifstream file;
    file.open("djikstra_times.txt");

    int n;
    int total = 0;
    int max_val = 0;
    while (file >> n)
    {
        total += n;
        max_val = std::max(max_val, n);
    }

    std::cout << "Djikstra Total: " << total << std::endl;
    std::cout << "Max: " << max_val << std::endl;

    file.close();

    file.open("hub_times.txt");

    total = 0;
    max_val = 0;
    while (file >> n)
    {
        total += n;
        max_val = std::max(max_val, n);
    }

    std::cout << "Hub Total: " << total << std::endl;
    std::cout << "Max: " << max_val << std::endl;
    file.close();
}