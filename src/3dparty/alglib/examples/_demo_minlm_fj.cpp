#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "minlm.h"

int main(int argc, char **argv)
{
    lmstate state;
    lmreport rep;
    ap::real_1d_array s;
    double x;
    double y;

    
    //
    // Example of solving simple task using FJ scheme.
    //
    // Function minimized:
    //     F = (x-2*y)^2 + (x-2)^2 + (y-1)^2
    // exact solution is (2,1).
    //
    s.setlength(2);
    s(0) = ap::randomreal()-0.5;
    s(1) = ap::randomreal()-0.5;
    minlmfj(2, 3, s, 0.0, 0.001, 0, state);
    while(minlmiteration(state))
    {
        x = state.x(0);
        y = state.x(1);
        if( state.needf )
        {
            state.f = ap::sqr(x-2*y)+ap::sqr(x-2)+ap::sqr(y-1);
        }
        if( state.needfij )
        {
            state.fi(0) = x-2*y;
            state.fi(1) = x-2;
            state.fi(2) = y-1;
            state.j(0,0) = 1;
            state.j(0,1) = -2;
            state.j(1,0) = 1;
            state.j(1,1) = 0;
            state.j(2,0) = 0;
            state.j(2,1) = 1;
        }
    }
    minlmresults(state, s, rep);
    
    //
    // output results
    //
    printf("X = %4.2lf (correct value - 2.00)\n",
        double(s(0)));
    printf("Y = %4.2lf (correct value - 1.00)\n",
        double(s(1)));
    printf("TerminationType = %0ld (should be 2 - stopping when step is small enough)\n",
        long(rep.terminationtype));
    printf("NFunc = %0ld\n",
        long(rep.nfunc));
    printf("NJac  = %0ld (should be 2 - task is very simple)\n",
        long(rep.njac));
    printf("NGrad = %0ld\n",
        long(rep.ngrad));
    printf("NHess = %0ld\n",
        long(rep.nhess));
    return 0;
}

