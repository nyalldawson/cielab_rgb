#include "convert.h"
#include <cmath>
#include <QDebug>

void convert::cieluvToXyz( const Constants &constants, double l, double u, double v, double &x, double &y, double &z )
{
  l = l * constants.constants[18 ]  + constants.constants[21];
  u = u * constants.constants[19 ]  + constants.constants[22];
  v = v * constants.constants[20 ]  + constants.constants[23];

  double Xr = constants.constants[6];
  double Yr = constants.constants[7];
  double Zr = constants.constants[8];


  double u0 = 4 * Xr / ( Xr + 15 * Yr + 3 * Zr );
  double v0 = 9 * Yr / ( Xr + 15 * Yr + 3 * Zr );

  double e = constants.constants[1];
  double k = constants.constants[2];

  if ( l > k * e )
    y = std::pow( ( l + constants.constants[4] ) / constants.constants[3], 3 );
  else
    y = l / k;

  double a = 1.0 / 3.0 * ( ( 52 * l ) / ( u + 13 * l * u0 ) - 1 );
  double b = -5 * y;
  double c = -1.0 / 3;
  double d = y * ( ( 39 * l ) / ( v + 13 * l * v0 ) - 5 );

  x = ( d - b ) / ( a - c );
  z = x * a + b;
}

void convert::cielabToXyz( const Constants &constants, double l, double a, double b, double &x, double &y, double &z )
{
  l = l * constants.constants[18 ]  + constants.constants[21];
  a = a * constants.constants[19 ]  + constants.constants[22];
  b = b * constants.constants[20 ]  + constants.constants[23];

  double fy = ( l + constants.constants[4] ) / constants.constants[3];
  double fz =  fy - ( b / constants.constants[5] );
  double fx = a / constants.constants[0] + fy;
  double e = constants.constants[1];
  double k = constants.constants[2];
  double xr;
  if ( fx * fx * fx > e )
    xr = fx * fx * fx;
  else
    xr = ( constants.constants[3] * fx - constants.constants[4] ) / k;

  double yr;
  if ( l > k * e )
    yr = std::pow( ( l + constants.constants[4] ) / constants.constants[3], 3 );
  else
    yr = l / k;

  double zr;
  if ( fz * fz * fz > e )
    zr = fz * fz * fz;
  else
    zr = ( constants.constants[3] * fz - constants.constants[4] ) / k;

  double Xr = constants.constants[6];
  double Yr = constants.constants[7];
  double Zr = constants.constants[8];

  x = xr * Xr;
  y = yr * Yr;
  z = zr * Zr;
}

double compand( const Constants &constants, double v )
{
#if 0
  // l*
  if ( v < 0.008856 )
    return v * 903.3 / 100;
  else
    return 1.16 * std::pow( v, 1 / 3.0 ) - 0.16;

#elif 0
  // gamma compand
  return std::pow( v, 1 / constants.constants[24] );
#endif

#if 1
// srgb compand
  if ( v <= 0.0031308 )
    return 12.92 * v;
  else
    return 1.055 * std::pow( v, 1 / 2.4 ) - 0.055;
#endif
}

void convert::xyzToRgb( const Constants &constants, double x, double y, double z, int &r, int &g, int &b )
{
  r = std::round( 255 * compand( constants, constants.constants[9] * x + constants.constants[10] * y + constants.constants[11] * z ) );
  g = std::round( 255 * compand( constants, constants.constants[12] * x + constants.constants[13] * y + constants.constants[14] * z ) );
  b = std::round( 255 * compand( constants, constants.constants[15] * x + constants.constants[16] * y + constants.constants[17] * z ) );
}

void convert::cielabToRgb( const Constants &constants, double l, double a, double b, int &r, int &g, int &bl )
{
  double x, y, z;
  cielabToXyz( constants, l, a, b, x, y, z );
  xyzToRgb( constants, x, y, z, r, g, bl );
}

void convert::cieluvToRgb( const Constants &constants, double l, double u, double v, int &r, int &g, int &bl )
{
  double x, y, z;
  cieluvToXyz( constants, l, u, v, x, y, z );
  xyzToRgb( constants, x, y, z, r, g, bl );
}

unsigned long long convert::delta( const Constants &constants, double l, double a, double b, int r, int g, int bl )
{
  int rr, gg, bb;
  cielabToRgb( constants, l, a, b, rr, gg, bb );
//  cieluvToRgb( constants, l, a, b, rr, gg, bb );
  return static_cast< unsigned long long  >( r - rr ) * ( r - rr ) + ( g - gg ) * ( g - gg ) + ( bb - bl ) * ( bb - bl );
}

unsigned long long convert::delta( const Constants &constants, const ColorMap &map )
{
  return delta( constants, map.l, map.a, map.b, map.r, map.g, map.bl );
}

unsigned long long convert::delta( const Constants &constants, const std::vector<ColorMap> &map )
{
  unsigned long long  d = 0;
  for ( const ColorMap &m : map )
  {
    d += delta( constants, m );
  }
  return d;
}

double Constants::defaults[] =
{
  500.0,
  0.008856, // ϵ
  903.3, // κ
  116.0, //[3]
  16.0, //[4]
  200.0,
  0.9504, // Xr
  1, // Yr
  1.0888, // Zr

  // Transformation matrix M - see http://www.brucelindbloom.com/index.html?Eqn_Lab_to_XYZ.html
  3.240479,
  -1.53715,
  -0.498535,

  -0.969256,
  1.875991,
  0.041556,

  0.055648,
  - 0.204043,
  1.057311,

  1,
  1,
  1,

  0,
  0,
  0,

  2.2// gamma
};

void Constants::dump()
{
  for ( int i = 0; i < len; ++i )
    qDebug() << constants[i] << " (" << defaults[i] << ")";
}
