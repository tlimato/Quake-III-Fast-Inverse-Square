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

The following code is the fast inverse square root implementation from Quake III Arena including the exact original comment text

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
1) a given length vector is equal to $\sqrt(x^2 + y^2 + z^2)$
2) scale down the length of the vector to normalize said vector
  $$
  x/(\sqrt(x^2 + y^2 + z^2) , y/(\sqrt(x^2 + y^2 + z^2), z/(\sqrt(x^2 + y^2 + z^2)
  $$
  or in other words, the respective dimension multiplied by $1/length$

This gives an accurate although slow way to normalize vectors using inverse square. Conversely, the fast inverse square root algorithm serves as a fast "Good Enough" approximation of the result for usage in a video game where exact results aren't as important compared to scientific applications. Specifically, this algorithm boasts an error of at most 1% with a 3 times execution speedup.


## References
(1) https://en.wikipedia.org/wiki/Fast_inverse_square_root
