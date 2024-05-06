#include <omp.h>
#include <iostream>
// for traditional sqrt
#include <math.h>
// For measuring time
#include <chrono>
#include <vector>

// Parallel Quake 3's method implimentation using OpenMP.
void parallel_q_rsqrt(float* numbers, float* results, int n) {
    const float threehalfs = 1.5F;

    // Parallelize the loop using OpenMP. "omp parallel for" is a directive that tells the compiler to parallelize the loop across multiple threads.
    #pragma omp parallel for
    for (int idx = 0; idx < n; idx++) {
        float number = numbers[idx];
        long i;
        float x2, y;

        x2 = number * 0.5F;
        y = number;
        i = *(long*)&y;                          // evil floating point bit level hacking
        i = 0x5f3759df - (i >> 1);               // what the fuck?
        y = *(float*)&i;
        y = y * (threehalfs - (x2 * y * y));     // 1st iteration

        results[idx] = y;
    }
}

// Standard Quake 3's method, Taken directly from the Wiki.
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

// just plain ole' modern sqrt from math.h
float modern_mathH(float number) {
    return 1.0 / sqrt(number);
}

// Manual sqrt using Newton's method. Closer to a manual implementation of inverse sqrt. That can't leverage the hardware as much.
float manual_inv_sqrt(float x) {
    if (x <= 0) return 0; // Handle non-positive input

    float y = 1.0f / sqrt(x); // Initial guess using the standard sqrt function for a better start
    float last_y = 0.0f;

    // Iterate using Newton's method until the change is small enough
    while (fabs(y - last_y) > 1e-6) {
        last_y = y;
        y = y * (1.5f - (x * 0.5f * y * y)); // Newton-Raphson iteration for inverse square root
    }

    return y;
}


// Runtime
int main() {
    const int array_size = 10000000; // Example size
    // I needed to use a vector for this because I kept getting segmentation errors.
    std::vector<float> numbers(array_size);
    std::vector<float> results(array_size);

    // Initialize numbers, for example:
    for (int i = 0; i < array_size; ++i) {
        numbers[i] = static_cast<float>(i + 1);
    }

    // Run multiple test runs of each algorithm and time it.
    for (int i = 0; i < 10; i++) {
        std::cout << "Run number " << i + 1 << std::endl;
        // #1
        // Paralllel Quake: Start timing
        auto start = std::chrono::high_resolution_clock::now();
        parallel_q_rsqrt(numbers.data(), results.data(), array_size);
        // Stop timing
        auto stop = std::chrono::high_resolution_clock::now();
        // Calculate duration
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        // Print the results
        std::cout << "Parallel Quake 3, Execution time: " << duration.count() << " milliseconds" << std::endl;


        // #2
        // Quake: Start timing
        start = std::chrono::high_resolution_clock::now();
        for(int i=0; i<array_size; i++) {
            q_rsqrt(numbers[i]);
        }
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Sequential Quake 3, Execution time: " << duration.count() << " milliseconds" << std::endl;


        // #3
        // math.h modern library: Start timing
        start = std::chrono::high_resolution_clock::now();
        for(int i=0; i<array_size; i++) {
            manual_inv_sqrt(numbers[i]);
        }
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Manual Sqrt, Execution time: " << duration.count() << " milliseconds" << std::endl;


        // #4
        // math.h modern library: Start timing
        start = std::chrono::high_resolution_clock::now();
        for(int i=0; i<array_size; i++) {
            modern_mathH(numbers[i]);
        }
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Modern math.h library sqrt, Execution time: " << duration.count() << " milliseconds" << std::endl;
    }
    return 0;
}