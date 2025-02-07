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

#include "sphereTreeWrapper/sphereTreeMedial.h"
#include "yaml-cpp/yaml.h"
#include "irmv/bot_common/log/log.h"
#include "Surface/Surface.h"
#include "Surface/OBJLoader.h"
#include "API/MSGrid.h"
#include "MedialAxis/Voronoi3D.h"
#include "API/SEConvex.h"
#include "API/SESphPt.h"
#include "API/VFAdaptive.h"
#include "API/SSIsohedron.h"
#include "API/SRMerge.h"
#include "API/SRExpand.h"
#include "API/SRBurst.h"
#include "API/SRComposite.h"
#include "API/SFWhite.h"
#include "API/REMaxElim.h"
#include "API/STGGeneric.h"
#include "API/SOSimplex.h"
#include "API/SOBalance.h"
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

    SphereTreeMethodMedial::SphereTreeMethodMedial(const std::string &config_path) {
        YAML::Node doc_full = YAML::LoadFile(config_path);
        m_method_name = "Medial";
        auto doc = doc_full[m_method_name];
        testerLevels = getParam<int>(doc, "TesterLevers", -1);
        branch = getParam<int>(doc, "Branch", 8);
        depth = getParam<int>(doc, "Depth", 3);
        numCoverPts = getParam<int>(doc, "NumCoverPts", 5000);
        minCoverPts = getParam<int>(doc, "MinCoverPts", 5);
        initSpheres = getParam<int>(doc, "InitSpheres", 500);
        erFact = getParam<float>(doc, "ErFact", 2);
        spheresPerNode = getParam<int>(doc, "SpheresPerNode", 100);
        verify = getParam<bool>(doc, "Verify", false);
        nopause = getParam<bool>(doc, "Nopause", false);
        eval = getParam<bool>(doc, "Eval", false);
        useMerge = getParam<bool>(doc, "UseMerge", false);
        useBurst = getParam<bool>(doc, "UseBurst", false);
        useExpand = getParam<bool>(doc, "UseExpand", false);
        optimise = static_cast<Optimiser>(getParam(doc, "Optimise", 0));
        balExcess = getParam<float>(doc, "BalExcess", 0.0);
        maxOptLevel = getParam<int>(doc, "MaxOptLevel", -1);

        //  default to combined algorithm
        if (!useMerge && !useBurst && !useExpand)
            useMerge = useExpand = true;

    }

    SphereTreeUniquePtr SphereTreeMethodMedial::create(const std::string &config_path) {
        return bot_common::AlgorithmFactory<SphereTreeMethodBase, const std::string &>::CreateAlgorithm(
                SphereTreeMethodMedialName, config_path);
    }

    bot_common::ErrorInfo SphereTreeMethodMedial::constructTree(Surface &sur, MySphereTree &tree) {
        float boxScale = sur.fitIntoBox(1000);
        MedialTester mt;
        mt.setSurface(sur);
        mt.useLargeCover = true;

        SEConvex convEval;
        convEval.setTester(mt);
        SEBase *eval_ = &convEval;

        Array<Point3D> sphPts;
        SESphPt sphEval;

        //  <= 0 will use convex tester
        if (testerLevels > 0) {
            SSIsohedron::generateSamples(&sphPts, testerLevels - 1);
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
        PLOGD << coverPts.getSize() << "cover points";

        /*
           Setup voronoi diagram
        */
        Point3D pC {};
        pC.x = (sur.pMax.x + sur.pMin.x) / 2.0;
        pC.y = (sur.pMax.y + sur.pMin.y) / 2.0;
        pC.z = (sur.pMax.z + sur.pMin.z) / 2.0;

        Voronoi3D vor;
        vor.initialise(pC, 1.5 * sur.pMin.distance(pC));

        /*
            setup adaptive Voronoi algorithm
        */
        VFAdaptive adaptive;
        adaptive.mt = &mt;
        adaptive.eval = eval_;

        /*
            setup FITTER
        */
        SFWhite fitter;

        /*
            setup MERGE
        */
        SRMerge merger;
        merger.sphereEval = eval_;
        merger.sphereFitter = &fitter;
        merger.useBeneficial = true;
        merger.doAllBelow = branch * 3; // DO_ALL_BELOW = branch * 3
        merger.setup(&vor, &mt);
        merger.vorAdapt = &adaptive;
        merger.initSpheres = initSpheres;
        merger.errorDecreaseFactor = erFact;
        merger.minSpheresPerNode = spheresPerNode;
        merger.maxItersForVoronoi = static_cast<int>(1.50 * spheresPerNode);

        /*
            setup BURST
        */
        SRBurst burster;
        burster.sphereEval = eval_;
        burster.sphereFitter = &fitter;
        burster.useBeneficial = true;
        burster.doAllBelow = branch * 3;
        burster.setup(&vor, &mt);

        if (!useBurst) {
            //  setup adaptive algorithm
            burster.vorAdapt = &adaptive;
            burster.initSpheres = initSpheres;
            burster.errorDecreaseFactor = erFact;
            burster.minSpheresPerNode = spheresPerNode;
            burster.maxItersForVoronoi = static_cast<int>(1.50 * spheresPerNode);
        } else {
            // leave adaptive algorithm out as merge will do it for us
            burster.vorAdapt = nullptr;
        }

        /*
            setup EXPAND generator
        */
        REMaxElim elimME;
        SRExpand expander;
        expander.redElim = &elimME;
        expander.setup(&vor, &mt);
        expander.errStep = 100;
        expander.useIterativeSelect = false;
        expander.relTol = static_cast<float>(1E-5);
        if (!useMerge && !useBurst) {
            //  setup adaptive algorithm
            expander.vorAdapt = &adaptive;
            expander.initSpheres = initSpheres;
            expander.errorDecreaseFactor = erFact;
            expander.minSpheresPerNode = spheresPerNode;
            expander.maxItersForVoronoi = static_cast<int>(1.50 * spheresPerNode);
        } else {
            // leave adaptive algorithm out as previous algs will do it for us
            expander.vorAdapt = nullptr;
        }

        /*
            setup the COMPOSITE algorithm
        */
        SRComposite composite;
        composite.eval = eval_;
        if (useMerge)
            composite.addReducer(&merger);
        if (useBurst)
            composite.addReducer(&burster);
        if (useExpand)
            composite.addReducer(&expander);

        /*
            setup simplex optimiser in case we want it
        */
        SOSimplex simOpt;
        simOpt.sphereEval = eval_;
        simOpt.sphereFitter = &fitter;

        /*
            setup balance optimiser to throw away spheres as long as
            increase in error is less than balExcess e.g. 0.05 is 1.05%
        */
        SOBalance balOpt;
        balOpt.sphereEval = eval_;
        balOpt.optimiser = &simOpt;
        balOpt.V = 0.0f;
        balOpt.A = 1;
        balOpt.B = balExcess;

        /*
            setup SphereTree constructor - using dynamic construction
        */
        STGGeneric treegen;
        treegen.eval = eval_;
        treegen.useRefit = true;
        treegen.setSamples(coverPts);
        treegen.reducer = &composite;
        treegen.maxOptLevel = maxOptLevel;
        if (optimise == SIMPLEX)
            treegen.optimiser = &simOpt;
        else if (optimise == BALANCE)
            treegen.optimiser = &balOpt;

        /*
            make sphere-tree
        */
        SphereTree m_tree;
        m_tree.setupTree(branch, depth + 1);

        treegen.constructTree(&m_tree);
        tree.setBySphereTree(m_tree, 1.0 / boxScale);
        return bot_common::ErrorInfo::OK();
    }


}