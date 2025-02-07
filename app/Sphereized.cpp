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
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " -i <input_urdf_path> -o <output_urdf_path> [-r <key> <value> ...] [--simplify <0|1>]" << std::endl;
        return 1;
    }

    std::string configPath = SpherizedURDFGenerator_CONFIG_PATH;
    std::string inputPath;
    std::string outputPath;
    std::vector<std::pair<std::string, std::string>> replacements;
    bool simplify = true;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputPath = argv[++i];
        } else if (arg == "-r" && i + 2 < argc) {
            // Parse replacement pairs
            std::string key = argv[++i];
            std::string value = argv[++i];
            replacements.emplace_back(key, value);
        }else if (arg == "--simplify" && i + 1 < argc) {
            simplify = std::stoi(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    // Check if input and output paths are provided
    if (inputPath.empty() || outputPath.empty()) {
        std::cerr << "Error: Missing input or output path." << std::endl;
        return 1;
    }

    // Create the SphereTreeURDFGenerator instance
    auto spherized_generator = std::make_shared<SphereTreeURDFGenerator>(
            configPath + "/sphereTree/sphereTreeConfig.yml", simplify);

    // Run the generator with the input, output, and replacement pairs
    auto ret = spherized_generator->run(inputPath, outputPath, replacements);
    std::cout << "Processing " << inputPath << " -> " << outputPath << ": " << ret.error_msg() << std::endl;

    return 0;
}

