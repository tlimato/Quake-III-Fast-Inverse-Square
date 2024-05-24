# Fast Inverse Square Quake 3 Arena
The concept for this project is both:

a) Exploring the methodology and motivation for the development of an ingenious algorithm from Quake 3 (1999) a 3D Shooter Video Game that came out before I was born that I think was really cool and have a nostalgic connection to as I played it on my parent's old computer as a kid.

b) Analyzing the Speed-ups gained through this method and the time complexity of it compared to traditional methods. Potentially, to expand on the limited nature of asymptotic analysis for algorithms that differ by constants.

The following code is the fast inverse square root implementation from Quake III Arena including the exact original comments text taken from the source code.
# The Code

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
In C, the long data type is 32 bits structured into the following style of four 8-bit sections.
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

**Visual Representation of Newton Approximation**

![alt text](<Newton Approx.jpg>)

With respect to the actual code, the Newton Iteration is done as follows:

```c
y = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
```

This line of code represents a single iteration of Newton's method for refining the approximation of the inverse square root. The mathematical basis of which is follows:

Given the function:

$$
f(y) = \frac{1}{y^2} - x
$$

where $x$ is the number we want to find the inverse square root, and $y$ is our current approximation. The goal is to find $y$ such that $f(y) = 0$, which implies:

$$
y = \frac{1}{\sqrt{x}}
$$

Newton's method updates the approximation $y$ using the formula:

$$
y_{\text{new}} = y - \frac{f(y)}{f'(y)}
$$

where $f'(y)$ is the derivative of $f(y)$. For the function $f(y)$, the derivative $f'(y)$ is:

$$
f'(y) = -\frac{2}{y^3}
$$

Substituting $f(y)$ and $f'(y)$ into the Newton's update formula gives:

$$
y_{\text{new}} = y - \frac{\frac{1}{y^2} - x}{-\frac{2}{y^3}} = y + \frac{y^3 (\frac{1}{y^2} - x)}{2} = y + \frac{y - xy^3}{2}
$$

Simplifying further, we get:

$$
y_{\text{new}} = y \left(\frac{3}{2} - \frac{x y^2}{2}\right)
$$

This is exactly what the code does, connecting back we can see:

```c
y = y (threehalfs - (x2 y y));
```
where $x2$ is $\frac{x}{2}$, making the expression equivalent to the mathematical derivation.

This step significantly improves the accuracy of the approximation by using the derivative to adjust the estimate based on how far off the current approximation is from the actual inverse square root.

### Breakdown Conclusions

It's incredible how a relatively innocuous piece of code can be so detailed an complex. Given the limited performance of computers and the design constraint of rendering on CPU in 1999, this code is a great example of how small changes can have a big impact on the performance of a program. In this class we have focused on runtime complexity using aystompotic analysis, and have hinted at the importance of memory management and reducing unnecessary operations even if they are constant but haven't really delved into an algorithm that concerns itself with this too much. 

If we look at the **asymptotic complexity** of Quake 3's  Fast Inverse Square Root implementation, it has a complexity of $O(1)$. There is no iterating over a loop nor any factors impacted by a given number of elements.
This means that in theory their algorithm would impact nothing given just the basic Inverse square root:
$i = \frac{1}{\sqrt{x}}$

is also a constant time operation: $O(1)$ yet Quakes is much Faster. As outlined in the code review portion of this assignment, Quake 3's Programmers used efficiencies in their code to make the program run faster. This includes using bitwise operations to manipulate the bits of a float and using Newton's method to refine the approximation of the inverse square root all while abusing the IEEE 754 standard to manipulate floating point numbers. Given traditionally on older machines, operations like division and square roots are much slower than bitwise operations and addition and subtraction, these optimizations influence a lot of the overhead for calculating the inverse square root.

In games where the Frames per Second is a major concern, this can add up to a significant amount of time saved in the program when a large number of vectors need to be normalized for lighting, physics, and rendering in the actual deployment of the game. From a personal perspective, I love Quake 3 as a game, growing up with very slow computers from the early 2000s that could only run games like Quake 3 and Half Life I wanted to explore an algorithm that was relevant to it.

## Next Steps
As a means to implement this myself using techniques that were covered in class, i thought it would be appropriate to create a parallelized version of this function and compare it to the original as well as the traditional $\frac{1}{\sqrt{x}}$ and measure timings. While not a perfectly scientific method, it would be interesting to see how the speedup from using Quake 3's method and my parallelization compares to the traditional method given a large amount of data to iterate over.

Given this code is in C++, the only framework I'm even moderately familiar with for parallelization is OpenMP. I used to work at the University of Wyoming Advanced Research Computing Center and got some exposure to this topic prior to the class. 

I was able to implement a parallelized version of this function in 'main.cpp' and compare it to the original as well as the traditional $\frac{1}{\sqrt{x}}$ and measure timings. See Main.cpp for the code.

### Results
a data set of 10 million numbers was used to test the speed of the different implementations given this was required to register a meaningful difference between all the algorithms
The raw results as output by the program in the terminal are as follows:

```bash
Run number 1
Parallel Quake 3, Execution time: 51 milliseconds
Sequential Quake 3, Execution time: 136 milliseconds
Manual Sqrt, Execution time: 93 milliseconds
Modern math.h library sqrt, Execution time: 50 milliseconds
Run number 2
Parallel Quake 3, Execution time: 42 milliseconds
Sequential Quake 3, Execution time: 138 milliseconds
Manual Sqrt, Execution time: 92 milliseconds
Modern math.h library sqrt, Execution time: 49 milliseconds
Run number 3
Parallel Quake 3, Execution time: 37 milliseconds
Sequential Quake 3, Execution time: 144 milliseconds
Manual Sqrt, Execution time: 96 milliseconds
Modern math.h library sqrt, Execution time: 50 milliseconds
Run number 4
Parallel Quake 3, Execution time: 37 milliseconds
Sequential Quake 3, Execution time: 140 milliseconds
Manual Sqrt, Execution time: 95 milliseconds
Modern math.h library sqrt, Execution time: 50 milliseconds
Run number 5
Parallel Quake 3, Execution time: 38 milliseconds
Sequential Quake 3, Execution time: 146 milliseconds
Manual Sqrt, Execution time: 95 milliseconds
Modern math.h library sqrt, Execution time: 53 milliseconds
Run number 6
Parallel Quake 3, Execution time: 39 milliseconds
Sequential Quake 3, Execution time: 141 milliseconds
Manual Sqrt, Execution time: 92 milliseconds
Modern math.h library sqrt, Execution time: 50 milliseconds
Run number 7
Parallel Quake 3, Execution time: 38 milliseconds
Sequential Quake 3, Execution time: 138 milliseconds
Manual Sqrt, Execution time: 92 milliseconds
Modern math.h library sqrt, Execution time: 50 milliseconds
Run number 8
Parallel Quake 3, Execution time: 38 milliseconds
Sequential Quake 3, Execution time: 141 milliseconds
Manual Sqrt, Execution time: 94 milliseconds
Modern math.h library sqrt, Execution time: 51 milliseconds
Run number 9
Parallel Quake 3, Execution time: 44 milliseconds
Sequential Quake 3, Execution time: 141 milliseconds
Manual Sqrt, Execution time: 93 milliseconds
Modern math.h library sqrt, Execution time: 49 milliseconds
Run number 10
Parallel Quake 3, Execution time: 38 milliseconds
Sequential Quake 3, Execution time: 138 milliseconds
Manual Sqrt, Execution time: 92 milliseconds
Modern math.h library sqrt, Execution time: 49 milliseconds
```

This gives the following table:
| Run Number | Parallel Quake 3 (ms) | Sequential Quake 3 (ms) | Manual Sqrt (ms) | Modern math.h library sqrt (ms) |
|------------|-----------------------|-------------------------|------------------|---------------------------------|
| 1 | 51 | 136 | 93 | 50 |
| 2 | 42 | 138 | 92 | 49 |
| 3 | 37 | 144 | 96 | 50 |
| 4 | 37 | 140 | 95 | 50 |
| 5 | 38 | 146 | 95 | 53 |
| 6 | 39 | 141 | 92 | 50 |
| 7 | 38 | 138 | 92 | 50 |
| 8 | 38 | 141 | 94 | 51 |
| 9 | 44 | 141 | 93 | 49 |
| 10 | 38 | 138 | 92 | 49 |

Of course, the modern math.h library sqrt is still faster than the traditional sqrt and even the traditional Quake 3's method. This is because of modern math.h library sqrt is implemented in hardware and optimized for the specific architecture it is running on, while the traditional sqrt and Quake 3's method are implemented in software and require the software emulation of the hardware instructions. Furthermore, Quake 3's method was designed with the popular Pentium III in mind, which was only a single core and no multithreading meaning, their fast inverse square root algorithm won't leverage the multiple cores and threads available on modern computers like math.h.

Interestingly, by using OpenMP, we can see that the parallelized version of Quake 3's method is faster in some cases than modern math.h sqrt method, which is unexpected and quite a bit faster than the sequential Quake 3 method. This is because the parallelized version of Quake 3's method uses multiple threads to compute the inverse square root in parallel, which can benefit multi-core processors. However, the parallelized version of Quake 3's method is still slower than the traditional sqrt and modern math.h library sqrt. This is because the parallelized version of Quake 3's method uses shared memory to pass data between threads, which can be slower than a method that is more meticulous and complex like the sqrt in the math.h library.

### Analysis
parallel_q_rsqrt computes the inverse square root of each element in an array using a parallelized loop. The loop iterates over each element of the input array numbers and performs a constant amount of work for each element, regardless of the size of the array. The key operations inside the loop (bit manipulation and arithmetic operations) do not depend on the size of the array and are executed a fixed number of times for each element.
Despite the function being parallelized accross multiple threads, the time complexity is still $O(n)$. The speed up comes from the fact that the parallelized version of the Quake 3's method is able to leverage the multiple cores and threads available on modern computers to compute the inverse square root of each element in the array in parrallel, which was observed to cause a significant speed up in execution time across a large set of data.

## References

1. Wikipedia contributors. (n.d.). Fast inverse square root. Wikipedia, The Free Encyclopedia. Retrieved from https://en.wikipedia.org/wiki/Fast_inverse_square_root
2. id Software. (n.d.). Quake III Arena. GitHub repository. Retrieved from https://github.com/id-Software/Quake-III-Arena
3. Azad, K. (n.d.). Understanding Quake's Fast Inverse Square Root. BetterExplained. Retrieved from https://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/
4. ThatOneGameDev. (n.d.). Fast Square Root in Quake III. ThatOneGameDev. Retrieved from https://thatonegamedev.com/math/fast-square-root-quake-iii/
5. 0xcode. (n.d.). The Brilliance of Quake's Fast Inverse Square Root Algorithm. Medium. Retrieved from https://medium.com/0xcode/the-brilliance-of-quakes-fast-inverse-square-root-algorithm-d18365f8bba2
