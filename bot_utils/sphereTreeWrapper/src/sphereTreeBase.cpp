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

#include "sphereTreeWrapper/sphereTreeBase.h"
#include "Surface/OBJLoader.h"
namespace SphereTreeMethod {

    Sphere::Sphere(double x, double y, double z, double r) {
        data << x, y, z, r;
    }

    const Eigen::Vector4d &Sphere::getData() const {
        return data;
    }

    void Sphere::setByRaw(double x, double y, double z, double r) {
        data << x, y, z, r;
    }

    const double &Sphere::X() const {
        return data.x();
    }

    const double &Sphere::Y() const {
        return data.y();
    }

    const double &Sphere::Z() const {
        return data.z();
    }

    const double &Sphere::R() const {
        return data.w();
    }

    MySphereTree::MySphereTree(const SphereTree &tree, double scale) {
        setBySphereTree(tree, scale);
    }

    void MySphereTree::setBySphereTree(const SphereTree &tree, double scale) {
        int num_nodes = tree.nodes.getSize();
        levels = tree.levels;
        degree = tree.degree;
        for (int i = 0; i < num_nodes; i++) {
            const STSphere *s = &tree.nodes.index(i);
            if (i == 0) {
                biggest_sphere.setByRaw(s->c.x * scale, s->c.y * scale, s->c.z * scale, s->r * scale);
                continue;
            }
            sub_spheres.emplace_back(s->c.x * scale, s->c.y * scale, s->c.z * scale, s->r * scale);
        }
    }

    const std::string &SphereTreeMethodBase::getMethodName() {
        return m_method_name;
    }

    void
    SphereTreeMethodBase::loadOBJFromEigen(Surface *sur, const Eigen::MatrixXd &V, const Eigen::MatrixXi &F,
                                           float boxSize) {
        // Clear existing data
        sur->vertices.setSize(0);
        sur->triangles.setSize(0);

        // Add vertices
        for (int i = 0; i < V.rows(); ++i) {
            Surface::Point *p = &sur->vertices.addItem();
            p->p.x = V(i, 0);
            p->p.y = V(i, 1);
            p->p.z = V(i, 2);
        }

        // Add triangles
        for (int i = 0; i < F.rows(); ++i) {
            Surface::Triangle *tri = &sur->triangles.addItem();
            tri->v[0] = F(i, 0);
            tri->v[1] = F(i, 1);
            tri->v[2] = F(i, 2);
            tri->f[0] = -1; // Neighboring face not given in OBJ
            tri->f[1] = -1;
            tri->f[2] = -1;

            // Compute triangle normal
            Vector3D V1 {}, V2 {};
            V1.difference(sur->vertices.index(tri->v[1]).p, sur->vertices.index(tri->v[0]).p);
            V2.difference(sur->vertices.index(tri->v[2]).p, sur->vertices.index(tri->v[0]).p);
            tri->n.cross(V1, V2);
            tri->n.norm();

        }
        sur->setupBoundingBox();

        // Setup bounding box and normals
        if (boxSize > 0)
            sur->fitIntoBox(boxSize);

        sur->setupNormals(0, sur->vertices.getSize(), 0, sur->triangles.getSize());
        sur->setupAdjacent(0, sur->triangles.getSize());
        sur->stitchTriangles();
    }

    bot_common::ErrorInfo
    SphereTreeMethodBase::constructTree(const std::string &file, SphereTreeMethod::MySphereTree &tree) {
        if (file.size() > 4 && file.substr(file.size() - 4) == ".obj") {
            Surface sur;

            bool loaded = loadOBJ(&sur, file.c_str());
            if (!loaded) {
                return {bot_common::ErrorCode::Error, file + " cannot be loaded"};
            }
            return constructTree(sur, tree);
        } else
            return {bot_common::ErrorCode::Error, file + "is invalid file. Only OBJ file is supported"};
    }

    bot_common::ErrorInfo SphereTreeMethodBase::constructTree(const Eigen::MatrixXd &V, const Eigen::MatrixXi &F,
                                                              SphereTreeMethod::MySphereTree &tree) {
        Surface sur;
        loadOBJFromEigen(&sur, V, F);
        return constructTree(sur, tree);
    }

    void SphereTreeMethodBase::setBranch(int branch) {
        this->branch = branch;
    }
}
