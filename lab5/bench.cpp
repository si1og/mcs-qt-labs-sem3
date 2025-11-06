#include <iostream>
#include <vector>
#include <list>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

template<typename Func>
double measureTime(Func func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    duration<double> diff = end - start;
    return diff.count();
}

int main() {
    const int N = 100'000'00;

    cout << "  Бенчмарк: list vs vector на " << N << " элементов\n";

    cout << "1. vector::push_back() (добавление в конец)\n";
    cout << "   ";

    double vectorTime = measureTime([N]() {
        vector<int> v;
        v.reserve(N);
        for (int i = 0; i < N; i++) {
            v.push_back(i);
        }
    });

    cout << "Время: " << fixed << setprecision(3) << vectorTime << " сек\n\n";


    cout << "2. list::push_back() (добавление в конец)\n";
    cout << "   ";

    double listTime = measureTime([N]() {
        list<int> l;
        for (int i = 0; i < N; i++) {
            l.push_back(i);
        }
    });

    cout << "Время: " << fixed << setprecision(3) << listTime << " сек\n\n";

    return 0;
}
