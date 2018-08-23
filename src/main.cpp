#include <QCoreApplication>
#include <QDebug>
#include <QFile>

#include "convert.h"
#include "population.h"

int main( int argc, char *argv[] )
{
  QCoreApplication a( argc, argv );

  QFile file( QStringLiteral( "/home/nyall/dev/slyr/data/colors_full.txt" ) );
  qDebug() << file.open( QIODevice::ReadOnly );

  QTextStream in( &file );
  std::vector< ColorMap > map;
  int u = 0;
  while ( !in.atEnd() )
  {
    u++;
    QString line = in.readLine();
    QStringList parts = line.split( ',' );
    //if ( u % 100  == 0 )
    // if ( parts[3].toDouble() < 32 )
    //   continue;

    map.emplace_back( ColorMap( parts[3].toDouble(), parts[4].toDouble(), parts[5].toDouble(),
                                parts[0].toInt(), parts[1].toInt(), parts[2].toInt() ) );
  }

  qDebug() << map.size();
#if 0
//  Constants c;
  God g;
  Population pop( g );

  pop.calcDeltas( map );
  pop.sortByDelta();
  qDebug() << pop.individuals[0].delta;
  int i = 0;
  while ( pop.individuals[0].delta > 0 )
  {
    pop.select();
    pop.calcDeltas( map );
    pop.sortByDelta();
    if ( i % 100 == 0 )
    {
      qDebug() << "----";
      qDebug() << pop.individuals[0].delta << " gen: " << i;
      pop.individuals[0].dump();
    }
    i++;
  }

  pop.individuals[0].dump();
#else

  QFile outFile( QStringLiteral( "/home/nyall/lut.txt" ) );
  outFile.open( QIODevice::ReadWrite | QIODevice::Truncate );

  QTextStream stream( &outFile );

  Individual i;
  Constants c;
  int variants = 0;
  //double maxL = std::numeric_limits< double >::lowest();
  //double minL = std::numeric_limits< double >::max();
// double maxDL = std::numeric_limits< double >::lowest();
// double minDL = std::numeric_limits< double >::max();
  for ( const ColorMap &m : map )
  {
    //maxL = std::max( m.l, maxL );
    //minL = std::min( m.l, minL );
    //if ( convert::delta( c, m ) > 0 )
    if ( !convert::dumpDelta( c, m, stream ) )
    {
      //  maxDL = std::max( m.l, maxDL );
      //  minDL = std::min( m.l, minDL );
      variants ++;
    }

    // break;
  }
  //qDebug() << minL;
  //qDebug() << minDL;
  //qDebug() << maxL;
  //qDebug() << maxDL;
  qDebug() << variants;


#endif
#if 0
  int r, g, b;
  convert::cielabToRgb( c, 97.1, -21.5, 94.5, r, g, b );
  qDebug() << r << g << b;
#endif


  //qDebug() << convert::delta( c, map );
  return 0;
}
