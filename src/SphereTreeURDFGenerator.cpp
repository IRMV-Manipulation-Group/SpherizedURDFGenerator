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

#include "SphereTreeURDFGenerator.h"
#include "sphereTreeWrapper/sphereTreeSpawn.h"
#include "sphereTreeWrapper/sphereTreeOctree.h"
#include "sphereTreeWrapper/sphereTreeHubbard.h"
#include "sphereTreeWrapper/sphereTreeMedial.h"
#include "sphereTreeWrapper/sphereTreeGrid.h"
#include "yaml-cpp/yaml.h"
#include <urdf_model/model.h>
#include <filesystem>
#include <vector>
#include <string>
#include <igl/decimate.h>
#include "log/log.h"

SphereTreeURDFGenerator::SphereTreeURDFGenerator(const std::string &st_config_path, bool single_sphere, bool simplify) {
    YAML::Node doc = YAML::LoadFile(st_config_path);
    auto type = static_cast<SphereTreeMethod::STMethodType>(doc["Method"].as<int>());
    switch (type) {
        case SphereTreeMethod::Grid:
            m_method = SphereTreeMethod::SphereTreeMethodGrid::create(st_config_path);
        case SphereTreeMethod::Hubbard:
            m_method = SphereTreeMethod::SphereTreeMethodHubbard::create(st_config_path);
        case SphereTreeMethod::Medial:
            m_method = SphereTreeMethod::SphereTreeMethodMedial::create(st_config_path);
        case SphereTreeMethod::Octree:
            m_method = SphereTreeMethod::SphereTreeMethodOctree::create(st_config_path);
        case SphereTreeMethod::Spawn:
            m_method = SphereTreeMethod::SphereTreeMethodSpawn::create(st_config_path);
        default:
            m_method = SphereTreeMethod::SphereTreeMethodMedial::create(st_config_path);
    }
    doSimplify = simplify;
    singleSphere = single_sphere;
}

SphereTreeURDFGenerator::~SphereTreeURDFGenerator(){
    m_method.reset();
    m_method = nullptr;
}

bot_common::ErrorInfo SphereTreeURDFGenerator::run(const std::string &urdf_path, const std::string &output_path,
                                                   const std::vector<std::pair<std::string, std::string>> &replace_pairs) {

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
                            Eigen::Vector3d centroid;
                            if(doSimplify){
                                Eigen::MatrixXd CH_V;
                                Eigen::MatrixXi CH_F;
                                Eigen::VectorXi J;
                                igl::decimate(V, F, 0.3 * F.rows(), CH_V, CH_F, J);
                                ret = saveCollisionGeometry(filename, CH_V, CH_F);
                                centroid = CH_V.colwise().mean();
                            }else if(!alreadyOBJ){
                                ret = saveCollisionGeometry(filename, V, F);
                                centroid = V.colwise().mean();
                            }

                            if (!ret.IsOK()) {
                                PLOGE << ret.error_msg();
                                return ret;
                            }else{
                                SphereTreeMethod::MySphereTree tree;
                                //todo: handle construct errors;
                                m_method->constructTree(filename.string(), tree);
                                if(singleSphere){
                                    auto sphere = std::make_shared<urdf::Sphere>();
                                    sphere->radius = tree.biggest_sphere.R();
                                    collision->origin.position.x = centroid.x() + tree.biggest_sphere.X();
                                    collision->origin.position.y = centroid.y() + tree.biggest_sphere.Y();
                                    collision->origin.position.z = centroid.z() + tree.biggest_sphere.Z();
                                    collision->origin.rotation.clear();
                                    collision->geometry = sphere;
                                }else{
                                    link_pair.second->collision_array.clear();
                                    for(const SphereTreeMethod::Sphere& sub_sphere : tree.sub_spheres){
                                        auto sphere_collision  = std::make_shared<urdf::Collision>();
                                        sphere_collision->origin.position.x = centroid.x() + sub_sphere.X();
                                        sphere_collision->origin.position.y = centroid.y() + sub_sphere.Y();
                                        sphere_collision->origin.position.z = centroid.z() + sub_sphere.Z();
                                        sphere_collision->origin.rotation.clear();
                                        auto sphere = std::make_shared<urdf::Sphere>();
                                        sphere->radius = sub_sphere.R();
                                        sphere_collision->geometry = sphere;
                                        link_pair.second->collision_array.emplace_back(sphere_collision);
                                    }
                                    link_pair.second->collision = link_pair.second->collision_array[0];
                                }
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