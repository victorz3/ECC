/* Definitions for the elliptic curve arithmetic.
 * Written by Victor Zamora on July 3rd 2020
 */

// Datatypes

// Basic point structure.
typedef struct{
  int x;
  int y;
} point;

// Variables

// Curve's coefficients with default value 1.
int a = 1, b = 1;

// Prime defining the field we are working on. Default value 5.
int prime = 5;

// Macros

// Multiplication of two integers
#define MULT(x, y) (x*y) % prime

// Modular addition
#define ADD(x, y) (x+y) % prime


// Functions
int in_curve(point);
int main();

