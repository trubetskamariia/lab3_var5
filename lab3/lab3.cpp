// MSVC, C++20 
#include <chrono>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <latch>
#include <syncstream>
#include <thread>

using namespace std;
using namespace std::chrono;

latch la(4), lb(5), lc(9), ld(9), le(4), lf(4), lg(4), lh(9), li(4), lj(6), lk(2), ln(3);

void f(char x, int i) {
    this_thread::sleep_for(1s);
    osyncstream(cout) << "Action " << i << " of set " << x << " completed.\n";
}

void act(char x, int from, int to, latch& own, initializer_list<latch*> deps) {
    for (int i = from; i <= to; ++i) {
        for (latch* d : deps) d->wait();
        f(x, i);
        own.count_down();
    }
}

void t1() {
    act('a', 1, 1, la, {});
    act('c', 2, 2, lc, {});
    act('c', 6, 6, lc, {});
    act('e', 3, 3, le, {});
    act('b', 4, 4, lb, {});
    act('h', 1, 7, lh, { &le, &lb });
    act('k', 1, 1, lk, { &ld, &lh, &lj });
}

void t2() {
    act('a', 2, 2, la, {});
    act('c', 3, 3, lc, {});
    act('c', 7, 7, lc, {});
    act('e', 4, 4, le, {});
    act('b', 5, 5, lb, {});
    act('h', 8, 9, lh, { &le, &lb });
    act('j', 1, 5, lj, { &lc });
    act('k', 2, 2, lk, { &ld, &lh, &lj });
}

void t3() {
    act('a', 3, 3, la, {});
    act('c', 4, 4, lc, {});
    act('c', 8, 8, lc, {});
    act('b', 1, 1, lb, {});
    act('d', 1, 1, ld, { &la });
    act('j', 6, 6, lj, { &lc });
    act('d', 4, 9, ld, { &la });
    act('n', 1, 1, ln, { &lg, &li, &lf });
}

void t4() {
    act('a', 4, 4, la, {});
    act('c', 5, 5, lc, {});
    act('c', 9, 9, lc, {});
    act('b', 2, 2, lb, {});
    act('d', 2, 2, ld, { &la });
    act('g', 1, 4, lg, { &la });
    act('i', 1, 2, li, { &le, &lb });
    act('n', 2, 2, ln, { &lg, &li, &lf });
}

void t5() {
    act('c', 1, 1, lc, {});
    act('e', 1, 1, le, {});
    act('e', 2, 2, le, {});
    act('b', 3, 3, lb, {});
    act('d', 3, 3, ld, { &la });
    act('i', 3, 4, li, { &le, &lb });
    act('f', 1, 4, lf, { &lc });
    act('n', 3, 3, ln, { &lg, &li, &lf });
}

int main() {
    osyncstream(cout) << "Computation started.\n";
    auto start = steady_clock::now();
    {
        jthread th1(t1), th2(t2), th3(t3), th4(t4), th5(t5);
    }
    duration<double> dur = steady_clock::now() - start;
    osyncstream(cout) << "Computation finished. Computation time: "
        << fixed << setprecision(2) << dur.count() << " seconds.\n";
}