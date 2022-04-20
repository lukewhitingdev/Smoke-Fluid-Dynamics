#include <iostream>
#include "solver.c"

float* u;
float* v;

float* u0;
float* v0;

float* d;
float* d0;

int main()
{
    const float N = 4;
    const float arrSize = (N + 2) * (N + 2);

    const float visc = 0.0f;
    const float diff = 0.0f;

    const float dt = 0.1f;

    u = new float[arrSize];
    v = new float[arrSize];

    u0 = new float[arrSize];
    v0 = new float[arrSize];

    d = new float[arrSize];
    d0 = new float[arrSize];

    for(int i = 0; i < arrSize; i++)
    {
        u[i] = 0;
        v[i] = 0;
        v0[i] = 0;
        u0[i] = 0;
        d[i] = 0;
        d0[i] = 0;
    }

    int x = 1;
    int y = 1;

    int index = ((x)+(N + 2) * (y));

    d0[index] = 100.0f;
    v0[index] = 10.0f;
    u0[index] = 1.0f;

   while(true)
    {

       // Make sure to reset the current frames values so they dont concatenate.
       for (int i = 0; i < arrSize; i++)
       {
           u[i] = 0;
           v[i] = 0;
           d[i] = 0;
       }

       // Add forces.
       d0[index] = 100.0f;

       // Simulate
        vel_step(N, u, v, u0, v0, visc, dt);
        dens_step(N, d, d0, u, v, diff, dt);

        // Repeat.
     

        // Print
        for(int y = 0; y < N; ++y)
        {
            for (int x = 0; x < N; ++x)
            {
                int index = ((x)+(N + 2) * (y));
                printf(" [%f] [%f, %f] ", d[index], u[index], v[index]);
            }
            printf("\n");
        }
        printf("\n");
    }

    std::cout << "Hello World!\n";
}
