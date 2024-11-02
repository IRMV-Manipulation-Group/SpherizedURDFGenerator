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

#include <cstdio>
#include <ctime>  // clock_t, clock, CLOCKS_PER_SEC
#include <gtest/gtest.h>
#include "ConvexHullCollisionURDFGenerator.h"
#include "SphereTreeURDFGenerator.h"


class URDFGeneratorTest : public testing::Test {
protected:
    void SetUp() override {
        convex_generator = std::make_shared<ConvexHullCollisionURDFGenerator>();
        spherized_generator = std::make_shared<SphereTreeURDFGenerator>(configPath + "/sphereTree/sphereTreeConfig.yml", false, false);
    }

    void TearDown() override {
    }

public:

protected:
    std::string resourcePath = RESOURCE_PATH;
    std::string configPath = CONFIG_PATH;
    std::shared_ptr<ConvexHullCollisionURDFGenerator> convex_generator;
    std::shared_ptr<SphereTreeURDFGenerator> spherized_generator;
};

TEST_F(URDFGeneratorTest, CVXTest) {
    auto ret = convex_generator->run("/home/zyx/path_ws/src/franka_panda_description/robots/panda_arm.urdf",
                          "/home/zyx/path_ws/src/franka_panda_description/robots/panda_arm_cvx.urdf",{
                                             {"package:/", "/home/zyx/path_ws/src/ningde/simulation"}
    });

    std::cout<<ret.error_msg()<<std::endl;
}

TEST_F(URDFGeneratorTest, STTest) {
    auto ret = spherized_generator->run("/home/zyx/path_ws/src/franka_panda_description/robots/panda_arm.urdf",
                                     "/home/zyx/path_ws/src/franka_panda_description/robots/panda_arm_spherized.urdf",{
                                             {"package:/", "/home/zyx/path_ws/src"}
                                     });

    std::cout<<ret.error_msg()<<std::endl;
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}