
#ifndef _testgkq_h
#define _testgkq_h

#include "ap.h"
#include "ialglib.h"

#include "tsort.h"
#include "blas.h"
#include "rotations.h"
#include "tdevd.h"
#include "gammafunc.h"
#include "gq.h"
#include "gkq.h"


/*************************************************************************
Test
*************************************************************************/
bool testgkqunit(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testgkq_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testgkq_test();


#endif

