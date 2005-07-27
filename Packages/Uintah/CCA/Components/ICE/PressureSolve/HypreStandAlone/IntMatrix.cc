#include "IntMatrix.h"
#include <iostream>
using namespace std;

IntMatrix::IntMatrix(const Counter rows, 
                     const Counter cols)
  : rows(rows), cols(cols)
{
  assert(rows>0);
  assert(cols>0);
  mat = new int[rows*cols];
}

IntMatrix::~IntMatrix(void)
{
  if (mat) delete[] mat;
}

void
IntMatrix::transpose(const IntMatrix& a)
{
  assert(cols == a.rows);
  assert(rows == a.cols);
  // Cache-coherent on writes.  We could do better by blocking, but
  // This should be good enough for small matrices, which is what this
  // class is intended for
  for(Counter row=0;row<rows; row++){
    for(Counter col=0;col<cols; col++){
      this->operator()(row,col) = a(col,row);
    }
  }
}

/*---------------------------------------------------------------------
  Function~  MatrixMultiplication--
  Reference~  This multiplies matrix (this) and vector (b) and returns X
  ---------------------------------------------------------------------  */
void
IntMatrix::multiply(const vector<int>& b,
                    vector<int>& X) const
{
  assert(cols == b.size());
  assert(rows == X.size());
  for (Counter row=0; row<rows; row++) {
    int sum=0;
    for (Counter col=0; col<cols; col++) {
      sum += this->operator()(row,col) * b[col];
    }
    X[row]=sum;
  }
}

void
IntMatrix::multiply(const int* b,
                    int* X) const
{
  for (Counter row=0; row<rows; row++) {
    int sum=0;
    for (Counter col=0; col<cols; col++) {
      sum += this->operator()(row,col) * b[col];
    }
    X[row]=sum;
  }
}

// this = a*b;
void
IntMatrix::multiply(const IntMatrix& a,
                    const IntMatrix& b)
{
  assert(rows == a.rows);
  assert(cols == b.cols);
  assert(a.cols == b.rows);
  Counter s=a.cols;
  for(Counter i=0;i<rows;i++){
    for(Counter j=0;j<cols;j++){
      int sum=0;
      for(Counter k=0;k<s;k++){
	sum+=a(i,k)*b(k,j);
      }
      this->operator()(i,j)=sum;
    }
  }
}

void
IntMatrix::multiply(const int s)
{
  for(Counter i = 0; i < rows*cols; i++) {
    mat[i] *= s;
  }
}

void
IntMatrix::identity(void)
{
  assert(rows == cols);
  zero();
  for(Counter i=0;i<rows;i++)
    this->operator()(i,i) = 1;
}

void
IntMatrix::zero(void)
{
  for(Counter i = 0; i < rows*cols; i++) {
    mat[i] = 0;
  }
}

void
IntMatrix::copy(const IntMatrix& a)
{
  assert(rows == a.rows);
  assert(cols == a.cols);
  //Counter size=rows*cols;
  for(Counter i=0;i<rows;i++){
    for(Counter j=0;j<cols;j++)
      this->operator()(i,j) = a(i,j);
  }
}

void
IntMatrix::print(ostream& out)
{
  for(Counter i=0;i<rows;i++){
    out << i << ":";
    for(Counter j=0;j<cols;j++){
      out << '\t' << this->operator()(i,j);
    }
    out << '\n';
  }
}

IntMatrix&
IntMatrix::operator = (const IntMatrix& other)
{
  rows = other.rows;
  cols = other.cols;
  //Counter size=rows*cols;
  if (mat) {
    delete[] mat;
    mat = new int[rows*cols];
  }
  for(Counter i = 0; i < rows*cols; i++) {
    mat[i] = other.mat[i];
  }
  return *this;
}

IntMatrix::IntMatrix(const IntMatrix& other)
{
  rows = other.rows;
  cols = other.cols;
  //Counter size=rows*cols;
  if (mat) {
    delete[] mat;
    mat = new int[rows*cols];
  }
  for(Counter i = 0; i < rows*cols; i++) {
    mat[i] = other.mat[i];
  }
}
