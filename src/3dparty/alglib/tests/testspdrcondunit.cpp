
#include <stdafx.h>
#include <stdio.h>
#include "testspdrcondunit.h"

static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b);
static bool invmattr(ap::real_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular);
static bool invmatlu(ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n);
static void matlu(ap::real_2d_array& a,
     int m,
     int n,
     ap::integer_1d_array& pivots);
static bool invmat(ap::real_2d_array& a, int n);
static void refrcond(const ap::real_2d_array& a,
     int n,
     double& rc1,
     double& rcinf);

bool testrcondcholesky(bool silent)
{
    bool result;
    ap::real_2d_array l;
    ap::real_2d_array a;
    ap::real_2d_array a2;
    int minij;
    ap::integer_1d_array p;
    int n;
    int maxn;
    int i;
    int j;
    int pass;
    int passcount;
    bool waserrors;
    bool err50;
    bool err90;
    bool errless;
    double erc1;
    double ercinf;
    ap::real_1d_array q50;
    ap::real_1d_array q90;
    double v;
    double threshold50;
    double threshold90;
    int htask;
    bool upperin;

    waserrors = false;
    err50 = false;
    err90 = false;
    errless = false;
    maxn = 10;
    passcount = 100;
    threshold50 = 0.5;
    threshold90 = 0.1;
    q50.setbounds(0, 1);
    q90.setbounds(0, 1);
    
    //
    // process
    //
    for(n = 1; n <= maxn; n++)
    {
        l.setbounds(0, n-1, 0, n-1);
        a.setbounds(0, n-1, 0, n-1);
        a2.setbounds(0, n-1, 0, n-1);
        for(i = 0; i <= 1; i++)
        {
            q50(i) = 0;
            q90(i) = 0;
        }
        for(htask = 0; htask <= 1; htask++)
        {
            for(pass = 1; pass <= passcount; pass++)
            {
                upperin = htask==0;
                
                //
                // Prepare task:
                // * A contains full matrix A
                // * L contains its Cholesky factor (upper or lower)
                // * A2 contains upper/lower triangle of A
                //
                for(i = 0; i <= n-1; i++)
                {
                    for(j = i+1; j <= n-1; j++)
                    {
                        l(i,j) = 2*ap::randomreal()-1;
                        l(j,i) = l(i,j);
                    }
                    l(i,i) = 1.1+ap::randomreal();
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        minij = ap::minint(i, j);
                        v = ap::vdotproduct(l.getrow(i, 0, minij), l.getcolumn(j, 0, minij));
                        a(i,j) = v;
                        a2(i,j) = v;
                    }
                }
                for(i = 0; i <= n-1; i++)
                {
                    for(j = 0; j <= n-1; j++)
                    {
                        if( upperin )
                        {
                            if( j<i )
                            {
                                l(i,j) = 0;
                                a2(i,j) = 0;
                            }
                        }
                        else
                        {
                            if( i<j )
                            {
                                l(i,j) = 0;
                                a2(i,j) = 0;
                            }
                        }
                    }
                }
                refrcond(a, n, erc1, ercinf);
                
                //
                // normal
                //
                v = 1/spdmatrixrcond(a, n, upperin);
                if( ap::fp_greater_eq(v,threshold50*erc1) )
                {
                    q50(0) = q50(0)+double(1)/double(passcount);
                }
                if( ap::fp_greater_eq(v,threshold90*erc1) )
                {
                    q90(0) = q90(0)+double(1)/double(passcount);
                }
                errless = errless||ap::fp_greater(v,erc1*1.001);
                
                //
                // Cholesky
                //
                v = 1/spdmatrixcholeskyrcond(l, n, upperin);
                if( ap::fp_greater_eq(v,threshold50*erc1) )
                {
                    q50(1) = q50(1)+double(1)/double(passcount);
                }
                if( ap::fp_greater_eq(v,threshold90*erc1) )
                {
                    q90(1) = q90(1)+double(1)/double(passcount);
                }
                errless = errless||ap::fp_greater(v,erc1*1.001);
            }
        }
        for(i = 0; i <= 1; i++)
        {
            err50 = err50||ap::fp_less(q50(i),0.50);
            err90 = err90||ap::fp_less(q90(i),0.90);
        }
    }
    
    //
    // report
    //
    waserrors = err50||err90||errless;
    if( !silent )
    {
        printf("TESTING RCOND (SYMMETRIC)\n");
        printf("50%% within [0.5*cond,cond]:              ");
        if( err50||errless )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        printf("90%% within [0.1*cond,cond]               ");
        if( err90||errless )
        {
            printf("FAILED\n");
        }
        else
        {
            printf("OK\n");
        }
        if( waserrors )
        {
            printf("TEST FAILED\n");
        }
        else
        {
            printf("TEST PASSED\n");
        }
        printf("\n\n");
    }
    result = !waserrors;
    return result;
}


/*************************************************************************
Copy
*************************************************************************/
static void makeacopy(const ap::real_2d_array& a,
     int m,
     int n,
     ap::real_2d_array& b)
{
    int i;
    int j;

    b.setbounds(0, m-1, 0, n-1);
    for(i = 0; i <= m-1; i++)
    {
        for(j = 0; j <= n-1; j++)
        {
            b(i,j) = a(i,j);
        }
    }
}


/*************************************************************************
triangular inverse
*************************************************************************/
static bool invmattr(ap::real_2d_array& a,
     int n,
     bool isupper,
     bool isunittriangular)
{
    bool result;
    bool nounit;
    int i;
    int j;
    double v;
    double ajj;
    ap::real_1d_array t;

    result = true;
    t.setbounds(0, n-1);
    
    //
    // Test the input parameters.
    //
    nounit = !isunittriangular;
    if( isupper )
    {
        
        //
        // Compute inverse of upper triangular matrix.
        //
        for(j = 0; j <= n-1; j++)
        {
            if( nounit )
            {
                if( ap::fp_eq(a(j,j),0) )
                {
                    result = false;
                    return result;
                }
                a(j,j) = 1/a(j,j);
                ajj = -a(j,j);
            }
            else
            {
                ajj = -1;
            }
            
            //
            // Compute elements 1:j-1 of j-th column.
            //
            if( j>0 )
            {
                ap::vmove(t.getvector(0, j-1), a.getcolumn(j, 0, j-1));
                for(i = 0; i <= j-1; i++)
                {
                    if( i<j-1 )
                    {
                        v = ap::vdotproduct(&a(i, i+1), &t(i+1), ap::vlen(i+1,j-1));
                    }
                    else
                    {
                        v = 0;
                    }
                    if( nounit )
                    {
                        a(i,j) = v+a(i,i)*t(i);
                    }
                    else
                    {
                        a(i,j) = v+t(i);
                    }
                }
                ap::vmul(a.getcolumn(j, 0, j-1), ajj);
            }
        }
    }
    else
    {
        
        //
        // Compute inverse of lower triangular matrix.
        //
        for(j = n-1; j >= 0; j--)
        {
            if( nounit )
            {
                if( ap::fp_eq(a(j,j),0) )
                {
                    result = false;
                    return result;
                }
                a(j,j) = 1/a(j,j);
                ajj = -a(j,j);
            }
            else
            {
                ajj = -1;
            }
            if( j<n-1 )
            {
                
                //
                // Compute elements j+1:n of j-th column.
                //
                ap::vmove(t.getvector(j+1, n-1), a.getcolumn(j, j+1, n-1));
                for(i = j+1; i <= n-1; i++)
                {
                    if( i>j+1 )
                    {
                        v = ap::vdotproduct(&a(i, j+1), &t(j+1), ap::vlen(j+1,i-1));
                    }
                    else
                    {
                        v = 0;
                    }
                    if( nounit )
                    {
                        a(i,j) = v+a(i,i)*t(i);
                    }
                    else
                    {
                        a(i,j) = v+t(i);
                    }
                }
                ap::vmul(a.getcolumn(j, j+1, n-1), ajj);
            }
        }
    }
    return result;
}


/*************************************************************************
LU inverse
*************************************************************************/
static bool invmatlu(ap::real_2d_array& a,
     const ap::integer_1d_array& pivots,
     int n)
{
    bool result;
    ap::real_1d_array work;
    int i;
    int iws;
    int j;
    int jb;
    int jj;
    int jp;
    double v;

    result = true;
    
    //
    // Quick return if possible
    //
    if( n==0 )
    {
        return result;
    }
    work.setbounds(0, n-1);
    
    //
    // Form inv(U)
    //
    if( !invmattr(a, n, true, false) )
    {
        result = false;
        return result;
    }
    
    //
    // Solve the equation inv(A)*L = inv(U) for inv(A).
    //
    for(j = n-1; j >= 0; j--)
    {
        
        //
        // Copy current column of L to WORK and replace with zeros.
        //
        for(i = j+1; i <= n-1; i++)
        {
            work(i) = a(i,j);
            a(i,j) = 0;
        }
        
        //
        // Compute current column of inv(A).
        //
        if( j<n-1 )
        {
            for(i = 0; i <= n-1; i++)
            {
                v = ap::vdotproduct(&a(i, j+1), &work(j+1), ap::vlen(j+1,n-1));
                a(i,j) = a(i,j)-v;
            }
        }
    }
    
    //
    // Apply column interchanges.
    //
    for(j = n-2; j >= 0; j--)
    {
        jp = pivots(j);
        if( jp!=j )
        {
            ap::vmove(work.getvector(0, n-1), a.getcolumn(j, 0, n-1));
            ap::vmove(a.getcolumn(j, 0, n-1), a.getcolumn(jp, 0, n-1));
            ap::vmove(a.getcolumn(jp, 0, n-1), work.getvector(0, n-1));
        }
    }
    return result;
}


/*************************************************************************
LU decomposition
*************************************************************************/
static void matlu(ap::real_2d_array& a,
     int m,
     int n,
     ap::integer_1d_array& pivots)
{
    int i;
    int j;
    int jp;
    ap::real_1d_array t1;
    double s;

    pivots.setbounds(0, ap::minint(m-1, n-1));
    t1.setbounds(0, ap::maxint(m-1, n-1));
    ap::ap_error::make_assertion(m>=0&&n>=0, "Error in LUDecomposition: incorrect function arguments");
    
    //
    // Quick return if possible
    //
    if( m==0||n==0 )
    {
        return;
    }
    for(j = 0; j <= ap::minint(m-1, n-1); j++)
    {
        
        //
        // Find pivot and test for singularity.
        //
        jp = j;
        for(i = j+1; i <= m-1; i++)
        {
            if( ap::fp_greater(fabs(a(i,j)),fabs(a(jp,j))) )
            {
                jp = i;
            }
        }
        pivots(j) = jp;
        if( ap::fp_neq(a(jp,j),0) )
        {
            
            //
            //Apply the interchange to rows
            //
            if( jp!=j )
            {
                ap::vmove(&t1(0), &a(j, 0), ap::vlen(0,n-1));
                ap::vmove(&a(j, 0), &a(jp, 0), ap::vlen(0,n-1));
                ap::vmove(&a(jp, 0), &t1(0), ap::vlen(0,n-1));
            }
            
            //
            //Compute elements J+1:M of J-th column.
            //
            if( j<m )
            {
                jp = j+1;
                s = 1/a(j,j);
                ap::vmul(a.getcolumn(j, jp, m-1), s);
            }
        }
        if( j<ap::minint(m, n)-1 )
        {
            
            //
            //Update trailing submatrix.
            //
            jp = j+1;
            for(i = j+1; i <= m-1; i++)
            {
                s = a(i,j);
                ap::vsub(&a(i, jp), &a(j, jp), ap::vlen(jp,n-1), s);
            }
        }
    }
}


/*************************************************************************
Matrix inverse
*************************************************************************/
static bool invmat(ap::real_2d_array& a, int n)
{
    bool result;
    ap::integer_1d_array pivots;

    matlu(a, n, n, pivots);
    result = invmatlu(a, pivots, n);
    return result;
}


/*************************************************************************
reference RCond
*************************************************************************/
static void refrcond(const ap::real_2d_array& a,
     int n,
     double& rc1,
     double& rcinf)
{
    ap::real_2d_array inva;
    double nrm1a;
    double nrminfa;
    double nrm1inva;
    double nrminfinva;
    double v;
    int k;
    int i;

    
    //
    // inv A
    //
    makeacopy(a, n, n, inva);
    if( !invmat(inva, n) )
    {
        rc1 = 0;
        rcinf = 0;
        return;
    }
    
    //
    // norm A
    //
    nrm1a = 0;
    nrminfa = 0;
    for(k = 0; k <= n-1; k++)
    {
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+fabs(a(i,k));
        }
        nrm1a = ap::maxreal(nrm1a, v);
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+fabs(a(k,i));
        }
        nrminfa = ap::maxreal(nrminfa, v);
    }
    
    //
    // norm inv A
    //
    nrm1inva = 0;
    nrminfinva = 0;
    for(k = 0; k <= n-1; k++)
    {
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+fabs(inva(i,k));
        }
        nrm1inva = ap::maxreal(nrm1inva, v);
        v = 0;
        for(i = 0; i <= n-1; i++)
        {
            v = v+fabs(inva(k,i));
        }
        nrminfinva = ap::maxreal(nrminfinva, v);
    }
    
    //
    // result
    //
    rc1 = nrm1inva*nrm1a;
    rcinf = nrminfinva*nrminfa;
}


/*************************************************************************
Silent unit test
*************************************************************************/
bool testspdrcondunit_test_silent()
{
    bool result;

    result = testrcondcholesky(true);
    return result;
}


/*************************************************************************
Unit test
*************************************************************************/
bool testspdrcondunit_test()
{
    bool result;

    result = testrcondcholesky(false);
    return result;
}




