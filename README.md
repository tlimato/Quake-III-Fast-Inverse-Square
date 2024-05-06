[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/tTztJ7yI)
# Wildcard Project

You have a cool idea for an algorithms project? Use this repository. Make sure
to explain what problem you're solving, how you're doing it, and that you test
your code.

# Fast Inverse Square Quake 3 Arena
The concept for this project is both:

a) Exploring the methodology and motivation for the development of an engenious algorithm for a 3D videogame

b) Analyzing the Speed ups gained through this method and the time complexity of it compared to traditional methods. Potentially, to expand on the limited nature of asymptotic analysis for algorithms that differ by constants.


# The Code

The following code is the fast inverse square root implementation from Quake III Arena including the exact original comments text taken from the original source code.

```C
float q_rsqrt(float number)
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

```
## Modern Case:
```
float y = 1/sqrt(x) // given an include <math.h>
```
The motivation for the development of something faster than this operation was founded in the development of the Video Game Quake 3 Arena, a 1999 multiplayer-focused first-person shooter developed by id Software.
This game utilized an in-house 3D game engine to run the game and they needed an efficient means to handle Physics, lighting, and reflections, in 1999 before it was commonplace for PC gamer consumers to have GPUs in their machines as a given (nowadays it's expected for you to have a GPU for gaming). However, Physics, lighting, and reflections all require normalized vectors which need to be calculated in real time (or close to it) for the game to both look good and run well.

### Normalization

1) The length of a vector $(x, y, z)$ is given by:
   $$
   \text{Length} = \sqrt{x^2 + y^2 + z^2}
   $$

2) scale down the length of the vector to normalize said vector

  $$
     \left(\frac{x}{\sqrt{x^2 + y^2 + z^2}}, \frac{y}{\sqrt{x^2 + y^2 + z^2}}, \frac{z}{\sqrt{x^2 + y^2 + z^2}}\right)
  $$
  This represents each dimension multiplied by the reciprocal of the vector's length.

While this method provides accurate results, it is computationally slow. In contrast, the fast inverse square root algorithm offers a "good enough" approximation for video game applications, where exact precision is less critical than in scientific computations. This algorithm achieves an error margin of at most 1% while providing a speedup of approximately three times compared to the traditional method.

### Flow of the Function

1) input: float number

2) Internal values: 
  - long $i$: 32 bit number where $i$ is the bitwise representation of $y$

  - float $x_2$: 32 bit decimal number where $x_2$ is half of the input number
  - float $y$: 32 bit decimal number where $y$ is equal to the input number
  - float $threehalfs$: 32 bit decimal number

3) The rest of the components are broken down into the following steps:
- Evil Bit Hack
- What the f*ck?
- Newton Iteration
 By getting into the weeds with these respective components we can understand how ingenious the algorithm is and the reasoning behind it.

### Step 1: Evil Bit Hack
Here is the section in question.
```c
 long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
```
The first thing we need to understand is the bit representation of a floating point number. {Computer Organization, Digital System Design, C}
In C, the long data type is 32 bits structured into the following style of four 8 bit sections.
'''
00000000 00000000 00000000 00000000
'''
Conversely, floats in C utilize the IEEE 754 standard where they are structured into the following style of three sections, 1 sign bit, an 8 bit exponent (think scientific notation), and 23 bit mantissa.
'''
0 00000000 00000000000000000000000
'''
#### Example:

Given a 23 bit mantissa $M$ and an 8 bit exponent $E$, the value $V$ of the floating-point number is given by:
$$
V = (1 + \frac{M}{2^{23}}) \times 2^{(E-127)}
$$
derives to:
$$
(1+ \frac{M}{2^{23}}) \times 2^{(E-127)}
$$

if we take the log_2 of the equation we get:
$$
\log_2(V) = \log_2(1 + \frac{M}{2^{23}}) + (E-127) 
$$
This simplifies approximately to $log_2(1+x) \approx x + \mu$ for small values of x, where $\mu$ is the conversion coefficient. The conversion coefficient chosen by the Quake 3 programmers is $\mu$ = 0.0430 which gives the smallest error possible. (we will revisit why this is later)

Using this relationship, we can derive the following forms:

1) An initial form including the $\mu$ error term

$$
\log_2(V) = \frac{M}{2^{23}} + (E-127) + \mu
$$

2) Which simplifies to.

$$
\log_2(V) = \frac{M}{2^{23}} + \frac{E * 2^{23}}{2^{23}} + \mu - 127
$$

3) And gives the form:

$$
\log_2(V) = \frac{1}{2^{23}} * (M + E * 2^{23}) + \mu - 127
$$

THIS MEANS THAT the Bit Representation of a floating point number V is it's own log_2 value. This brings us to how the "Evil Bit Hack" Works.
The input is stored in float $y$. Floats, by default in C, do not enable us to use Bitwise Operations. Therefore, we must use the "Evil bit hack" to get the bitwise representation of the float.
We want to keep the decimal number and the bits that make it up, but in a long datatype without the conversion messing with our bits. (i.e just copy the bits 1-1 from a float y to a long). To do this we can convert the **memory address** NOT the number in it.

```c
i = * (long *) &y;
```
From right to left, we take the adress of a float and cast it to a pointer of a long adresss. we then dereference the pointer to the long , which reads the value as if it were a long and assigns it to $i$. Now we can manipulate the bits of $i$ to get the value we want.

### Step 2: What the f*ck?

Here is where our knowledge of the bit representation of a floating point number comes in to play.
```c
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
```
First let's talk about what on earth we are doing to $i$.
well we know that $log_2(y) = it's own bitwise representation$.
Therefore, we can use the bitwise representation to do the following
$$
log_2(y) \approx IEEE 754 Representation of float y
$$
Therefore, we can reasonably conclude that:
$$
log_2(\frac{1}{\sqrt{y}}) = log_2(y^(-1/2)) = -1/2 * log_2(y)
$$
in bitwise operations, this result is expressed as $\approx -(i >> 1)$
The $i >> 1$ operation performs a bitwise right shift by one on $i$. In terms of logarithms, shifting right by one bit is equivalent to dividing the number by 2, or mathematically, subtracting 1 from the exponent in the binary representation of a floating-point number. This operation approximates multiplying the logarithm of the number by -1/2.

Now what about the actual $log_2$ operation on y? What is the weird 0x5f3759df value and why are we subtracting our bit shift from it?

To find out let's take a more rigourous look at our relationship:
$$
log_2(y) \approx IEEE-754-Representation-of-float-y
$$
let $\sigma = \frac{1}{\sqrt{y}}$ and substitute it into our bit representation.
$$
\frac{1}{2^(23)}(M_\sigma + E_\sigma * 2^{23}) + \mu - 127
= \frac{-1}{2}(\frac{1}{2^{23}} * (M_y + 2^{23} * E_y) + \mu - 127)
$$
we can then solve for $M_\sigma$ and $E_\sigma$ gives:
$$
(M_\sigma +  2^{23} * E_\sigma) = \frac{3}{2} * 2^{23} * (127 - \mu)- \frac{1}{2}(M_y + 2^{23} *E_y)
$$
where we can then derive 0x5f3759df from the following:
$$
0x5f3759df = \frac{3}{2} * 2^{23} * (127 - \mu) , \mu = 0.0430
$$
more explicitly 0x5f3759df is the hexidicmal result of the following:
$$
\frac{3}{2} * 2^{23} * (127 - 0.0430)
$$
This results in the complete long estimate: 
$$
i = 0x5f3759df - ( i >> 1 )
$$
then the next step:
```c
y  = * ( float * ) &i;
```
This converts the long back to a float and assigns it to y.
From right to left, we take the address of a long and cast it to a pointer of a float adresss. we then dereference the pointer to the float, which reads the value as if it were a float and assigns it to $y$. This gives us a good approximation which we can refine in the next step.

### Step 3: Newton Iteration
After step 2, we have a decent approximation, but are left with some error terms. Newton's method is a way to refine the approximation by taking the derivative of the function and solving for the root $(f(x) = 0)$. This is done by taking an approximation and then returning a better approximation. The Quake 3 Developers chose to use Newton's method to refine the approximation as only a single iteration gives an error within 1%. I have drawn what this process looks like visually and will go into more detail for it's implications in code after.

![alt text](<Newton Approx.jpg>)


## References
(1) https://en.wikipedia.org/wiki/Fast_inverse_square_root
