
#ifndef _testpcaunit_h
#define _testpcaunit_h

#include "ap.h"
#include "ialglib.h"

#include "reflections.h"
#include "bidiagonal.h"
#include "qr.h"
#include "lq.h"
#include "blas.h"
#include "rotations.h"
#include "bdsvd.h"
#include "svd.h"
#include "descriptivestatistics.h"
#include "pca.h"


bool testpca(bool silent);


/*************************************************************************
Silent unit test
*************************************************************************/
bool testpcaunit_test_silent();


/*************************************************************************
Unit test
*************************************************************************/
bool testpcaunit_test();


#endif

