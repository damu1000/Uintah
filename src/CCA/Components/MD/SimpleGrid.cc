/*
 * The MIT License
 *
 * Copyright (c) 1997-2012 The University of Utah
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <CCA/Components/MD/SimpleGrid.h>
#include <Core/Math/UintahMiscMath.h>
#include <Core/Geometry/BBox.h>
#include <Core/Math/MiscMath.h>

#include <sci_values.h>

using namespace Uintah;
using namespace SCIRun;

template<class T>
SimpleGrid<T>::SimpleGrid()
{

}

template<class T>
SimpleGrid<T>::SimpleGrid(const IntVector& extents,
                          const IntVector& offset,
                          const int numGhostCells) :
    d_gridExtents(extents), d_gridOffset(extents), d_numGhostCells(numGhostCells)
{

}

template<class T>
SimpleGrid<T>::~SimpleGrid()
{

}

template<class T>
bool SimpleGrid<T>::verifyRegistration(SimpleGrid<T>& gridIn)
{
  if ((this->d_gridExtents != gridIn.d_gridExtents) || (this->d_gridOffset != gridIn.d_gridOffset)
      || (this->d_numGhostCells != gridIn.d_numGhostCells)) {
    ostringstream ostr;
    ostr << "MD SimpleGrids differ in extent, offset or number of ghost cells.";
    throw SCIRun::InternalError(ostr.str(), __FILE__, __LINE__);
  } else {
    return true;
  }
}

template<class T>
SimpleGrid<T> SimpleGrid<T>::operator*(const SimpleGrid<T>& gridIn)
{
  SimpleGrid sg();
  for (unsigned int x = 0; x < d_gridExtents.x(); ++x) {
    for (unsigned int y = 0; y < d_gridExtents.y(); ++y) {
      for (unsigned int z = 0; z < d_gridExtents.z(); ++z) {
        sg[x][y][z] *= gridIn.d_charges[x][y][z];
      }
    }
  }
  return sg;
}

template<class T>
SimpleGrid<T> SimpleGrid<T>::operator+(const SimpleGrid<T>& gridIn)
{
  SimpleGrid sg();
  for (unsigned int x = 0; x < d_gridExtents.x(); ++x) {
    for (unsigned int y = 0; y < d_gridExtents.y(); ++y) {
      for (unsigned int z = 0; z < d_gridExtents.z(); ++z) {
        sg[x][y][z] += gridIn.d_charges[x][y][z];
      }
    }
  }
  return sg;
}

template<class T>
SimpleGrid<T> SimpleGrid<T>::operator-(const SimpleGrid<T>& gridIn)
{
  SimpleGrid sg();
  for (unsigned int x = 0; x < d_gridExtents.x(); ++x) {
    for (unsigned int y = 0; y < d_gridExtents.y(); ++y) {
      for (unsigned int z = 0; z < d_gridExtents.z(); ++z) {
        sg[x][y][z] -= gridIn.d_charges[x][y][z];
      }
    }
  }
  return sg;
}

// ------------------------------------------------------------------

template<class T>
SimpleGrid<T>& SimpleGrid<T>::operator-()
{

}

template<class T>
std::ostream& SimpleGrid<T>::print(std::ostream& out) const
{
  out << "Extent, [x,y,z]: " << this->d_gridExtents;
  out << "Offset, [x,y,z]: " << this->d_gridOffset;
  out << "GhostCells, [x,y,z]: " << this->d_gridExtents;
  return out;
}

template<class T>
std::ostream& operator<<(std::ostream& out,
                         const Uintah::SimpleGrid<T>& sg)
{
  return sg.print(out);
}
