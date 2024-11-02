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

#include "ConvexHullCollisionURDFGenerator.h"
#include <urdf_model/model.h>
#include <filesystem>
#include <vector>
#include <string>
#include <igl/copyleft/cgal/convex_hull.h>
#include <igl/moments.h>

#include "log/log.h"

ConvexHullCollisionURDFGenerator::ConvexHullCollisionURDFGenerator() : URDFGenerator() {
    m_model = std::make_shared<urdf::ModelInterface>();
}

ConvexHullCollisionURDFGenerator::~ConvexHullCollisionURDFGenerator() {
    m_model.reset();
    m_model = nullptr;
}

bot_common::ErrorInfo
ConvexHullCollisionURDFGenerator::run(const std::string &urdf_path, const std::string &output_path,
                                      const std::vector<std::pair<std::string, std::string>>& replace_pairs) {
    auto ret = loadURDF(urdf_path, m_model);
    if(!ret.IsOK()){
        PLOGE<<ret.error_msg();
        return ret;
    }
    for (auto &link_pair: m_model->links_) {
        if(link_pair.second->collision_array.size() > 1){
            return {bot_common::ErrorCode::Error, "We only accept one collision mesh"};
        }else{
            auto& collision = link_pair.second->collision;
            if(collision != nullptr){
                switch (collision->geometry->type) {
                    case urdf::Geometry::MESH: {
                        auto *mesh = dynamic_cast<urdf::Mesh *>(collision->geometry.get());
                        std::string filename_raw = mesh->filename;
                        for(const auto& replace_pair : replace_pairs){
                            replaceWith(filename_raw, replace_pair.first, replace_pair.second);
                        }
                        std::filesystem::path filename = filename_raw;
                        Eigen::MatrixXd V;
                        Eigen::MatrixXi F, N;
                        bool alreadyOBJ = false;
                        ret = loadedIntoIGL(filename, V, F, N, alreadyOBJ);
                        if (!ret.IsOK()) {
                            PLOGE << ret.error_msg();
                            return ret;
                        } else {
                            Eigen::MatrixXd CH_V;
                            Eigen::MatrixXi CH_F;

                            igl::copyleft::cgal::convex_hull(V, CH_V, CH_F);

                            ret = saveCollisionGeometry(filename, CH_V, CH_F);
                            if (!ret.IsOK()) {
                                PLOGE << ret.error_msg();
                                return ret;
                            }else{
                                // compute inertia and write collision into URDF
                                mesh->filename = filename.string();
                                for(const auto& replace_pair : replace_pairs){
                                    replaceWith(mesh->filename, replace_pair.second, replace_pair.first);
                                }
                                double volume;
                                Eigen::Vector3d centroid;
                                Eigen::Matrix3d inertia;
                                igl::moments(CH_V, CH_F, volume, centroid, inertia);
                                if(link_pair.second->inertial)
                                    inertia *= link_pair.second->inertial->mass;
                                else
                                    link_pair.second->inertial = std::make_shared<urdf::Inertial>();
                                auto& inertia_out = link_pair.second->inertial;
                                inertia_out->ixx = inertia(0, 0);
                                inertia_out->ixy = inertia(0, 1);
                                inertia_out->ixz = inertia(0, 2);
                                inertia_out->iyy = inertia(1, 1);
                                inertia_out->iyz = inertia(1, 2);
                                inertia_out->izz = inertia(2, 2);
                            }
                        }
                        break;
                    }
                    //do nothing for simple geometry primitives
                    default:
                        break;
                }
            }
        }
    }
    return writeURDF(output_path, m_model);
}
