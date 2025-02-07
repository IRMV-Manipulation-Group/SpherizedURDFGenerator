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

#include <urdf_model/model.h>
#include <urdf_parser/urdf_parser.h>
#include <tinyxml2.h>
#include "URDFGenerator.h"
#include "irmv/bot_common/log/log.h"
#include <fstream>
#include <igl/readSTL.h>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <set>

bot_common::ErrorInfo
URDFGenerator::loadURDF(const std::string &urdf_path, urdf::ModelInterfaceSharedPtr &robot_model) {
    std::ifstream urdf_file(urdf_path);
    if (!urdf_file.is_open()) {
        PLOGE << "Could not open URDF file: " << urdf_path;
        return {bot_common::ErrorCode::Error, "Not valid urdf path"};
    }

    std::string urdf_xml((std::istreambuf_iterator<char>(urdf_file)),
                         std::istreambuf_iterator<char>());
    urdf_file.close();

    robot_model = urdf::parseURDF(urdf_xml);
    if (!robot_model) {
        PLOGE << "Error: Failed to parse URDF file " << urdf_path;
        return {bot_common::ErrorCode::Error, "Not valid urdf file content"};
    }
    return bot_common::ErrorInfo::OK();
}

// Function to create an origin element
void createOriginElement(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *parent, const urdf::Pose &origin) {
    tinyxml2::XMLElement *origin_elem = doc.NewElement("origin");
    std::stringstream ss;
    ss << origin.position.x << " " << origin.position.y << " " << origin.position.z;
    origin_elem->SetAttribute("xyz", ss.str().c_str());

    double roll, pitch, yaw;
    origin.rotation.getRPY(roll, pitch, yaw);
    ss.str(""); // Clear the stringstream
    ss << roll << " " << pitch << " " << yaw;
    origin_elem->SetAttribute("rpy", ss.str().c_str());

    parent->InsertEndChild(origin_elem);
}

// Function to create a geometry element
void createGeometryElement(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *parent,
                           const urdf::GeometrySharedPtr &geometry) {
    tinyxml2::XMLElement *geometry_elem = doc.NewElement("geometry");
    switch (geometry->type) {
        case urdf::Geometry::BOX: {
            urdf::Box *box = dynamic_cast<urdf::Box *>(geometry.get());
            if (box) {
                tinyxml2::XMLElement *box_elem = doc.NewElement("box");
                std::stringstream ss;
                ss << box->dim.x << " " << box->dim.y << " " << box->dim.z;
                box_elem->SetAttribute("size", ss.str().c_str());
                geometry_elem->InsertEndChild(box_elem);
            }
            break;
        }
        case urdf::Geometry::CYLINDER: {
            urdf::Cylinder *cylinder = dynamic_cast<urdf::Cylinder *>(geometry.get());
            if (cylinder) {
                tinyxml2::XMLElement *cylinder_elem = doc.NewElement("cylinder");
                cylinder_elem->SetAttribute("radius", std::to_string(cylinder->radius).c_str());
                cylinder_elem->SetAttribute("length", std::to_string(cylinder->length).c_str());
                geometry_elem->InsertEndChild(cylinder_elem);
            }
            break;
        }
        case urdf::Geometry::SPHERE: {
            urdf::Sphere *sphere = dynamic_cast<urdf::Sphere *>(geometry.get());
            if (sphere) {
                tinyxml2::XMLElement *sphere_elem = doc.NewElement("sphere");
                sphere_elem->SetAttribute("radius", std::to_string(sphere->radius).c_str());
                geometry_elem->InsertEndChild(sphere_elem);
            }
            break;
        }
        case urdf::Geometry::MESH: {
            urdf::Mesh *mesh = dynamic_cast<urdf::Mesh *>(geometry.get());
            if (mesh) {
                tinyxml2::XMLElement *mesh_elem = doc.NewElement("mesh");
                mesh_elem->SetAttribute("filename", mesh->filename.c_str());
                if (mesh->scale.x != 1 || mesh->scale.y != 1 || mesh->scale.z != 1) {
                    std::stringstream ss;
                    ss << mesh->scale.x << " " << mesh->scale.y << " " << mesh->scale.z;
                    mesh_elem->SetAttribute("scale", ss.str().c_str());
                }
                geometry_elem->InsertEndChild(mesh_elem);
            }
            break;
        }
    }
    parent->InsertEndChild(geometry_elem);
}

void createInertiaElement(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *parent, const urdf::Inertial &inertial) {
    tinyxml2::XMLElement *inertial_elem = doc.NewElement("inertial");

    // Add origin element
    createOriginElement(doc, inertial_elem, inertial.origin);

    // Add mass element
    tinyxml2::XMLElement *mass_elem = doc.NewElement("mass");
    mass_elem->SetAttribute("value", std::to_string(inertial.mass).c_str());
    inertial_elem->InsertEndChild(mass_elem);

    // Add inertia element
    tinyxml2::XMLElement *inertia_elem = doc.NewElement("inertia");
    std::stringstream ss;
    ss << std::scientific << std::setprecision(6);

    ss << inertial.ixx;
    inertia_elem->SetAttribute("ixx", ss.str().c_str());
    ss.str("");
    ss.clear();
    ss << inertial.ixy;
    inertia_elem->SetAttribute("ixy", ss.str().c_str());
    ss.str("");
    ss.clear();
    ss << inertial.ixz;
    inertia_elem->SetAttribute("ixz", ss.str().c_str());
    ss.str("");
    ss.clear();
    ss << inertial.iyy;
    inertia_elem->SetAttribute("iyy", ss.str().c_str());
    ss.str("");
    ss.clear();
    ss << inertial.iyz;
    inertia_elem->SetAttribute("iyz", ss.str().c_str());
    ss.str("");
    ss.clear();
    ss << inertial.izz;
    inertia_elem->SetAttribute("izz", ss.str().c_str());
    inertial_elem->InsertEndChild(inertia_elem);

    parent->InsertEndChild(inertial_elem);
}

// Handler for visual or collision elements
void handleVisualOrCollision(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *link_elem,
                             const urdf::VisualSharedPtr &visual) {
    tinyxml2::XMLElement *visual_elem = doc.NewElement("visual");
    createOriginElement(doc, visual_elem, visual->origin);
    createGeometryElement(doc, visual_elem, visual->geometry);

    // Add material element if available
    if (visual->material) {
        tinyxml2::XMLElement *material_elem = doc.NewElement("material");
        if (!visual->material->name.empty()) {
            material_elem->SetAttribute("name", visual->material->name.c_str());
        } else {
            material_elem->SetAttribute("name", "");
        }

        tinyxml2::XMLElement *color_elem = doc.NewElement("color");
        std::stringstream ss;
        ss << visual->material->color.r << " " << visual->material->color.g << " " << visual->material->color.b << " "
           << visual->material->color.a;
        color_elem->SetAttribute("rgba", ss.str().c_str());
        material_elem->InsertEndChild(color_elem);

        if (!visual->material->texture_filename.empty()) {
            tinyxml2::XMLElement *texture_elem = doc.NewElement("texture");
            texture_elem->SetAttribute("filename", visual->material->texture_filename.c_str());
            material_elem->InsertEndChild(texture_elem);
        }
        visual_elem->InsertEndChild(material_elem);
    }

    link_elem->InsertEndChild(visual_elem);
}

void handleVisualOrCollision(tinyxml2::XMLDocument &doc, tinyxml2::XMLElement *link_elem,
                             const urdf::CollisionSharedPtr &collision) {
    tinyxml2::XMLElement *collision_elem = doc.NewElement("collision");
    createOriginElement(doc, collision_elem, collision->origin);
    createGeometryElement(doc, collision_elem, collision->geometry);
    link_elem->InsertEndChild(collision_elem);
}


bot_common::ErrorInfo
URDFGenerator::writeURDF(const std::string &file_path, const urdf::ModelInterfaceSharedPtr &robot_model) {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLDeclaration *decl = doc.NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\"");
    doc.InsertFirstChild(decl);
    tinyxml2::XMLElement *root = doc.NewElement("robot");
    root->SetAttribute("name", robot_model->getName().c_str());
    doc.InsertEndChild(root);

    // Write links and joints based on the order of joints
    std::set<std::string> processed_links;
    for (const auto &joint_pair: robot_model->joints_) {
        const urdf::JointSharedPtr &joint = joint_pair.second;

        // Write parent link if not already processed
        if (processed_links.find(joint->parent_link_name) == processed_links.end()) {
            const urdf::LinkConstSharedPtr &parent_link = robot_model->getLink(joint->parent_link_name);
            if (parent_link) {
                tinyxml2::XMLElement *link_elem = doc.NewElement("link");
                link_elem->SetAttribute("name", parent_link->name.c_str());

                // Write visual elements
                for (const auto &visual: parent_link->visual_array) {
                    handleVisualOrCollision(doc, link_elem, visual);
                }

                // Write collision elements
                for (const auto &collision: parent_link->collision_array) {
                    handleVisualOrCollision(doc, link_elem, collision);
                }

                // Write inertia element if available
                if (parent_link->inertial) {
                    createInertiaElement(doc, link_elem, *(parent_link->inertial));
                }

                root->InsertEndChild(link_elem);
                processed_links.insert(parent_link->name);
            }
        }

        // Write child link if not already processed
        if (processed_links.find(joint->child_link_name) == processed_links.end()) {
            const urdf::LinkConstSharedPtr &child_link = robot_model->getLink(joint->child_link_name);
            if (child_link) {
                tinyxml2::XMLElement *link_elem = doc.NewElement("link");
                link_elem->SetAttribute("name", child_link->name.c_str());

                // Write visual elements
                for (const auto &visual: child_link->visual_array) {
                    handleVisualOrCollision(doc, link_elem, visual);
                }

                // Write collision elements
                for (const auto &collision: child_link->collision_array) {
                    handleVisualOrCollision(doc, link_elem, collision);
                }

                // Write inertia element if available
                if (child_link->inertial) {
                    createInertiaElement(doc, link_elem, *(child_link->inertial));
                }

                root->InsertEndChild(link_elem);
                processed_links.insert(child_link->name);
            }
        }

        // Write joint element
        tinyxml2::XMLElement *joint_elem = doc.NewElement("joint");
        joint_elem->SetAttribute("name", joint->name.c_str());

        switch (joint->type) {
            case urdf::Joint::UNKNOWN:
                joint_elem->SetAttribute("type", "unknown");
                break;
            case urdf::Joint::REVOLUTE:
                joint_elem->SetAttribute("type", "revolute");
                break;
            case urdf::Joint::CONTINUOUS:
                joint_elem->SetAttribute("type", "continuous");
                break;
            case urdf::Joint::PRISMATIC:
                joint_elem->SetAttribute("type", "prismatic");
                break;
            case urdf::Joint::FLOATING:
                joint_elem->SetAttribute("type", "floating");
                break;
            case urdf::Joint::PLANAR:
                joint_elem->SetAttribute("type", "planar");
                break;
            case urdf::Joint::FIXED:
                joint_elem->SetAttribute("type", "fixed");
                break;
            default:
                joint_elem->SetAttribute("type", "unknown");
                break;
        }

        // Add parent and child links
        tinyxml2::XMLElement *parent_elem = doc.NewElement("parent");
        parent_elem->SetAttribute("link", joint->parent_link_name.c_str());
        joint_elem->InsertEndChild(parent_elem);

        tinyxml2::XMLElement *child_elem = doc.NewElement("child");
        child_elem->SetAttribute("link", joint->child_link_name.c_str());
        joint_elem->InsertEndChild(child_elem);

        // Add origin element
        createOriginElement(doc, joint_elem, joint->parent_to_joint_origin_transform);

        // Add axis element if available
        if (joint->type != urdf::Joint::FIXED && (joint->axis.x != 0 || joint->axis.y != 0 || joint->axis.z != 0)) {
            tinyxml2::XMLElement *axis_elem = doc.NewElement("axis");
            std::stringstream ss;
            ss << joint->axis.x << " " << joint->axis.y << " " << joint->axis.z;
            axis_elem->SetAttribute("xyz", ss.str().c_str());
            joint_elem->InsertEndChild(axis_elem);
        }

        // Add limit element if applicable
        if (joint->limits) {
            tinyxml2::XMLElement *limit_elem = doc.NewElement("limit");
            limit_elem->SetAttribute("lower", std::to_string(joint->limits->lower).c_str());
            limit_elem->SetAttribute("upper", std::to_string(joint->limits->upper).c_str());
            limit_elem->SetAttribute("effort", std::to_string(joint->limits->effort).c_str());
            limit_elem->SetAttribute("velocity", std::to_string(joint->limits->velocity).c_str());
            joint_elem->InsertEndChild(limit_elem);
        }

        // Add dynamics element if applicable
        if (joint->dynamics) {
            tinyxml2::XMLElement *dynamics_elem = doc.NewElement("dynamics");
            dynamics_elem->SetAttribute("damping", std::to_string(joint->dynamics->damping).c_str());
            dynamics_elem->SetAttribute("friction", std::to_string(joint->dynamics->friction).c_str());

            joint_elem->InsertEndChild(dynamics_elem);
        }

        // Insert joint into the root
        root->InsertEndChild(joint_elem);
    }

    // Save the file
    if (doc.SaveFile(file_path.c_str()) != tinyxml2::XML_SUCCESS) {
        PLOGE << "Error: Could not write to URDF file: " << file_path;
        return {bot_common::ErrorCode::Error, "Not valid urdf file path"};
    }
    return bot_common::ErrorInfo::OK();
}

std::string URDFGenerator::toLowerCase(const std::string &str) {
    std::string result;
    result.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bot_common::ErrorInfo
URDFGenerator::loadedIntoIGL(const std::filesystem::path &filename, Eigen::MatrixXd &V, Eigen::MatrixXi &F,
                             Eigen::MatrixXi &N, bool &alreadyOBJ) {
    const std::string SUFFIX = toLowerCase(filename.extension().string());
    typedef std::function<bool(const std::filesystem::path &filename, Eigen::MatrixXd &V,
                               Eigen::MatrixXi &F,
                               Eigen::MatrixXi &N,
                               bool &alreadyOBJ)> readFunc_t;
    static std::unordered_map<std::string, readFunc_t> methods {
            {".stl", [&](const std::filesystem::path &filename, Eigen::MatrixXd &V, Eigen::MatrixXi &F,
                         Eigen::MatrixXi &N, bool &alreadyOBJ) -> bool {
                std::ifstream file_input(filename.string());
                alreadyOBJ = false;
                bool ret;
                try {
                    ret = igl::readSTL(file_input, V, F, N);
                } catch (...) {
                    ret = false;
                }
                return ret;
            }},
            {".obj", [&](const std::filesystem::path &filename, Eigen::MatrixXd &V, Eigen::MatrixXi &F,
                         Eigen::MatrixXi &N, bool &alreadyOBJ) -> bool {
                bool ret;
                alreadyOBJ = true;
                try {
                    ret = igl::readOBJ(filename.string(), V, F);
                } catch (...) {
                    ret = false;
                }
                return ret;
            }}
    };

    auto iter = methods.find(SUFFIX);
    if (iter != methods.end()) {
        if (iter->second(filename, V, F, N, alreadyOBJ)) {
            return bot_common::ErrorInfo::OK();
        } else {
            return {bot_common::ErrorCode::Error,
                    filename.string() + " can not be loaded into igl-type"};
        }
    }

    return {bot_common::ErrorCode::Error,
            std::string {"We only support OBJ/STL type but the given file type is "} + SUFFIX};

}

bot_common::ErrorInfo
URDFGenerator::saveCollisionGeometry(std::filesystem::path &filename, const Eigen::MatrixXd &V,
                                     const Eigen::MatrixXi &F) {
    std::filesystem::path dir = filename.parent_path();
    std::filesystem::path collisionDir = dir / "collision";

    if (!std::filesystem::exists(collisionDir)) {
        if (!std::filesystem::create_directory(collisionDir)) {
            PLOGE << "cannot create directory " << collisionDir;
            return {bot_common::ErrorCode::Error,
                    std::string {"Cannot create directory "} + collisionDir.string()};
        }
    }

    filename = collisionDir / filename.stem();
    filename += ".obj";
    bool write_ret = igl::writeOBJ(filename.string(), V, F);

    if (write_ret) {
        return bot_common::ErrorInfo::OK();
    }
    return {bot_common::ErrorCode::Error,
            std::string {"cannot save "} + filename.string()};
}

bool URDFGenerator::replaceWith(std::string &src, const std::string &original, const std::string &now) {
    size_t pos = src.find(original);
    if (pos != std::string::npos) {
        src.replace(pos, original.length(), now);
        return true;
    }
    return false;
}