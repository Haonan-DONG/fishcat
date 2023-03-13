#ifndef PANORAMIC_STITCHING_
#define PANORAMIC_STITCHING_

#include "calibration/calibration_base.h"

namespace fishcat
{
    void PanoramicStitchingStereo(cv::Mat right_image, cv::Mat left_image, cv::Mat rotation, cv::Mat translation, double fov);
    void FisheyeExpansion(cv::Mat fisheye_image, cv::Mat intrinsic, cv::Mat distortion_coefficient);
    cv::Point2d FisheyeToNormalCylinder(double u, double v, cv::Mat intrinsic, cv::Mat distortion_coefficient);
}
#endif