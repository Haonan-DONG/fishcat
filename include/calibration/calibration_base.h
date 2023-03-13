#ifndef CALIBRATION_BASE_H_
#define CALIBRATION_BASE_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <unordered_map>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#define WIDTH 5000
#define HEIGHT 4000
#define VALID_HEIGHT 3800

namespace fishcat
{

    class CalibrationSettings
    {
    public:
        CalibrationSettings() : good_input_(false) {}

        enum CalibrationPattern
        {
            NOT_EXISTING,
            CHESSBOARD,
            CIRCLES_GRID,
            ASYMMETRIC_CIRCLES_GRID,
            META_BOARD
        };

        enum CalibrationInputType
        {
            INVALID,
            IMAGE_LIST
        };

        void Write(cv::FileStorage &fs) const;
        void Read(const cv::FileNode &node);
        void Interprate();
        cv::Mat NextImage();
        std::vector<cv::Mat> NextImage(bool is_stereo);
        bool ReadStringList(const std::string &filename, std::vector<std::string> &l);

    public:
        cv::Size board_size_;                    // The cv::Size of the board -> Number of items by width and height
        CalibrationPattern calibration_pattern_; // One of the Chessboard, circles, or asymmetric circle pattern
        float square_size_;                      // The cv::Size of a square in your defined unit (point, millimeter,etc.
        float aspect_ratio_;                     // The aspect ratio
        bool bwrite_points_;                     //  Write detected feature points
        bool bwrite_extrinsics_;                 // Write extrinsic parameters
        bool calib_zero_tangent_dist_;           // Assume zero tangential distortion
        bool calib_fix_principal_point_;         // Fix the principal point at the center
        bool flip_vertical_;                     // Flip the captured images around the horizontal axis
        std::string output_fileName_;            // The name of the file where to write
        bool show_undistorsed_;                  // Show undistorted images after calibration
        bool show_partial_board_;
        std::string input_; // The input ->
        std::string input_path_;
        std::string image_path_;
        bool use_fisheye_model_;            // Kannala-Brandt Model is used.
        int calibration_type_;              // 0 for intrinsic and 1 for extrinsic.
        std::string camera_intrinsic_path_; // yaml file for intrinsic path.
        std::string original_fisheye_image_;

        bool bis_stereo_camera_; // If use stereo camera

        int camera_id_;
        std::vector<std::string> image_list_;
        std::vector<std::string> origin_image_list_;
        int at_image_list_;
        cv::VideoCapture input_capture_;
        CalibrationInputType input_type_;
        bool good_input_;
        int flag_;

        std::string center_point_xml_;
        std::string plane_point_xml_;

    private:
        std::string pattern_to_use_;
    };

    enum
    {
        DETECTION = 0,
        CAPTURING = 1,
        CALIBRATED = 2
    };

    // important declaration for ">>" operator in Persistence.hpp
    void read(const cv::FileNode &node, CalibrationSettings &x, const CalibrationSettings &default_value);
}

#endif