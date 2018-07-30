#ifndef CONVERT_H
#define CONVERT_H

#include <vector>

class Constants
{
  public:

    Constants()
    {

    }

    static constexpr int len = 25;
    static double defaults[];
    void dump();


    double constants[len];
};

class ColorMap
{
  public:
    ColorMap( double l, double a, double b, int r, int g, int bl )
      : l( l )
      , a( a )
      , b( b )
      , r( r )
      , g( g )
      , bl( bl )
    {}

    double l;
    double a;
    double b;
    int r;
    int g;
    int bl;
};

class convert
{
  public:

    static void cielabToXyz(
      const Constants &constants,
      double l, double a, double b, double &x, double &y, double &z );

    static void xyzToRgb( const Constants &constants, double x, double y, double z,
                          int &r, int &g, int &b );

    static void cielabToRgb( const Constants &constants,
                             double l, double a, double b, int &r, int &g, int &bl );

    static unsigned long long delta( const Constants &constants,
                                     double l, double a, double b, int r, int g, int bl );

    static unsigned long long  delta( const Constants &constants,
                                      const ColorMap &map );

    static unsigned long long  delta( const Constants &constants,
                                      const std::vector< ColorMap > &map );

};

#endif // CONVERT_H
