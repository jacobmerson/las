#ifndef LAS_CUSPARSE_H_
#define LAS_CUSPARSE_H_
#include "lasSparse.h"
#include "lasAlloc.h"
namespace las
{
  typedef sparse cusparse;
  LasOps<cusparse> * getLASOps();
  template <>
  LasCreateMat * getMatBuilder<cusparse>(int id);
  template <>
  LasCreateVec * getVecBuilder<cusparse>(int id);
  Solve * createCuSparseSolve();
  MatVecMult * createCuMatVecMult();
  MatMatMult * createCuCsrMatCsrMatMult();
  MatMatMult * createCuCsrMatDMatMult();
}
#include "lasCuSparse_impl.h"
#endif
