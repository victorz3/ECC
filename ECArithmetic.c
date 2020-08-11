/* Program for handling elliptic curve arithmetic.
 * This program only works with curves of the form y²=x³+ax+b
 * Written by Victor Zamora on July 3rd 2020
 */
#include "ECArithmetic_def.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

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

// Returns the inverse modulo a prime p of the number.
int inv(int x, int p){
  x = x % p;
  if(x < 0)
    x = p + x;
  // Remainders and elements of the linear combination in the extended Euclidean algorithm
  int r0, r1, r_temp, t0, t1, t_temp;
  r0 = p, r1 = x, t0 = 0, t1 = 1;
  r_temp = r1, t_temp = t1;
  while(r1 != 1){
    // Quotient of division
    int q = r0 / r1;
    r_temp = r0 % r1;
    t_temp = t0 - q*t1;
    r0 = r1, r1 = r_temp;
    t0 = t1, t1 = t_temp;
  }
  return t1 < 0 ? p + t1 : t1;
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
  point q = {p.x, modulo((prime - p.y), prime)};
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
      alpha = multiply(p1.y-p2.y, inv(p1.x - p2.x, prime));
    else{
      // Temporary variable
      int temp = multiply(p1.x, p1.x);
      alpha = 3*temp;
      alpha = modulo(alpha, prime);
      alpha += a;
      alpha = modulo(alpha, prime);
      alpha = multiply(alpha, inv(p1.y + p1.y, prime));
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
  // Base 2 logarithm of d.
  int log2d = log(d) / log(2);
  // Second Highest power of 2 smaller than d
  int h_power = 1 << (log2d - 1);
  // Auxiliary value
  point y = {p.x, p.y};
  if(d == 0)
    return PAI;
  int coef = 1;
  while(h_power > 0){
    if((h_power & d) > 0){
      y = add(y, y);
      y = add(y, p);
      coef = 2*coef + 1;
    }
    else{
      y = add(y, y);
      coef = 2*coef;
    }
    h_power >>= 1;
  }
  return y;
}

// Performs multiplication by scalar slowly.
point slow_addition(int d, point p){
  // Auxiliary value
  point y = PAI;
  if(d == 0)
    return y;
  // Counter
  int count;
  for(count = 0; count < d; ++count)
    y = add(y, p);
  return y;
} 

// Prints an array of points.
void print_points(point points[], int size){
  // Counter
  int i;
  for(i = 0; i < size; ++i)
    printf("(%d, %d) ", points[i].x, points[i].y);
  printf("\n");
}

// Tells us whether a number is prime.
// This method is extremely inefficient.
int is_prime(int p){
  if(p < 2)
    return 0;
  // Potential divisor
  int i;
  // Square root of p.
  double s = sqrt(p);
  for(i = 2; i <= s; ++i)
    if(p % i == 0)
      return 0;
  return 1;
}

// Signs document in ECDSA schema.
point sign(int k, point p, int n, int d, int(*hash)(char*), char *document){
  
  // Computing k * P
  point kp = fast_addition(k, p);
  // Computing r = x1 % n.
  int r = modulo(kp.x, n);

  if(r == 0){
    printf("Incorrect value of k. Please pick another integer in the interval [1, n-1]\n");
    exit(1);
  }

  // Inverse modulo n of k.
  int k_inv = inv(k, n);
  // Computing s = k_inv * (h(m) + dr) where h is hash function, m is message
  int s = modulo(k_inv * (hash(document) + modulo(d * r, n)), n);
  
  if(s == 0){
    printf("Incorrect value of k. Please pick another integer in the interval [1, n-1]\n");
    exit(1);
  }

  // Return value.
  point ret = {r, s};
  return ret;
}

// Verifies correct signature.
int verify_sign(point sign, int n, int(*hash)(char*), char *document, point p, point q){
  if(sign.x < 1 || sign.x > (n-1) || sign.y < 1 || sign.y > (n-1))
    return 0;
  // Compute w = s_inv mod n.
  int w = inv(sign.y, n);
  // Compute h = hash(message)
  int h = hash(document);
  // Compute v1 = h * w mod n.
  int u1 = modulo(h*w, n);
  // Compute v2 = r*w mod n.
  int u2 = modulo(sign.x*w, n);
  // Intermediate steps for x0, y0.
  point p0, q0;
  p0 = fast_addition(u1, p);
  q0 = fast_addition(u2, q);
  point sum = add(p0, q0);
  int v = modulo(sum.x, n);
  if(v == sign.x)
    return 1;
  return 0;
}

int main(){
  return 0;
}
 
