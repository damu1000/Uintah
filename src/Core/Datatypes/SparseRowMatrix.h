
/*
 *  SparseRowMatrix.h:  Sparse Row Matrices
 *
 *  Written by:
 *   Steven G. Parker
 *   Department of Computer Science
 *   University of Utah
 *   November 1994
 *
 *  Copyright (C) 1994 SCI Group
 */


#ifndef SCI_project_SparseRowMatrix_h
#define SCI_project_SparseRowMatrix_h 1

#include <SCICore/Datatypes/Matrix.h>
#include <SCICore/Containers/Array1.h>

namespace SCICore {
namespace Datatypes {

using SCICore::Containers::Array1;

class AddMatrices;
class SCICORESHARE SparseRowMatrix : public Matrix {
    int nnrows;
    int nncols;
    double dummy;
    double minVal;
    double maxVal;
protected:
public:
    int* columns;
    int* rows;
    int nnz;
    double* a;
    SparseRowMatrix();
    SparseRowMatrix(int, int, Array1<int>&, Array1<int>&);
    SparseRowMatrix(int, int, int*, int*, int, double*);
    SparseRowMatrix(int, int, int*, int*, int);
    virtual ~SparseRowMatrix();
    SparseRowMatrix(const SparseRowMatrix&);
    SparseRowMatrix& operator=(const SparseRowMatrix&);

    void transpose( SparseRowMatrix &);
    virtual double& get(int, int);
    int getIdx(int, int);
    virtual void put(int, int, const double&);
    virtual void add(int, int, const double&);
    virtual int nrows() const;
    virtual int ncols() const;
    virtual double minValue();
    virtual double maxValue();
    double density();
    virtual void getRowNonzeros(int r, Array1<int>& idx, Array1<double>& val);
    virtual void solve(ColumnMatrix&);
    virtual void zero();
    virtual void mult(const ColumnMatrix& x, ColumnMatrix& b,
		      int& flops, int& memrefs, int beg=-1, int end=-1,
		      int spVec=0) const;
    virtual void mult_transpose(const ColumnMatrix& x, ColumnMatrix& b,
				int& flops, int& memrefs, int beg=-1, 
				int end=-1, int spVec=0);
    virtual void print();
    MatrixRow operator[](int r);
    friend class AddMatrices;
    virtual double* get_val(){return a;}
    virtual int* get_row(){return rows;}
    virtual int* get_col(){return columns;}
  int get_nnz() { return nnz; }
    // Persistent representation...
    virtual void io(Piostream&);
    static PersistentTypeID type_id;
};

} // End namespace Datatypes
} // End namespace SCICore

#endif
