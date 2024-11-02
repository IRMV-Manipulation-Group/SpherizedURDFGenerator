// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#include "lscm.h"
#include "lscm_hessian.h"
#include "massmatrix.h"
#include "repdiag.h"
#include "eigs.h"
#include "min_quad_with_fixed.h"
#include <iostream>


template <
  typename DerivedV, 
  typename DerivedF, 
  typename Derivedb, 
  typename Derivedbc, 
  typename DerivedV_uv, 
  typename QScalar>
IGL_INLINE bool igl::lscm(
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<Derivedb> & b,
  const Eigen::MatrixBase<Derivedbc> & bc,
  Eigen::PlainObjectBase<DerivedV_uv> & V_uv,
  Eigen::SparseMatrix<QScalar> & Q)
{
  using namespace Eigen;
  using namespace std;
  igl::lscm_hessian(V,F,Q);

  Eigen::Matrix<typename Derivedb::Scalar,Eigen::Dynamic,1> b_flat(b.size()*bc.cols(),1);
  Eigen::Matrix<typename Derivedbc::Scalar,Eigen::Dynamic,1> bc_flat(bc.size(),1);
  for(int c = 0;c<bc.cols();c++)
  {
    b_flat.block(c*b.size(),0,b.rows(),1) = c*V.rows() + b.array();
    bc_flat.block(c*bc.rows(),0,bc.rows(),1) = bc.col(c);
  }

  const VectorXd B_flat = VectorXd::Zero(V.rows()*2);
  igl::min_quad_with_fixed_data<QScalar> data;
  if(!igl::min_quad_with_fixed_precompute(Q,b_flat,SparseMatrix<QScalar>(),true,data))
  {
    return false;
  }

  MatrixXd W_flat;
  if(!min_quad_with_fixed_solve(data,B_flat,bc_flat,VectorXd(),W_flat))
  {
    return false;
  }


  assert(W_flat.rows() == V.rows()*2);
  V_uv.resize(V.rows(),2);
  for (unsigned i=0;i<V_uv.cols();++i)
  {
    V_uv.col(i) = W_flat.block(V_uv.rows()*i,0,V_uv.rows(),1);
  }
  return true;
}

template <
  typename DerivedV, 
  typename DerivedF, 
  typename Derivedb, 
  typename Derivedbc, 
  typename DerivedV_uv>
IGL_INLINE bool igl::lscm(
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  const Eigen::MatrixBase<Derivedb> & b,
  const Eigen::MatrixBase<Derivedbc> & bc,
  Eigen::PlainObjectBase<DerivedV_uv> & V_uv)
{
  Eigen::SparseMatrix<typename DerivedV_uv::Scalar> Q;
  return lscm(V, F, b, bc, V_uv, Q);
}

template <
  typename DerivedV, 
  typename DerivedF, 
  typename DerivedV_uv>
IGL_INLINE bool igl::lscm(
  const Eigen::MatrixBase<DerivedV> & V,
  const Eigen::MatrixBase<DerivedF> & F,
  Eigen::PlainObjectBase<DerivedV_uv> & V_uv)
{
  using Scalar = typename DerivedV_uv::Scalar;
  Eigen::SparseMatrix<Scalar> Q;
  igl::lscm_hessian(V,F,Q);
  Eigen::SparseMatrix<Scalar> M;
  igl::massmatrix(V,F,igl::MASSMATRIX_TYPE_DEFAULT,M);
  Eigen::SparseMatrix<Scalar> M2;
  igl::repdiag(M,2,M2);
  Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic> U;
  Eigen::Matrix<Scalar,Eigen::Dynamic,1> S;
  bool success = igl::eigs(Q,M2,3,igl::EIGS_TYPE_SM,U,S);
  if(!success)
  {
    return false;
  }
  V_uv.resize(V.rows(),2);
  V_uv<< U.col(0).head(V.rows()),U.col(0).tail(V.rows());
  return true;
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template bool igl::lscm<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
// generated by autoexplicit.sh
template bool igl::lscm<Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, -1, 0, -1, -1>, Eigen::Matrix<int, -1, 1, 0, -1, 1>, Eigen::Matrix<double, -1, -1, 0, -1, -1>, Eigen::Matrix<double, -1, -1, 0, -1, -1> >(Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, -1, 0, -1, -1> > const&, Eigen::MatrixBase<Eigen::Matrix<int, -1, 1, 0, -1, 1> > const&, Eigen::MatrixBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> > const&, Eigen::PlainObjectBase<Eigen::Matrix<double, -1, -1, 0, -1, -1> >&);
#endif
