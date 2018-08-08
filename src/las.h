#ifndef LAS_H_
#define LAS_H_
#include "lasComm.h"
#include "lasSys.h"
namespace las
{
  /**
   * The opaque type for all matrices in LAS.
   */
  class Mat;
  /**
   * The opaque type for all vectors in LAS.
   */
  class Vec;
  /**
   * The opaque type for all nonzero sparsity patterns in LAS.
   */
  class Sparsity;
  /**
   * The primary interface for low-level LAS operations.
   *  Each operation in this interface can be inlined
   *  when using optimized compilation, as the backend is
   *  known at compile-time (CRTP + inlining). That makes
   *  these operations as efficient as possible with the
   *  given backend, so they are safe to use in tight
   *  loops.
   */
  template <class T>
  class LasOps
  {
  public:
    void zero(Mat * m)
    {
      static_cast<T*>(this)->_zero(m);
    }
    void zero(Vec * v)
    {
      static_cast<T*>(this)->_zero(v);
    }
    void zero(Mat * v, int rw)
    {
      static_cast<T*>(this)->_zero(v,rw);
    }
    void assemble(Vec * v, int cnt, int * rws, scalar * vls)
    {
      static_cast<T*>(this)->_assemble(v,cnt,rws,vls);
    }
    void assemble(Mat * m, int cntr, int * rws, int cntc, int * cls, scalar * vls)
    {
      static_cast<T*>(this)->_assemble(m,cntr,rws,cntc,cls,vls);
    }
    void set(Vec * v, int cnt, int * rws, scalar * vls)
    {
      static_cast<T*>(this)->_set(v,cnt,rws,vls);
    }
    void set(Mat * m, int cntr, int * rws, int cntc, int * cls, scalar * vls)
    {
      static_cast<T*>(this)->_set(m,cntr,rws,cntc,cls,vls);
    }
    /**
     * @note vls is allocated inside this function, the user
     *       is responsible for destroying the array after use
     */
    void get(Vec * v, int cntr, int * rws, scalar ** vls)
    {
      static_cast<T*>(this)->_get(v,cntr,rws,vls);
    }
    /**
     * @note vls is allocated inside this function, the user
     *       is responsible for destroying the array after use
     */
    void get(Mat * m, int cntr, int * rws, int cntc, int * cls, scalar ** vls)
    {
      static_cast<T*>(this)->_get(m,cntr,rws,cntc,cls,vls);
    }
    scalar norm(Vec * v)
    {
      return static_cast<T*>(this)->_norm(v);
    }
    scalar dot(Vec * v0, Vec * v1)
    {
      return static_cast<T*>(this)->_dot(v0,v1);
    }
    void axpy(scalar a, Vec * x, Vec * y)
    {
      static_cast<T*>(this)->_axpy(a,x,y);
    }
    void get(Vec * v, scalar *& vls)
    {
      static_cast<T*>(this)->_get(v,vls);
    }
    void restore(Vec * v, scalar *& vls)
    {
      static_cast<T*>(this)->_restore(v,vls);
    }
  };
  /**
   * A generic matrix factory interface. Subclasses of
   *  this may or may not actually use all the arguments
   *  supplied, but some backends (like PETSc) use all of
   *  them.
   */
  class LasCreateMat
  {
  public:
    virtual ~LasCreateMat() {}
    /**
     * Create a matrix.
     * @param lcl The local number of rows (per-process in cm)
     * @param bs  The block size (should be the same over cm)
     * @param s   A Sparsity object for the particular backend a
     *            subclass of this interface is implementing.
     * @param cm  The comm over which the matrix is collective.
     * @note Any of the arguments may be required to be LAS_NULL for
     *       a particular backend, as they may be unused for that backend.
     * @todo Allow creation of non-square matrices
     */
    virtual Mat * create(unsigned lcl, unsigned bs, Sparsity * s, MPI_Comm cm) = 0;
    virtual void destroy(Mat * m) = 0;
  };
  /**
   * A generic vector factory interface. Subclasses of
   *  this may or may not actually use all the arguments
   *  supplied, and may not implement createRHS or createLHS.
   */
  class LasCreateVec
  {
  public:
    virtual ~LasCreateVec() {}
    /**
     * Create a vector.
     * @param lcl The local number of rows (per-process in cm)
     * @param bs The block size (should be the same over cm)
     * @param cm The comm over which the vector is collective.
     */
    virtual Vec * create(unsigned lcl, unsigned bs, MPI_Comm cm) = 0;
    virtual void destroy(Vec * v) = 0;
    /**
     * Create a vector suitable to act as the RHS vector to a
     *  supplied matrix.
     */
    virtual Vec * createRHS(Mat * m);
    /**
     * Create a vector suitable to act as the LH"S vector to a
     *  supplied matrix.
     */
    virtual Vec * createLHS(Mat * m);
  };
  /**
   * Get the operations object for a specific backend.
   *  Each backend defines an opaque class identifying
   *  the backend, this function is specialized
   *  by each backend using the identifying backend class.
   */
  template <typename T>
  LasOps<T> * getLASOps();
  /**
   * Get a factory to create a matrix, this
   *  function is also specialized for each backend,
   *  the id parameter currently doesn't do anything,
   *  but is there in case a backend may have many
   *  different matrix creation factories/algorithms.
   */
  template <typename T>
  LasCreateMat * getMatBuilder(int id);
  /**
   * Same as getMatBuilder but for vectors.
   */
  template <typename T>
  LasCreateVec * getVecBuilder(int id);
  /**
   * Interface for solving a linear system.
   * @todo Retrieve backend-specific solvers using
   *       backend id classes to do template
   *       specialization, as above.
   */
  class Solve
  {
  public:
    virtual void solve(Mat * k, Vec * u, Vec * f) = 0;
    virtual ~Solve() {}
  };
  /**
   * Interface for Matrix-Vector multiplication
   * @todo Retrieve backend-specific solvers using
   *       backend id classes to do template
   *       specialization, as above.
   */
  class MatVecMult
  {
  public:
    virtual void exec(Mat * x, Vec * a, Vec * b) = 0;
    virtual ~MatVecMult() {}
  };
  /**
   * Interface for Matrix-Matrix multiplication
   * @todo Retrieve backend-specific solvers using
   *       backend id classes to do template
   *       specialization, as above.
   */
  class MatMatMult
  {
  public:
    virtual void exec(Mat * a, Mat * b, Mat ** c) = 0;
    virtual ~MatMatMult() {}
  };
  template <class T>
  void destroySparsity(Sparsity * s);
}
#endif
