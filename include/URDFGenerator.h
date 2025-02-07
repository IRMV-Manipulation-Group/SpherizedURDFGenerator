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


#ifndef SPHERIZEDURDFGENERATOR_URDFGNEREATOR_H
#define SPHERIZEDURDFGENERATOR_URDFGNEREATOR_H

#include <string>
#include <urdf_world/types.h>
#include <Eigen/Dense>
#include "irmv/bot_common/state/error_code.h"
#include <filesystem>

class URDFGenerator {
public:
    URDFGenerator() = default;

    virtual ~URDFGenerator() = default;
protected:
    urdf::ModelInterfaceSharedPtr m_model;
    urdf::ModelInterfaceSharedPtr m_biggest_model;
public:
    virtual bot_common::ErrorInfo run(const std::string &urdf_path, const std::string &output_path,
                                      const std::vector<std::pair<std::string, std::string>>& replace_pairs) = 0;

protected:
    bot_common::ErrorInfo loadURDF(const std::string &urdf_path, urdf::ModelInterfaceSharedPtr& robot_model);

    bot_common::ErrorInfo writeURDF(const std::string& output_path, const urdf::ModelInterfaceSharedPtr& robot_mode);

    std::string toLowerCase(const std::string &str);

    bot_common::ErrorInfo loadedIntoIGL( const std::filesystem::path &file_path, Eigen::MatrixXd &V, Eigen::MatrixXi &F,
                                         Eigen::MatrixXi &N, bool& alreadyOBJ);

    bot_common::ErrorInfo saveCollisionGeometry(std::filesystem::path& filename, const Eigen::MatrixXd & V,
                                                const Eigen::MatrixXi & F);

    static bool replaceWith(std::string &src, const std::string &original, const std::string &now);

};


#endif //SPHERIZEDURDFGENERATOR_URDFGNEREATOR_H
