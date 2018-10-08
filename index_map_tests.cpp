// <author>Mikhail Semenov</author>
// <date>2015-01-18</date>
// <summary>Contains tests various maps with integer keys</summary>


#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <sstream>
#include <map>
#include "index_maps.h"

//#define USE_BOOST

#ifdef USE_BOOST
#include "boost/container/flat_map.hpp"
#endif



//Testing
#include <chrono>
#include <random>
#include <iomanip>
#include <set>
#include <bitset>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <string>

#include <malloc.h>

std::ptrdiff_t HeapUsed()
{
    _HEAPINFO info = { 0, 0, 0 };
    std::ptrdiff_t used = 0;
    int rc;
    while ((rc = _heapwalk(&info)) == _HEAPOK)
    {
        if (info._useflag == _USEDENTRY)
            used += info._size;
    }
    if (rc != _HEAPEND && rc != _HEAPEMPTY)
        used = (used ? -used : -1);
    return used;
}


using namespace std::chrono;
std::default_random_engine generator;
std::uniform_int_distribution<unsigned> uint_distribution(0, 4294967295);
std::uniform_real_distribution<double> real_distribution(0.0, 1.0);

unsigned random_uint() // between 0 and 4294967295
{ /* Generates numbers between 0 and 1. */
    return uint_distribution(generator);
}

double random_real()
{
    return real_distribution(generator);
}

void reset_random_distribution()
{
    uint_distribution.reset();
    real_distribution.reset();
    generator.seed(0x11111111);
}

typedef std::chrono::high_resolution_clock clk;
typedef duration<double, std::ratio<1, 1000>>    time_in_msec;

const unsigned repeat = 200; // was 200
const unsigned steps = 1000000;// was 1000000
std::ptrdiff_t heap1 = 0;

template<class Map>
void Test_IndexMap(const std::string& testName, unsigned interval_length, const std::vector<std::pair<unsigned, double>>& values, Map& map)
{
    std::cout << "_____________________________________________________" << std::endl;
    std::cout << testName << std::endl;

    clk::time_point t1 = high_resolution_clock::now();

    for (auto x : values)
    {
        map.insert(x);
    }

    clk::time_point t2 = clk::now();
    time_in_msec time_span = duration_cast<time_in_msec>(t2 - t1);

    double timing = time_span.count();
    std::cout << testName << " Generation    took " << time_span.count() << " milliseconds." << std::endl;

    auto heap2 = HeapUsed();
    std::cout << "Memory used: " << (heap2 - heap1) << std::endl;

#if(1)
    reset_random_distribution();

    t1 = high_resolution_clock::now();


    double sum = 0;
    unsigned counter = 0;

    for (unsigned i = 0; i < steps; ++i)
    {
        unsigned k1 = random_uint() % interval_length;
        auto it = map.find(k1);
        if (it != map.end())
        {
            sum += it->second * it->first;
            counter++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << testName << " Random access took " << time_span.count() << " milliseconds; counter " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    std::cout << testName << " Random access (per step) took " << ((time_span.count() / steps)*1e6) << " nanoseconds; counter " << counter << " sum: " << std::setprecision(15) << sum << std::endl;
    reset_random_distribution();
    t1 = high_resolution_clock::now();

    sum = 0;
    unsigned long long counter2 = 0;

    for (unsigned k = 0; k < repeat; k++)
    {
        for (auto x : map)
        {
            double coeff = random_uint();
            sum += x.second * coeff;//f(*it);
            counter2++;
        }
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);
    std::cout << testName << " Scanning      took " << (time_span.count() / repeat) << " milliseconds; counter: " << counter2 << " sum: " << std::setprecision(15) << sum << std::endl;

    //reset_random_distribution();

    t1 = high_resolution_clock::now();

    for (auto x : values)
    {
        map.erase(x.first);
    }

    t2 = clk::now();
    time_span = duration_cast<time_in_msec>(t2 - t1);

    std::cout << testName << " Deletion      took " << time_span.count() << " milliseconds; counter: " << map.size() << std::endl;
    reset_random_distribution();
#endif
}

template<class Map>
void TestMap()
{
    Map x(10);
    x.insert(std::make_pair(2, 3.5));
    //x.insert(std::make_pair(9, 7.0));
    x[9] = 7.0;
    x.insert(std::make_pair(5, 123));
    x.insert(std::make_pair(4, -2.3));
    //x.insert(std::make_pair(7, -2.9));
    x[7] = -2.9;

    try
    {
        double p = x.at(3);
        std::cout << p << std::endl;
    }
    catch (const std::exception& s)
    {
        std::cout << "exception:" << s.what() << std::endl;
    }

    for (auto& p : x)
    {
        std::cout << p.first << " " << p.second << std::endl;
    }

    auto z = x.find(2);

    if (z != x.end())
    {
        std::cout << "z: " << z->first << " " << z->second << std::endl;
    }

    z = x.find(4);

    if (z != x.end())
    {
        std::cout << "z: " << z->first << " " << z->second << std::endl;
    }

    double v = x.at(5);

    std::cout << "z: " << 5 << " " << v << std::endl;

    x.erase(7);
    x.erase(2);
    x.erase(5);

    std::cout << "after erase: 7,2,5" << std::endl;
    for (auto& p : x)
    {
        std::cout << p.first << " " << p.second << std::endl;
    }
    std::cout << "____________________________________________" << std::endl;
}


int main()
{
    {
        unordered_index_map<double> x(10);
        x.insert(std::make_pair(2, 3.5));
        x.insert(std::make_pair(9, 7.0));
        x.insert(std::make_pair(5, 123));
        x.insert(std::make_pair(4, -2.3));
        x.insert(std::make_pair(7, -2.9));

        for (auto& p : x)
        {
            std::cout << p.first << " " << p.second << std::endl;
        }

        x.erase(7);
        x.erase(2);
        x.erase(5);

        std::cout << "after erase: 7,2,5" << std::endl;
        for (auto& p : x)
        {
            std::cout << p.first << " " << p.second << std::endl;
        }
        std::cout << "____________________________________________" << std::endl;
    }

    TestMap<sparse_map<double>>();
    TestMap<unordered_index_map<double>>();
    TestMap<two_level_map<double>>();
    TestMap<flat_map<unsigned, double>>();

    std::pair<unsigned, double> x(5, 6);
    std::pair<unsigned, double> z(2, 7.2);
    std::pair <const unsigned, double> y(z);

    x = *reinterpret_cast<std::pair<unsigned, double>*>(&y);
    // y.first = 3;

    std::cout << x.first << " " << x.second << std::endl;

    
    unsigned interval_length = 10000000;
    unsigned elem_count = 10000;
    std::vector<std::pair<unsigned, double>> values(elem_count);

    reset_random_distribution();
    for (unsigned i = 0; i < elem_count; i++)
    {
        unsigned index = random_uint() % interval_length;
        double v = random_real();
        values[i] = std::make_pair(index, v);
    }

    {
        heap1 = HeapUsed();
        sparse_map<double> map(interval_length);
        Test_IndexMap("sparse_map", interval_length, values, map);
    }

    {
        heap1 = HeapUsed();
        two_level_map<double> map(interval_length);
        Test_IndexMap("two_level_map", interval_length, values, map);
    }

    {
        heap1 = HeapUsed();
        std::map<unsigned, double> map;
        Test_IndexMap("std::map", interval_length, values, map);
    }


    {
        heap1 = HeapUsed();
        std::unordered_map<unsigned, double> map;
        Test_IndexMap("std::unordered_map", interval_length, values, map);
    }


#if(1)
    {
        heap1 = HeapUsed();
        flat_index_map<double> map(interval_length);
        Test_IndexMap("flat_index_map", interval_length, values, map);
    }

#ifdef USE_BOOST
    {
        heap1 = HeapUsed();
        boost::container::flat_map<unsigned, double> map;
        Test_IndexMap("Boost:flat_map", interval_length, values, map);
    }
#endif

    {
        heap1 = HeapUsed();
        flat_map<unsigned, double> map;
        Test_IndexMap("flat_map", interval_length, values, map);
    }    

    {
        heap1 = HeapUsed();
        unordered_index_map<double> map(interval_length);
        Test_IndexMap("unordered_index_map", interval_length, values, map);
    }
#endif
    return 0;
}

