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

#include "sphereTreeWrapper/sphereTreeHubbard.h"
#include "yaml-cpp/yaml.h"
#include "irmv/bot_common/log/log.h"
#include "Surface/Surface.h"
#include "Surface/OBJLoader.h"
#include "MedialAxis/Voronoi3D.h"
#include "API/MSGrid.h"
#include "API/SSIsohedron.h"
#include "API/STGHubbard.h"
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

    SphereTreeMethodHubbard::SphereTreeMethodHubbard(const std::string &config_path) {
        YAML::Node doc_full = YAML::LoadFile(config_path);
        m_method_name = "Hubbard";
        auto doc = doc_full[m_method_name];
        branch = getParam<int>(doc, "Branch", 8);
        depth = getParam<int>(doc, "Depth", 3);
        numSamples = getParam<int>(doc, "NumSamples", 500);
        minSamples = getParam<int>(doc, "MinSamples", 1);
        verify = getParam<bool>(doc, "Verify", false);
        nopause = getParam<bool>(doc, "Nopause", false);

    }

    SphereTreeUniquePtr SphereTreeMethodHubbard::create(const std::string &config_path) {
        return bot_common::AlgorithmFactory<SphereTreeMethodBase, const std::string &>::CreateAlgorithm(
                SphereTreeMethodHubbardName, config_path);
    }

    bot_common::ErrorInfo SphereTreeMethodHubbard::constructTree(Surface& sur, MySphereTree &tree) {
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
            verify model
        */
        if (verify && !verifyModel(sur)) {
            return {bot_common::ErrorCode::Error, "model is not usable"};
        }

        /*
            generate the set of sample points
        */
        Array<Surface::Point> samplePts;
        MSGrid::generateSamples(&samplePts, numSamples, sur, TRUE, minSamples);
        PLOGD << "sample points: " << samplePts.getSize();

        //  SurfaceRep coverRep;
        //  coverRep.setup(coverPts);

        /*
           Setup voronoi diagram
        */
        Point3D pC {};
        pC.x = (sur.pMax.x + sur.pMin.x) / 2.0;
        pC.y = (sur.pMax.y + sur.pMin.y) / 2.0;
        pC.z = (sur.pMax.z + sur.pMin.z) / 2.0;

        Voronoi3D vor;
        vor.initialise(pC, 1.5 * sur.pMin.distance(pC));
        vor.randomInserts(samplePts);

        /*
            setup HUBBARD's algorithm
        */
        STGHubbard hubbard;
        hubbard.setup(&vor, &mt);

        /*
            make sphere-tree
        */
        SphereTree m_tree;
        m_tree.setupTree(branch, depth + 1);

        hubbard.constructTree(&m_tree);
        m_tree.setupTree(branch, depth + 1);
        tree.setBySphereTree(m_tree, 1.0 / boxScale);

        return bot_common::ErrorInfo::OK();
    }
}