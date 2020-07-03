/* Program for handling elliptic curve arithmetic.
 * This program only works with curves of the form y²=x³+ax+b
 * Written by Victor Zamora on July 3rd 2020
 */
#include "ECArithmetic_def.h"
#include <stdio.h>


// Returns 1 if the point is in the EC, 0 otherwise.
int in_curve(point point){

  // Left side of the equality.
  int left = MULT(point.y, point.y);

  // Right side of the equality.
  int right = MULT(MULT(point.x, point.x), point.x);
  right += MULT(a, point.x);
  right += b;

  if(left == right)
    return 1;

  return 0;
}

// Returns the order of the point's generated subgroup.

int main(){

}
 
