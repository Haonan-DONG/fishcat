#include <base/log.h>
#include <iostream>

#include "calibration/intrinsic_calibration.h"

namespace fishcat
{

    void IntrinsicCalibrationHelp()
    {
        LOG(INFO) << "Official Camera Intrinsic Calibration." << std::endl;
        LOG(INFO) << "Near the sample file you'll find the configuration file, which has detailed help of "
                     "how to edit it.  It may be any OpenCV supported file format XML/YAML."
                  << std::endl;
    }

} // namespace fishcat
