/* Program for handling elliptic curve arithmetic.
 * This program only works with curves of the form y²=x³+ax+b
 * Written by Victor Zamora on July 3rd 2020
 */
#include "ECArithmetic_def.h"
#include <stdio.h>
#include <string.h>

// Returns 1 if the point is in the EC, 0 otherwise.
int in_curve(point point){

  // Left side of the equality.
  int left = multiply(point.y, point.y);
  
  // Right side of the equality.
  int right = multiply(multiply(point.x, point.x), point.x);

  right += multiply(a, point.x);
  right += b;
  // Was missing modulo here. Important in case it goes over prime.
  right = modulo(right, prime); 

  if(left == right)
    return 1;

  return 0;
}

// Returns 1 if points are equal, false otherwise.
int equal(point p1, point p2){
  if((p1.x == p2.x) && (p1.y == p2.y))
    return 1;
  return 0;
}

// Returns the inverse modulo prime of the number.
int inv(int x){
  x = x % prime;
  if(x < 0)
    x = prime + x;
  // Remainders and elements of the linear combination in the extended Euclidean algorithm
  int r0, r1, r_temp, t0, t1, t_temp;
  r0 = prime, r1 = x, t0 = 0, t1 = 1;
  r_temp = r1, t_temp = t1;
  while(r1 != 1){
    // Quotient of division
    int q = r0 / r1;
    r_temp = r0 % r1;
    t_temp = t0 - q*t1;
    r0 = r1, r1 = r_temp;
    t0 = t1, t1 = t_temp;
  }
  return t1 < 0 ? prime + t1 : t1;
}

// Multiplies modulo prime
int multiply(int x, int y){
  return modulo(x*y, prime);
}

// Modulo operation
int modulo(int x, int y){
  // Remainder of division
  int mod = x % y;
  return mod < 0 ? y + mod : mod;
}

// Returns the order of the point's generated subgroup.
int order(point p){
  // Current computed order.
  int o = 1;
  // Current computed sum.
  point sum = p;
  while(!equal(sum, PAI)){
    sum = add(sum, p);
    o++;
  }
  return o;
}


// Returns the inverse of the given point.
point inverse(point p){
  if(equal(PAI, p))
    return PAI;
  // The inverse.
  point q = {p.x, (prime - p.y)%prime};
  return q;
}

// Performs modular addition of p1 and p2
point add(point p1, point p2){
  if(equal(PAI, p1))
    return p2;
  else if(equal(PAI, p2))
    return p1;
  else if(equal(p1, inverse(p2)))
    return PAI;
  else{
    // Slope of the line p1p2
    int alpha;
    if(!equal(p1, p2))
      alpha = multiply(p1.y-p2.y, inv(p1.x - p2.x));
    else{
      // Temporary variable
      int temp = multiply(p1.x, p1.x);
      alpha = 3*temp;
      alpha += a;
      alpha = multiply(alpha, inv(p1.y + p1.y));
    }
    // Result coordinates.
    int x3, y3;
    x3 = multiply(alpha, alpha) - (p1.x + p2.x);
    x3 = modulo(x3, prime);
    y3 = multiply(alpha, p1.x - x3) - p1.y;
    y3 = modulo(y3, prime);
    // The resulting point.
    point sum = {x3, y3};
    return sum;
  }  
}

// Performs fast multiplication algorithm to add p to itself d times.
point fast_addition(int d, point p){
  // Auxiliary value
  point y = PAI;
  if(d == 0)
    return y;
  while(d > 1){
    if(d & 1)
      y = add(y, p);
    p = add(p, p);
    d /= 2;
  }
  return add(y, p);
  
}

// Prints an array of points.
void print_points(point points[], int size){
  // Counter
  int i;
  for(i = 0; i < size; ++i)
    printf("(%d, %d) ", points[i].x, points[i].y);
  printf("\n");
}

int main(){
  printf("TAREA EXAMEN 2:\n");
  a = b = 1;
  prime = 30677;
  point p = {1090, 18593};
  char answer[10];
  if(in_curve(p))
    strcpy(answer, "sí");
  else
    strcpy(answer, "no");
  printf("El punto (%d, %d) es racional? %s\n", p.x, p.y, answer);
}
 
