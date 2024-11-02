// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2014 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "centroid.h"
#include <Eigen/Geometry>

template <
  typename DerivedV, 
  typename DerivedF, 
  typename Derivedc, 
  typename Derivedvol>
IGL_INLINE void igl::centroid(
  const Eigen::MatrixBase<DerivedV>& V,
  const Eigen::MatrixBase<DerivedF>& F,
  Eigen::PlainObjectBase<Derivedc>& cen,
  Derivedvol & vol)
{
  using namespace Eigen;
  assert(F.cols() == 3 && "F should contain triangles.");
  assert(V.cols() == 3 && "V should contain 3d points.");
  const int m = F.rows();
  cen.setZero();
  vol = 0;
  // loop over faces
  for(int f = 0;f<m;f++)
  {
    // "Calculating the volume and centroid of a polyhedron in 3d" [Nuernberg 2013]
    // http://www2.imperial.ac.uk/~rn/centroid.pdf
    // rename corners
    typedef Eigen::Matrix<typename DerivedV::Scalar,1,3> RowVector3S;
    const RowVector3S & a = V.row(F(f,0));
    const RowVector3S & b = V.row(F(f,1));
    const RowVector3S & c = V.row(F(f,2));
    // un-normalized normal
    const RowVector3S & n = (b-a).cross(c-a);
    // total volume via divergence theorem: ∫ 1
    vol += n.dot(a)/6.;
    // centroid via divergence theorem and midpoint quadrature: ∫ x
    cen.array() += (1./24.*n.array()*((a+b).array().square() + (b+c).array().square() + 
        (c+a).array().square()).array());
  }
  cen *= 1./(2.*vol);
}

template <
  typename DerivedV, 
  typename DerivedF, 
  typename Derivedc>
IGL_INLINE void igl::centroid(
  const Eigen::MatrixBase<DerivedV>& V,
  const Eigen::MatrixBase<DerivedF>& F,
  Eigen::PlainObjectBase<Derivedc>& c)
{
  typename Derivedc::Scalar vol;
  return centroid(V,F,c,vol);
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
template void igl::centroid<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, 1, 3, 1, 1, 3>, double>(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, 1, 3, 1, 1, 3> >&, double&);
// generated by autoexplicit.sh
template void igl::centroid<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3>, Eigen::Matrix<float, 3, 1, 0, 3, 1>, float>(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> >&, float&);
// generated by autoexplicit.sh
template void igl::centroid<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3>, Eigen::Matrix<float, 3, 1, 0, 3, 1> >(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<unsigned int, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 3, 1, 0, 3, 1> >&);
// generated by autoexplicit.sh
template void igl::centroid<Eigen::Matrix<float, -1, 3, 1, -1, 3>, Eigen::Matrix<int, -1, 3, 1, -1, 3>, Eigen::Matrix<float, 1, 3, 1, 1, 3> >(Eigen::MatrixBase<Eigen::Matrix<float, -1, 3, 1, -1, 3> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 3, 1, -1, 3> > const&, Eigen::PlainObjectBase<Eigen::Matrix<float, 1, 3, 1, 1, 3> >&);
template void igl::centroid<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, 3, 1, 0, 3, 1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, 3, 1, 0, 3, 1> >&);
template void igl::centroid<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, 1, 3, 1, 1, 3> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, 1, 3, 1, 1, 3> >&);
#endif
