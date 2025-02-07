/*
 ************************************************************************\

                              C O P Y R I G H T

   Copyright © 2024 IRMV lab, Shanghai Jiao Tong University, China.
                         All Rights Reserved.

   Licensed under the Creative Commons Attribution-NonCommercial 4.0
   International License (CC BY-NC 4.0).
   You are free to use, copy, modify, and distribute this software and its
   documentation for educational, research, and other non-commercial purposes,
   provided that appropriate credit is given to the original author(s) and
   copyright holder(s).

   For commercial use or licensing inquiries, please contact:
   IRMV lab, Shanghai Jiao Tong University at: https://irmv.sjtu.edu.cn/

                              D I S C L A I M E R

   IN NO EVENT SHALL TRINITY COLLEGE DUBLIN BE LIABLE TO ANY PARTY FOR
   DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING,
   BUT NOT LIMITED TO, LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF TRINITY COLLEGE DUBLIN HAS BEEN ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGES.

   TRINITY COLLEGE DUBLIN DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE. THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND TRINITY
   COLLEGE DUBLIN HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
   ENHANCEMENTS, OR MODIFICATIONS.

   The authors may be contacted at the following e-mail addresses:

           YX.E.Z yixuanzhou@sjtu.edu.cn

   Further information about the IRMV and its projects can be found at the ISG web site :

          https://irmv.sjtu.edu.cn/

 \*************************************************************************

 */

#ifndef SPHERIZEDURDFGENERATOR_SPHERETREEBASE_H
#define SPHERIZEDURDFGENERATOR_SPHERETREEBASE_H

#include "irmv/bot_common/state/error_code.h"
#include <memory>
#include <SphereTree/SphereTree.h>
#include <Eigen/Dense>
#include "Surface/Surface.h"

namespace SphereTreeMethod {

    enum Optimiser {
        NONE,
        SIMPLEX,
        BALANCE
    };

    class Sphere {
    public:
        Sphere() = default;

        Sphere(double x, double y, double z, double r);

        ~Sphere() = default;

    protected:
        Eigen::Vector4d data;

    public:
        const Eigen::Vector4d &getData() const;

        const double &X() const;

        const double &Y() const;

        const double &Z() const;

        const double &R() const;

        void setByRaw(double x, double y, double z, double r);
    };

    class MySphereTree {
    public:
        MySphereTree() = default;

        MySphereTree(const SphereTree &tree, double scale);

        ~MySphereTree() = default;

    public:
        ulong levels, degree;
        Sphere biggest_sphere;
        std::vector<Sphere> sub_spheres;
    public:
        void setBySphereTree(const SphereTree &tree, double scale);
    };

    enum STMethodType{
        Grid,
        Hubbard,
        Medial,
        Octree,
        Spawn
    };

    class SphereTreeMethodBase {
    public:
        SphereTreeMethodBase() = default;

        virtual ~SphereTreeMethodBase() = default;

    public:
        virtual bot_common::ErrorInfo constructTree(const std::string &file, MySphereTree &tree);

        virtual bot_common::ErrorInfo constructTree(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F, MySphereTree &tree);

        virtual bot_common::ErrorInfo constructTree(Surface &sur, MySphereTree &tree) = 0;

        const std::string &getMethodName();

        void setBranch(int branch);
    protected:
        std::string m_method_name;

        static void loadOBJFromEigen(Surface *sur, const Eigen::MatrixXd & V, const Eigen::MatrixXi& F, float boxSize = -1);

        int branch = 8;             ///<  branching factor of the sphere-tree


    };

    typedef std::shared_ptr<SphereTreeMethodBase> SphereTreePtr;
    typedef std::unique_ptr<SphereTreeMethodBase> SphereTreeUniquePtr;
}

#endif //SPHERIZEDURDFGENERATOR_SPHERETREEBASE_H
