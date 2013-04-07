/*
 * dwt.c
 *
 *  Created on: Jul 4, 2012
 *      Author: robert
 */
#include "dwt_init.h"

/* Checks for correct # of input variables based on type of transform. */
int dwtInputCheck(int nrhs, int dwtType) {
  if (dwtType == INVERSE_REDUNDANT_DWT) {
    if (nrhs > 4){
      mexErrMsgTxt("There are at most 4 input parameters allowed!");
      return 1;
    }
    if (nrhs < 3){
      mexErrMsgTxt("There are at least 3 input parameters required!");
      return 1;
    }
  }
  else {
    if (nrhs > 3){
      mexErrMsgTxt("There are at most 3 input parameters allowed!");
      return 1;
    }
    if (nrhs < 2){
      mexErrMsgTxt("There are at least 2 input parameters required!");
      return 1;
    }
  }
  return 0;
}

int dwtEstimateL(int n, int m) {
  int i, j, L;
  i = n ; j = 0;
  while (even(i)) {
    i = (i >> 1);
    j++;
  }
  L = m; i = 0;
  while (even(L)) {
    L = (L >> 1);
    i++;
  }
  if (min(m, n) == 1)
    L = max(i, j);
  else
    L = min(i, j);
  if (L == 0){
    mexErrMsgTxt("Maximum number of levels is zero; no decomposition can be performed!");
    return -1;
  }
  else return L;
}

rwt_init_params dwtInit(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], int dwtType) {
  rwt_init_params params;
  int mh, nh, h_col, h_row, dim, argNumL;
  double mtest, ntest;

  /* check for correct # of input variables */
  if (dwtInputCheck(nrhs, dwtType) != 0) return;

  /* buffer overflow will occur if matrix isn't 1-D or 2-D */
  dim = mxGetNumberOfDimensions(prhs[0]);
  if (dim > 2) {
    mexErrMsgTxt("Matrix must have fewer than 3 dimensions!");
    return;
  }

  /* Get input matrix row and column number */
  params.m = mxGetM(prhs[0]);
  params.n = mxGetN(prhs[0]);

  /* Read L from command line or compute L */
  argNumL = 2;
  if (dwtType == INVERSE_REDUNDANT_DWT) argNumL += 1;
  if ((argNumL + 1) == nrhs) {
    params.L = (int) *mxGetPr(prhs[argNumL]);
  }
  else params.L = dwtEstimateL(params.n, params.m);
  if (params.L < 0) {
    mexErrMsgTxt("The number of levels, L, must be a non-negative integer");
    return;
  }

  if (dwtType == INVERSE_REDUNDANT_DWT) {
    nh = mxGetN(prhs[1]);
    mh = mxGetM(prhs[1]);
    params.h = mxGetPr(prhs[2]);
    h_col = mxGetN(prhs[2]);
    h_row = mxGetM(prhs[2]);
    /* check for consistency of rows and columns of yl, yh */
    if (min(params.m, params.n) > 1){
      if((params.m != mh) | (3*params.n*params.L != nh)){
        mexErrMsgTxt("Dimensions of first two input matrices not consistent!");
        return;
      }
    }
    else{
      if((params.m != mh) | (params.n*params.L != nh)){
        mexErrMsgTxt("Dimensions of first two input vectors not consistent!");{
          return;
        }
      }
    }
  }
  else {
    params.h = mxGetPr(prhs[1]);
    h_col = mxGetN(prhs[1]);
    h_row = mxGetM(prhs[1]);
  }

  if (h_col > h_row)
    params.lh = h_col;
  else
    params.lh = h_row;

  /* Check the ROW dimension of input */
  if (params.m > 1) {
    mtest = (double) params.m / pow(2.0, (double) params.L);
    if (!isint(mtest))
      mexErrMsgTxt("The matrix row dimension must be of size m*2^(L)");
  }
  /* Check the COLUMN dimension of input */
  if (params.n > 1) {
    ntest = (double) params.n / pow(2.0, (double) params.L);
    if (!isint(ntest))
      mexErrMsgTxt("The matrix column dimension must be of size n*2^(L)");
  }
  plhs[0] = mxCreateDoubleMatrix(params.m, params.n, mxREAL);

  return params;
}
