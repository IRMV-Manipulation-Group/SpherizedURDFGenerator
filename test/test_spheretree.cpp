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
#include <filesystem>
#include "sphereTreeWrapper/sphereTreeBase.h"
#include "sphereTreeWrapper/sphereTreeSpawn.h"
#include "sphereTreeWrapper/sphereTreeOctree.h"
#include "sphereTreeWrapper/sphereTreeHubbard.h"
#include "sphereTreeWrapper/sphereTreeMedial.h"
#include "sphereTreeWrapper/sphereTreeGrid.h"

class SphereTreeTest : public testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }

public:
    static void
    fetchAllFilesWith(const std::string &directory_path, const std::string &suffix, std::vector<std::string> &results) {
        namespace fs = std::filesystem;
        try {
            if (fs::exists(directory_path) && fs::is_directory(directory_path)) {
                for (fs::recursive_directory_iterator iter(directory_path), end; iter != end; ++iter) {
                    if (fs::is_regular_file(*iter) && iter->path().extension() == suffix) {
                        results.push_back(fs::absolute(iter->path()).string());
                    }
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "General error: " << e.what() << std::endl;
        }
    }

    static bool replaceWith(std::string &src, const std::string &original, const std::string &now) {
        size_t pos = src.find(original);
        if (pos != std::string::npos) {
            src.replace(pos, original.length(), now);
            return true;
        }
        return false;
    }


protected:
    std::string resourcePath =  SpherizedURDFGenerator_RESOURCE_PATH;
    std::string configPath = SpherizedURDFGenerator_CONFIG_PATH;
    SphereTreeMethod::SphereTreeUniquePtr m_method;
};

TEST_F(SphereTreeTest, MedialTest) {
    std::string meshPath = "/home/zyx/path_ws/src/ningde/simulation/rm_dcr_description/meshes/collision/collision/collision/YOUSHOU.obj";
    std::vector<std::string> allOBJFiles;
    fetchAllFilesWith(meshPath, ".obj", allOBJFiles);
    m_method = SphereTreeMethod::SphereTreeMethodMedial::create(configPath + "/sphereTree/sphereTreeConfig.yml");
    const std::string& test_obj = allOBJFiles.front();
    SphereTreeMethod::MySphereTree tree;
    auto ret = m_method->constructTree(meshPath, tree);
    std::cout<<ret.error_msg()<<std::endl;
    ASSERT_TRUE(ret.IsOK());
}

TEST_F(SphereTreeTest, GridTest) {
    std::string meshPath = resourcePath + "/robots/panda/meshes/collision/simple";
    std::vector<std::string> allOBJFiles;
    fetchAllFilesWith(meshPath, ".obj", allOBJFiles);
    m_method = SphereTreeMethod::SphereTreeMethodGrid::create(configPath + "/sphereTree/sphereTreeConfig.yml");
    const std::string& test_obj = allOBJFiles.front();
    SphereTreeMethod::MySphereTree tree;
    auto ret = m_method->constructTree(test_obj, tree);
    std::cout<<ret.error_msg()<<std::endl;
    ASSERT_TRUE(ret.IsOK());
}

TEST_F(SphereTreeTest, SpawnTest) {
    std::string meshPath = resourcePath + "/robots/panda/meshes/collision/simple";
    std::vector<std::string> allOBJFiles;
    fetchAllFilesWith(meshPath, ".obj", allOBJFiles);
    m_method = SphereTreeMethod::SphereTreeMethodSpawn::create(configPath + "/sphereTree/sphereTreeConfig.yml");
    const std::string& test_obj = allOBJFiles.front();
    SphereTreeMethod::MySphereTree tree;
    auto ret = m_method->constructTree(test_obj, tree);
    std::cout<<ret.error_msg()<<std::endl;
    ASSERT_TRUE(ret.IsOK());
}


TEST_F(SphereTreeTest, HubbardTest) {
    std::string meshPath = resourcePath + "/robots/panda/meshes/collision/simple";
    std::vector<std::string> allOBJFiles;
    fetchAllFilesWith(meshPath, ".obj", allOBJFiles);
    m_method = SphereTreeMethod::SphereTreeMethodHubbard::create(configPath + "/sphereTree/sphereTreeConfig.yml");
    const std::string& test_obj = allOBJFiles.front();
    SphereTreeMethod::MySphereTree tree;
    auto ret = m_method->constructTree(test_obj, tree);
    std::cout<<ret.error_msg()<<std::endl;
    ASSERT_TRUE(ret.IsOK());
}

TEST_F(SphereTreeTest, OctreeTest) {
    std::string meshPath = resourcePath + "/robots/panda/meshes/collision/simple";
    std::vector<std::string> allOBJFiles;
    fetchAllFilesWith(meshPath, ".obj", allOBJFiles);
    m_method = SphereTreeMethod::SphereTreeMethodOctree::create(configPath + "/sphereTree/sphereTreeConfig.yml");
    const std::string& test_obj = allOBJFiles.front();
    SphereTreeMethod::MySphereTree tree;
    auto ret = m_method->constructTree(test_obj, tree);
    std::cout<<ret.error_msg()<<std::endl;
    ASSERT_TRUE(ret.IsOK());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}