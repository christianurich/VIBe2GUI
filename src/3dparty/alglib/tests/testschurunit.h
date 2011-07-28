
#ifndef _testschurunit_h
#define _testschurunit_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"
#include "hessenberg.h"
#include "blas.h"
#include "rotations.h"
#include "hsschur.h"
#include "schur.h"


/*************************************************************************
Testing Schur decomposition subroutine
*************************************************************************/
bool testschur(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testschurunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testschurunit_test();


#endif

