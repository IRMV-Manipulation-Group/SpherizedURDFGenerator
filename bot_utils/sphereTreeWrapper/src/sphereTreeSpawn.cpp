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


#include "sphereTreeWrapper/sphereTreeSpawn.h"
#include "yaml-cpp/yaml.h"
#include "log/log.h"
#include "Surface/Surface.h"
#include "Surface/OBJLoader.h"
#include "API/MSGrid.h"
#include "API/SEConvex.h"
#include "API/SESphPt.h"
#include "API/SSIsohedron.h"
#include "API/SRSpawn.h"
#include "API/STGGeneric.h"
#include "EvalTree.h"
#include "VerifyModel.h"

namespace SphereTreeMethod {
    template<typename T>
    static inline T getParam(const YAML::Node &node, const std::string &name, const T &defaultValue) {
        T v;
        try {
            v = node[name].as<T>();
        } catch (std::exception &e) {
            PLOGW << "Yaml exception " << e.what();
            v = defaultValue;
        }
        return v;
    }

    SphereTreeMethodSpawn::SphereTreeMethodSpawn(const std::string &config_path) {
        YAML::Node doc_full = YAML::LoadFile(config_path);
        m_method_name = "Spawn";
        auto doc = doc_full[m_method_name];
        testerLevels = getParam<int>(doc, "TesterLevers", -1);
        branch = getParam<int>(doc, "Branch", 8);
        depth = getParam<int>(doc, "Depth", 3);
        numCoverPts = getParam<int>(doc, "NumCoverPts", 5000);
        minCoverPts = getParam<int>(doc, "MinCoverPts", 5);
        verify = getParam<bool>(doc, "Verify", false);
        nopause = getParam<bool>(doc, "Nopause", false);
        eval = getParam<bool>(doc, "Eval", false);
    }

    SphereTreeUniquePtr SphereTreeMethodSpawn::create(const std::string &config_path) {
        return bot_common::AlgorithmFactory<SphereTreeMethodBase, const std::string &>::CreateAlgorithm(
                SphereTreeMethodSpawnName, config_path);
    }

    bot_common::ErrorInfo SphereTreeMethodSpawn::constructTree(const std::string &file, MySphereTree &tree) {
        if (file.size() > 4 && file.substr(file.size() - 4) == ".obj") {
            Surface sur;
            bool loaded = loadOBJ(&sur, file.c_str());
            if (!loaded) {
                return {bot_common::ErrorCode::Error, file + " cannot be loaded"};
            }
            /*
                scale box
            */
            float boxScale = sur.fitIntoBox(1000);

            /*
                make medial tester
            */
            MedialTester mt;
            mt.setSurface(sur);
            mt.useLargeCover = true;

            /*
                setup evaluator
            */
            SEConvex convEval;
            convEval.setTester(mt);
            SEBase *eval_ = &convEval;

            Array<Point3D> sphPts;
            SESphPt sphEval;
            if (testerLevels > 0){   //  <= 0 will use convex tester
                SSIsohedron::generateSamples(&sphPts, testerLevels-1);
                sphEval.setup(mt, sphPts);
                eval_ = &sphEval;
                PLOGI << "Using concave tester " << sphPts.getSize();
            }

            /*
                verify model
            */
            if (verify && !verifyModel(sur)) {
                return {bot_common::ErrorCode::Error, "model is not usable"};
            }

            /*
                setup for the set of cover points
            */
            Array<Surface::Point> coverPts;
            MSGrid::generateSamples(&coverPts, numCoverPts, sur, TRUE, minCoverPts);
            PLOGD<< coverPts.getSize() << "cover points";

            /*
                setup SPAWN algorithm
            */
            SRSpawn spawn;
            spawn.setup(mt);
            spawn.useIterativeSelect = false;
            spawn.eval = eval_;

            /*
                setup SphereTree constructor - using dynamic construction
            */
            STGGeneric treegen;
            treegen.eval = eval_;
            treegen.useRefit = true;
            treegen.setSamples(coverPts);
            treegen.reducer = &spawn;

            /*
                make sphere-tree
            */
            SphereTree m_tree;
            m_tree.setupTree(branch, depth+1);

            treegen.constructTree(&m_tree);

            //do scale
            tree.setBySphereTree(m_tree, 1.0 / boxScale);
            return bot_common::ErrorInfo::OK();
        } else
            return {bot_common::ErrorCode::Error, file + "is invalid file. Only OBJ file is supported"};
    }
}