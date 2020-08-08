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

// Constants

// Point at infinity 
const point PAI = {-1, -1};

// Macros

// Functions
int in_curve(point);
int equal(point, point);
int inv(int);
int multiply(int, int);
int modulo(int, int);
int order(point);
point inverse(point);
point add(point, point);
point fast_multiplication(int, point);
void print_points(point[], int);
int is_prime(int);
int main();

