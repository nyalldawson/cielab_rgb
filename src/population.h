#ifndef POPULATION_H
#define POPULATION_H

#include "convert.h"
#include <random>

class God;

class Individual
{
  public:

    Constants constants;

    void calcDelta( const std::vector< ColorMap > &map );

    unsigned long long  delta = std::numeric_limits<unsigned long long >::max();


    void dump();

};

class God
{
  public:

    God();

    Individual createIndividual();

    void mutate( Individual &individual, int index );

    Individual breed( const Individual &mum, const Individual &dad );

    std::random_device rd;
    std::mt19937 mt{ rd() };

    std::vector< std::uniform_real_distribution<double> > constantDist;
    std::uniform_int_distribution<int> breedDist{ 0, 1};

};

class Population
{
  public:
    Population( God &god, std::size_t count = 1000,
                std::size_t bestSample = 50, std::size_t randomSample = 5 );

    void calcDeltas( const std::vector< ColorMap > &map );

    void select();
    void sortByDelta();


    God &god;
    std::vector< Individual > individuals;

    std::size_t count;
    std::size_t bestSample;
    std::size_t randomSample;

    std::random_device rd;
    std::mt19937 mt{ rd() };
    std::uniform_int_distribution<std::size_t> randomDist;
    std::uniform_real_distribution<double> mutateChanceDist{ 0, 1 };
    std::uniform_int_distribution<std::size_t> parentDist;
    std::uniform_int_distribution<int> mutateGene{ 0, Constants::len - 1};


};

#endif // POPULATION_H
