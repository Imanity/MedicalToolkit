#pragma once

#ifdef DLL_EXPORTS
#define REGISTRATION_CT_DSA_EXPORT __declspec(dllexport)
#else
#define REGISTRATION_CT_DSA_EXPORT __declspec(dllimport)
#endif

#include "../VolumeData/VolumeData.h"

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

class REGISTRATION_CT_DSA_EXPORT Reg {
public:
	Reg();
	~Reg();

	void solve(VolumeData<short> &X, cv::Mat &Y, Eigen::Vector3d &R_vec, Eigen::Vector4d &var, std::string X_title, std::string Y_title, std::string tag);

	VolumeData<short> fuse(VolumeData<short> &X, std::vector<cv::Mat> &front, std::vector<cv::Mat> &right, double dx, double dy);

private:
	Eigen::Matrix3d rotate_vector2matrix(Eigen::Vector3d& v);

	Eigen::Vector2d project(Eigen::Vector3d &X, Eigen::VectorXd &t);
	std::vector<Eigen::Vector2d> project(std::vector<Eigen::Vector3d> &Xs, Eigen::VectorXd &t);
	cv::Mat project(std::vector<Eigen::Vector3d> &Xs, std::vector<Eigen::Vector3d>& Xs_skeleton, cv::Mat &Y, Eigen::VectorXd &t);

	double energy_func(std::vector<Eigen::Vector3d> &Xs, cv::Mat &Y, Eigen::VectorXd &t);

private:
	int v_nx, v_ny, v_nz;
	double v_dx, v_dy, v_dz;
	int i_nx, i_ny;
	double i_dx, i_dy;
	double dis_s_d, dis_s_p;
};

static double foo(const Eigen::VectorXd& x, Eigen::VectorXd& grad);
static double loss_func(const Eigen::VectorXd& t);
extern std::vector<Eigen::Vector3d> Xs_glo;
extern cv::Mat Y_glo;
extern double dx_glo, dy_glo;
extern Eigen::VectorXd t_best_glo;
extern double E_best_glo;
