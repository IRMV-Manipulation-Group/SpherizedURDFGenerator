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



#ifndef BOT_COMMON_ERROR_CODE_H
#define BOT_COMMON_ERROR_CODE_H

#include <string>

namespace bot_common {

    enum ErrorCode {
        OK = 0,
        Error = -1,
/************************HARDWARE -2 ~ -20********************/
        HeadConnectFailed = -2,
        ArmJointCommunicationFailed = -3,
        ArmJointTargetExceedLimits = -4,
        ArmJointTargetSingular = -5,
        ArmRealTimeKernelWrong = -6,
        ArmJointBusWrong = -7,
        ArmPlanningFailed = -8,
        ArmJointVelExceedLimits = -10,
        ArmEndBoardConnectionWrong = -11,
        ArmVelExceedLimits = -12,
        ArmAccExceedLimits = -13,
        ArmBrakeHold = -14,
        ArmTeachTooFast = -15,
        ArmCollisionHappend = -16,
        ArmNoSuchWorkFrame = -17,
        ArmNoSuchToolFrame = -18,
        ArmNotEnabled = -19,
        ArmControllerTemperatureHigh = -20,
        ArmControllerCurrentHigh = -21,
        ArmControllerCurrentLow = -22,
        ArmControllerVoltageHigh = -23,
        ArmControllerVolatgeLow = -24,
        ArmRealTimeKernelCommunicationWrong = -25,
        /**************Joint Flag *************/
        ArmJointFOCWrong = -26,
        ArmJointVoltageHigh = -27,
        ArmJointVoltageLow = -28,
        ArmJointTemperatureHigh = -29,
        ArmJointSetupFailed = -30,
        ArmJointEncoderWorng = -31,
        ArmJointCurrentHigh = -32,
        ArmJointSoftwareWrong = -33,
        ArmJointTemperatureSensorWrong = -34,
        ArmJointExceedLimits = -35,
        ArmJointIndexWrong = -36,
        ArmJointServoWrong = -37,
        ArmJointCurrentWrong = -38,
        ArmJointBrakeHold = -39,
        ArmJointCmdsStep  = -40,
        ArmJointLosLoop = -41,
        ArmJointConnectionDropFrames = -42,
/***********************SOFTWARRE********************/
        ROSError = -43,
        IkExceedMaxDis = -44,
        IKFailed = -45,
        CartesianPlanningFailed = -46,
        TrajectoryPlanningFailed = -47,
        /***************VALIDATOR -20 ~ ******************/
        EmptyPath = -48,
        InCollision = -49,
        OutLimitation = -50,
        NotValid = -51,
        CartesianPlanningTimeout = -52,


        /**************CONTROL******************/
        ExecuteFailed = -100,
        RobotConnectFailed = -101,
        ServerConnectFailed = -102,
        ArmMoving = -103,
        EmergencyStop = -104,
    };

    class ErrorInfo {

    public:
        ErrorInfo() : error_code_(ErrorCode::OK), error_msg_("all ok") {};

        ErrorInfo(ErrorCode error_code, const std::string &error_msg) : error_code_(error_code),
                                                                        error_msg_(error_msg) {};

        ErrorInfo &operator=(const ErrorInfo &error_info) {
            if (&error_info != this) {
                error_code_ = error_info.error_code_;
                error_msg_ = error_info.error_msg_;
            }
            return *this;
        }

        static ErrorInfo OK() {
            return ErrorInfo();
        }

        ErrorCode error_code() const { return error_code_; };

        const std::string &error_msg() const { return error_msg_; }

        bool operator==(const ErrorInfo &rhs) {
            return error_code_ == rhs.error_code();
        }

        bool IsOK() const {
            return (error_code_ == ErrorCode::OK);
        }

        ~ErrorInfo() = default;

    private:
        ErrorCode error_code_;
        std::string error_msg_;


    };

} //namespace bot_common

#endif //BOT_COMMON_ERROR_CODE_H
