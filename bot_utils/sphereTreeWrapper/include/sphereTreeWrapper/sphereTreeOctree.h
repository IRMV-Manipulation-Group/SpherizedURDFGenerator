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

#ifndef SPHERIZEDURDFGENERATOR_SPHERETREEOCTREE_H
#define SPHERIZEDURDFGENERATOR_SPHERETREEOCTREE_H


#include "sphereTreeBase.h"
#include "irmv/bot_common/alg_factory/algorithm_factory.h"

namespace SphereTreeMethod {
    constexpr char SphereTreeMethodOctreeName[] = "SphereTreeOctreeName";

    class SphereTreeMethodOctree : public SphereTreeMethodBase {
    public:
        SphereTreeMethodOctree(const std::string &config_path);

        ~SphereTreeMethodOctree() override = default;

        static SphereTreeUniquePtr create(const std::string &config_path);

        bot_common::ErrorInfo constructTree(const std::string &file, MySphereTree& tree) override;

    protected:
        int depth = 3;              ///<  depth of the sphere-tree
        bool verify = false;        ///<  verify model before construction
        bool nopause = false;       ///<  will we pause before starting
        bool eval = false;          ///<  do we evaluate the sphere-tree after construction
    };

    inline bot_common::REGISTER_ALGORITHM(SphereTreeMethodBase, SphereTreeMethodOctreeName, SphereTreeMethodOctree,
                                          const std::string&);
}

#endif //SPHERIZEDURDFGENERATOR_SPHERETREEOCTREE_H
