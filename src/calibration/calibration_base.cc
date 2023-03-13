#include <iostream>

#include "calibration/calibration_base.h"
#include "base/log.h"

namespace fishcat
{
    // since the static is used only for intrinsic calibration.
    void read(const cv::FileNode &node, CalibrationSettings &x, const CalibrationSettings &default_value = CalibrationSettings())
    {
        if (node.empty())
            x = default_value;
        else
            x.Read(node);
    }

    void CalibrationSettings::Write(cv::FileStorage &fs) const // Write serialization for this class
    {
        fs << "{"
           << "BoardSize_Width" << board_size_.width
           << "BoardSize_Height" << board_size_.height
           << "Square_Size" << square_size_
           << "Calibrate_Pattern" << pattern_to_use_
           << "Calibrate_FixAspectRatio" << aspect_ratio_
           << "Calibrate_AssumeZeroTangentialDistortion" << calib_zero_tangent_dist_
           << "Calibrate_FixPrincipalPointAtTheCenter" << calib_fix_principal_point_

           << "Write_DetectedFeaturePoints" << bwrite_points_
           << "Write_extrinsicParameters" << bwrite_extrinsics_
           << "Write_outputFileName" << output_fileName_

           << "Show_UndistortedImage" << show_undistorsed_
           << "Calibrate_UseFisheyeModel" << use_fisheye_model_

           << "Input_FlipAroundHorizontalAxis" << flip_vertical_
           << "Input_Path" << input_path_
           << "Image_Path" << image_path_
           << "Input" << input_
           << "}";
    }

    void CalibrationSettings::Read(const cv::FileNode &node) // Read serialization for this class
    {
        node["BoardSize_Width"] >> board_size_.width;
        node["BoardSize_Height"] >> board_size_.height;
        node["Calibrate_Pattern"] >> pattern_to_use_;
        node["Square_Size"] >> square_size_;
        node["Calibrate_FixAspectRatio"] >> aspect_ratio_;
        node["Write_DetectedFeaturePoints"] >> bwrite_points_;
        node["Write_extrinsicParameters"] >> bwrite_extrinsics_;
        node["Write_outputFileName"] >> output_fileName_;
        node["Calibrate_AssumeZeroTangentialDistortion"] >> calib_zero_tangent_dist_;
        node["Calibrate_FixPrincipalPointAtTheCenter"] >> calib_fix_principal_point_;
        node["Input_FlipAroundHorizontalAxis"] >> flip_vertical_;
        node["Show_UndistortedImage"] >> show_undistorsed_;
        node["Show_Incomplete_Board"] >> show_partial_board_;
        node["Input"] >> input_;
        node["Input_Path"] >> input_path_;
        node["Image_Path"] >> image_path_;
        node["Calibrate_UseFisheyeModel"] >> use_fisheye_model_;
        node["Calibration_Type"] >> calibration_type_;

        if (calibration_type_ > 0)
        {
            node["Camera_Intrinsic_Path"] >> camera_intrinsic_path_;
        }

        input_ = input_path_ + input_;
        original_fisheye_image_ = input_path_ + original_fisheye_image_;
        output_fileName_ = input_path_ + output_fileName_;
        Interprate();
    }

    void CalibrationSettings::Interprate()
    {
        good_input_ = true;
        if (board_size_.width <= 0 || board_size_.height <= 0)
        {
            LOG(ERROR) << "Invalid Board size: " << board_size_.width << " " << board_size_.height << std::endl;
            good_input_ = false;
        }

        if (square_size_ <= 10e-6)
        {
            LOG(ERROR) << "Invalid square size " << square_size_ << std::endl;
            good_input_ = false;
        }

        if (input_.empty()) // Check for valid input
            input_type_ = INVALID;
        else
        {
            ReadStringList(input_, image_list_);
            input_type_ == IMAGE_LIST;
        }

        if (input_type_ == INVALID)
        {
            LOG(ERROR) << " Inexistent input_: " << input_;
            good_input_ = false;
        }

        if (use_fisheye_model_)
        {
            LOG(INFO) << "The fisheye model will be used as the basic projective model."
                      << std::endl;
            flag_ = 0;
            flag_ |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
            // flag_ |= cv::fisheye::CALIB_CHECK_COND;
            flag_ |= cv::fisheye::CALIB_FIX_SKEW;
        }
        else
        {
            LOG(INFO) << "The pinhole model will be used as the basic projective model."
                      << std::endl;
            flag_ = 0;
            if (calib_fix_principal_point_)
                flag_ |= cv::CALIB_FIX_PRINCIPAL_POINT;
            if (calib_zero_tangent_dist_)
                flag_ |= cv::CALIB_ZERO_TANGENT_DIST;
            if (aspect_ratio_)
                flag_ |= cv::CALIB_FIX_ASPECT_RATIO;
        }

        calibration_pattern_ = NOT_EXISTING;
        if (!pattern_to_use_.compare("CHESSBOARD"))
            calibration_pattern_ = CHESSBOARD;
        if (!pattern_to_use_.compare("CIRCLES_GRID"))
            calibration_pattern_ = CIRCLES_GRID;
        if (!pattern_to_use_.compare("ASYMMETRIC_CIRCLES_GRID"))
            calibration_pattern_ = ASYMMETRIC_CIRCLES_GRID;
        if (calibration_pattern_ == NOT_EXISTING)
        {
            LOG(ERROR) << " Inexistent camera calibration mode: " << pattern_to_use_ << std::endl;
            good_input_ = false;
        }
        at_image_list_ = 0;
    }

    cv::Mat CalibrationSettings::NextImage()
    {
        cv::Mat result;
        std::string image_path = "";
        if (at_image_list_ < (int)image_list_.size())
        {
            image_path = image_path_ + image_list_[at_image_list_++];
            result = cv::imread(image_path, cv::IMREAD_COLOR);
        }
        return result;
    }

    bool CalibrationSettings::ReadStringList(const std::string &filename, std::vector<std::string> &l)
    {
        l.clear();
        cv::FileStorage fs(filename, cv::FileStorage::READ);
        if (!fs.isOpened())
            return false;
        cv::FileNode n = fs.getFirstTopLevelNode();
        if (n.type() != cv::FileNode::SEQ)
            return false;
        cv::FileNodeIterator it = n.begin(), it_end = n.end();
        for (; it != it_end; ++it)
            l.push_back((std::string)*it);
        return true;
    }
}