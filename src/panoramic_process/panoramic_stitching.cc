#include <math.h>

#include "base/log.h"
#include "calibration/calibration_base.h"
#include "panoramic_process/panoramic_stitching.h"

namespace fishcat
{
    void PanoramicStitchingStereo(cv::Mat right_image, cv::Mat left_image, cv::Mat right_intrinsic, cv::Mat left_intrinsic, cv::Mat rotation, cv::Mat translation, double fov)
    {
    }

    void FisheyeExpansion(cv::Mat fisheye_image, cv::Mat intrinsic, cv::Mat distortion_coefficient)
    {
        cv::Mat expanded_cylinder_image = cv::Mat::zeros(1000, 2000, CV_8UC3);
        cv::Size image_size = fisheye_image.size();

        for (int i = 0; i < image_size.height; i++)
        {
            for (int j = 0; j < image_size.width; j++)
            {

                cv::Point2d normalized_coord = FisheyeToNormalCylinder(i, j, intrinsic, distortion_coefficient);

                normalized_coord.x = (normalized_coord.x + 1) * 1000;
                normalized_coord.y = (normalized_coord.y + 1) * 500;

                if (normalized_coord.x > 0 && normalized_coord.y > 0 && normalized_coord.x < expanded_cylinder_image.cols && normalized_coord.y < expanded_cylinder_image.rows)
                {
                    cv::Vec3b color_value = fisheye_image.at<cv::Vec3b>(i, j);
                    expanded_cylinder_image.at<cv::Vec3b>(normalized_coord) = color_value;
                }
            }
        }
        LOG(INFO) << "Saving the expanded image."
                  << std::endl;

        cv::imwrite("cylinder_expanded_image.jpg", expanded_cylinder_image);
    }

    // geometry
    cv::Point2d FisheyeToNormalCylinder(double u, double v, cv::Mat intrinsic, cv::Mat distortion_coefficient)
    {
        cv::Point2d normal_cylinder_xy;
        const double &fx = intrinsic.at<double>(0, 0);
        const double &fy = intrinsic.at<double>(1, 1);
        const double &cx = intrinsic.at<double>(0, 2);
        const double &cy = intrinsic.at<double>(1, 2);
        const double &k1 = distortion_coefficient.at<double>(0, 0);
        const double &k2 = distortion_coefficient.at<double>(0, 1);
        const double &k3 = distortion_coefficient.at<double>(0, 2);
        const double &k4 = distortion_coefficient.at<double>(0, 3);
        double x_d = (u - cx) / fx;
        double y_d = (v - cy) / fy;
        double r_d = std::sqrt(y_d * y_d + x_d * x_d);
        double phi = atan2(y_d, x_d);
        double theta = r_d; // equidistance projection, and focal is unit.

        // find suitable theta by least square.
        double residual = 10000;
        double theta2 = theta * theta, theta3 = theta2 * theta, theta4 = theta2 * theta2, theta5 = theta4 * theta;
        double theta6 = theta3 * theta3, theta7 = theta6 * theta, theta8 = theta4 * theta4, theta9 = theta8 * theta;
        while (residual > 0.0001)
        {
            residual = std::pow((r_d - (theta + k1 * theta3 + k2 * theta5 + k3 * theta7 + k4 * theta9)), 2);
            double delta = (r_d - (theta + k1 * theta3 + k2 * theta5 + k3 * theta7 + k4 * theta9)) / (1 + 3 * k1 * theta2 + 5 * k2 * theta4 + 7 * k3 * theta6 + 9 * k4 * theta8);
            theta = theta + delta;
            theta2 = theta * theta;
            theta3 = theta2 * theta;
            theta4 = theta2 * theta2;
            theta5 = theta4 * theta;
            theta6 = theta3 * theta3;
            theta7 = theta6 * theta;
            theta8 = theta4 * theta4;
            theta9 = theta8 * theta;

            // Bugs for not converge.
            if (theta > (90 * 3.1415926 / 180))
            {
                theta = (90 * 3.1415926 / 180);
                break;
            }
        }

        // calculate latitude and longitude from theta and phi.
        // Note here use the simplified form.
        double longitude = phi;
        double latitude = 3.1415926 / 2 - theta; // bug for pi.

        // calculate the normal cylinder coordinate.
        normal_cylinder_xy.x = longitude / 3.1415926;
        normal_cylinder_xy.y = 2 * latitude / 3.1415926;

        return normal_cylinder_xy;
    }
}