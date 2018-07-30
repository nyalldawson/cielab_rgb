#include <QCoreApplication>
#include <QDebug>
#include <QFile>

#include "convert.h"
#include "population.h"

int main( int argc, char *argv[] )
{
  QCoreApplication a( argc, argv );

  QFile file( "/home/nyall/dev/genetic/colors.txt" );
  file.open( QIODevice::ReadOnly );
  QTextStream in( &file );
  std::vector< ColorMap > map;
  int u = 0;
  while ( !in.atEnd() )
  {
    u++;
    QString line = in.readLine();
    QStringList parts = line.split( ',' );
    if ( u % 100  == 0 )
      map.emplace_back( ColorMap( parts[3].toDouble(), parts[4].toDouble(), parts[5].toDouble(),
                                  parts[0].toDouble(), parts[1].toDouble(), parts[2].toDouble() ) );
  }

  qDebug() << map.size();

//  Constants c;
  God g;
  Population pop( g );

  pop.calcDeltas( map );
  pop.sortByDelta();
  qDebug() << pop.individuals[0].delta;
  int i = 0;
  while ( true )
  {
    pop.select();
    pop.calcDeltas( map );
    pop.sortByDelta();
    qDebug() << "----";
    qDebug() << pop.individuals[0].delta << " gen: " << i++;
    pop.individuals[0].dump();
  }

#if 0
  int r, g, b;
  convert::cielabToRgb( c, 97.1, -21.5, 94.5, r, g, b );
  qDebug() << r << g << b;
#endif


  //qDebug() << convert::delta( c, map );
  return 0;
}
