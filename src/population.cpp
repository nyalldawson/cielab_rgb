#include "population.h"
#include <algorithm>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
God::God()
{
  constantDist =
  {
    std::uniform_real_distribution<double>( 500, 500 ), // 500.0
    std::uniform_real_distribution<double>( 0.008856, 0.008856451679035631 ), //ϵ 0.008856
    std::uniform_real_distribution<double>( 903.2962962962963, 903.3 ), // κ 903.3
    std::uniform_real_distribution<double>( 116, 116 ), // 116
    std::uniform_real_distribution<double>( 16, 16 ), // 16
    std::uniform_real_distribution<double>( 200, 200 ), // 200
    std::uniform_real_distribution<double>( 0.2, 1.3 ), // 0.9504 Xr
    std::uniform_real_distribution<double>( 0.2, 1.2 ), // 1 Yr
    std::uniform_real_distribution<double>( 0.2, 1.2 ), // 1.0888 Zr

    std::uniform_real_distribution<double>( -5, 5 ), // 3.240479
    std::uniform_real_distribution<double>( -5, 5 ), // -1.53715
    std::uniform_real_distribution<double>( -5, 5 ), // -0.498535

    std::uniform_real_distribution<double>( -5, 5 ), // -0.969256
    std::uniform_real_distribution<double>( -5, 5 ), // 1.875991
    std::uniform_real_distribution<double>( -5, 5 ), // 0.041556

    std::uniform_real_distribution<double>( -5, 5 ), // 0.055648
    std::uniform_real_distribution<double>( -5, 5 ), // - 0.204043
    std::uniform_real_distribution<double>( -5, 5 ), // 1.057311

    std::uniform_real_distribution<double>( 0.3, 1 ), // 1
    std::uniform_real_distribution<double>( 1, 2 ), // 1
    std::uniform_real_distribution<double>( 0.3, 1 ), // 1

    std::uniform_real_distribution<double>( 0, 0 ), // 0
    std::uniform_real_distribution<double>( 0, 0 ), // 0
    std::uniform_real_distribution<double>( 0, 0 ), // 0

    std::uniform_real_distribution<double>( 1.0, 2.4 ), // 2.2

    // rgb scaling
    std::uniform_real_distribution<double>( 255, 255 ), // 255.0
    std::uniform_real_distribution<double>( 0, 0 ), // 0
    std::uniform_real_distribution<double>( 255, 255 ), // 255.0
    std::uniform_real_distribution<double>( 0, 0 ), // 0
    std::uniform_real_distribution<double>( 255, 255 ), // 255.0
    std::uniform_real_distribution<double>( 0, 0 ), // 0

    // rgb translation matrix
    std::uniform_real_distribution<double>( -10, 10 ), // 1.0
    std::uniform_real_distribution<double>( -10, 10 ), // 0.0
    std::uniform_real_distribution<double>( -10, 10 ), // 0.0
    std::uniform_real_distribution<double>( -10, 10 ), // 0.0
    std::uniform_real_distribution<double>( -10, 10 ), // 1.0
    std::uniform_real_distribution<double>( -10, 10 ), // 0.0
    std::uniform_real_distribution<double>( -10, 10 ), // 0.0
    std::uniform_real_distribution<double>( -10, 10 ), // 0.0
    std::uniform_real_distribution<double>( -10, 10 ), // 1.0



  };
}

Individual God::createIndividual()
{
  Individual ind;
  for ( int i = 0; i < Constants::len; ++i )
  {
    mutate( ind, i );
  }
  return ind;
}

void God::mutate( Individual &individual, int index )
{
  individual.constants.constants[index] = constantDist[index]( mt );
}

Individual God::breed( const Individual &mum, const Individual &dad )
{
  Individual ind;
  for ( int i = 0; i < Constants::len; ++i )
  {
    if ( breedDist( mt ) == 0 )
      ind.constants.constants[ i ] = mum.constants.constants[i];
    else
      ind.constants.constants[ i ] = dad.constants.constants[i];
  }
  return ind;
}


Population::Population( God &god, std::size_t count, std::size_t bestSample, std::size_t randomSample )
  : god( god )
  , count( count )
  , bestSample( bestSample )
  , randomSample( randomSample )
  , randomDist( std::uniform_int_distribution<std::size_t>( bestSample, count - 1 ) )
  , parentDist( std::uniform_int_distribution<std::size_t>( 0, bestSample + randomSample - 1 ) )
{
  individuals.reserve( count );
  for ( std::size_t i = 0; i < count; ++i )
    individuals.emplace_back( god.createIndividual() );
}


class DeltaCalc
{

  public:

    DeltaCalc( const std::vector<ColorMap> &map )
      : map( map )
    {}

    void operator()( Individual &i )
    {
      i.calcDelta( map );
    }

    const std::vector<ColorMap> &map;

};


void Population::calcDeltas( const std::vector<ColorMap> &map )
{
  DeltaCalc calc( map );
  QtConcurrent::blockingMap<std::vector<Individual> >( individuals, calc );
//  for ( auto &ind : individuals )
//   ind.calcDelta( map );
}

void Population::select()
{
  std::vector< Individual > parents;
  parents.reserve( bestSample + randomSample );

  // take best
  for ( std::size_t i = 0; i < bestSample; ++i )
    parents.emplace_back( individuals[ i ] );

  // take lucky few
  for ( std::size_t i = 0; i < randomSample; ++i )
    parents.emplace_back( individuals[ randomDist( mt ) ] );

  // breed and mutate
  std::vector< Individual > nextGen;
  nextGen.reserve( count );
  for ( std::size_t i = 0; i < count; ++i )
  {

    Individual ind = god.breed( parents[ parentDist( mt )], parents[parentDist( mt )] );

    for ( int j = 0; j < Constants::len; ++j )
    {
      if ( mutateChanceDist( mt ) < 0.1 )
      {
        god.mutate( ind, j /*mutateGene( mt )*/ );
      }
    }
    nextGen.emplace_back( ind );
  }

  individuals = nextGen;
}

void Population::sortByDelta()
{
  std::sort( individuals.begin(),
             individuals.end(),
             []( const Individual & l, const Individual & r )
  {
    return l.delta < r.delta;
  } );
}


void Individual::calcDelta( const std::vector<ColorMap> &map )
{
  delta = convert::delta( constants, map );
}

void Individual::dump()
{
  constants.dump();
}
