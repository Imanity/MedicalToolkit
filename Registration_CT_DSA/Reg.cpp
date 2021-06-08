#define DLL_EXPORTS

#include <fstream>
#include <cstdlib>
#include <ctime>

#include "Reg.h"
#include "LBFGS.h"

#include "../Skeletonize/Skeletonize.h"

std::vector<Eigen::Vector3d> Xs_glo;
cv::Mat Y_glo;
double dx_glo, dy_glo;
Eigen::VectorXd t_best_glo;
double E_best_glo;

Reg::Reg() {
	//
}

Reg::~Reg() {
	//
}

void Reg::solve(VolumeData<short> &X, cv::Mat& Y, Eigen::Vector3d& R_vec, Eigen::Vector4d& var, std::string X_title, std::string Y_title, std::string tag) {
	std::cout << std::endl << "Register: " << X_title << " & " << Y_title << std::endl;
	
	// 读取信息
	v_nx = X.nx, v_ny = X.ny, v_nz = X.nz;
	v_dx = X.dx, v_dy = X.dy, v_dz = X.dz;
	i_nx = Y.cols, i_ny = Y.rows;
	i_dx = var(0), i_dy = var(1);
	dis_s_d = var(2), dis_s_p = var(3);

	// 初值
	Eigen::Matrix3d R = rotate_vector2matrix(R_vec);

	Eigen::Vector3d voxel_center;
	voxel_center << v_dx * v_nx / 2, v_dy * v_ny / 2, v_dz * v_nz / 2;
	Eigen::Vector3d voxel_projected = R * voxel_center;
	Eigen::Vector3d T;
	T << -voxel_projected(0), -voxel_projected(1), dis_s_p;

	Eigen::VectorXd t(7);
	t << R_vec(0), R_vec(1), R_vec(2), T(0), T(1), T(2), dis_s_d;

	// 体数据骨架提取
	VolumeData<short> X_skeleton(v_nx, v_ny, v_nz, v_dx, v_dy, v_dz);
	int dims[] = { v_nx, v_ny, v_nz };
	Skeletonize thinner(X.data, X_skeleton.data, dims);
	thinner.thinning();

	// 体数据->点云
	std::vector<Eigen::Vector3d> Xs;
	for (int i = 0; i < X.nvox; ++i) {
		if (X.data[i]) {
			Eigen::Vector3i voxel_idx = X.coord(i);
			Eigen::Vector3d voxel;
			voxel << v_dx * voxel_idx(0), v_dy* voxel_idx(1), v_dz* voxel_idx(2);
			Xs.push_back(voxel);
		}
	}

	std::vector<Eigen::Vector3d> Xs_skeleton;
	for (int i = 0; i < X_skeleton.nvox; ++i) {
		if (X_skeleton.data[i]) {
			Eigen::Vector3i voxel_idx = X_skeleton.coord(i);
			Eigen::Vector3d voxel;
			voxel << v_dx * voxel_idx(0), v_dy * voxel_idx(1), v_dz * voxel_idx(2);
			Xs_skeleton.push_back(voxel);
		}
	}

	// 可视化配准前
	//cv::imwrite("../" + X_title + " & " + Y_title + "_before.png", project(Xs, Xs_skeleton, Y, t));
	//cv::imshow(X_title + " & " + Y_title + "_before", project(Xs, Xs_skeleton, Y, t));

	// Dirty
	if (tag.find('f') != Y_title.npos) {
		t(3) += 5.0;
		t(4) -= 8.0;
	}

	clock_t start_time = clock();

	// 模拟退火配准
	double temp = 10.0;
	double step[] = { 0.05, 0.05, 0.05, 0.1, 0.1, 0.1, 1.6 };
	double lambda = 0.99;
	double E = energy_func(Xs_skeleton, Y, t);
	double E_best = E;
	Eigen::VectorXd t_best = t;
	int Time = 0;

	while (Time < 10000) {
		int morph_pos = rand() % 7;
		double morph_step = step[morph_pos] * rand() / RAND_MAX;
		if (rand() % 2)
			morph_step = -morph_step;
		Eigen::VectorXd t_ = t;
		t_(morph_pos) += morph_step;
		double E_ = energy_func(Xs_skeleton, Y, t_);
		double delta_E = E_ - E;
		std::cout << "Iter: " << Time << " / 10000" << "\r";
		if (delta_E > 0 || (double)rand() / RAND_MAX < exp(delta_E / temp)) {
			E = E_;
			t = t_;
			if (E_ > E_best) {
				E_best = E;
				t_best = t;
			}
		}
		temp = 10.0 / (1.0 + Time);
		Time += 1;
	}

	// Powell 配准
	/*
	Xs_glo.clear();
	for (int i = 0; i < Xs_skeleton.size(); ++i)
		Xs_glo.push_back(Xs_skeleton[i]);
	Y_glo = Y.clone();
	dx_glo = i_dx;
	dy_glo = i_dy;

	LBFGSpp::LBFGSParam<double> param;
	param.max_iterations = 1000;
	LBFGSpp::LBFGSSolver<double> solver(param);

	double fx;
	E_best_glo = 0.0;
	t_best_glo = t;
	int niter = solver.minimize(foo, t, fx);

	double E_best = E_best_glo;
	Eigen::VectorXd t_best = t_best_glo;
	*/

	clock_t end_time = clock();

	double err = 0;
	std::vector<Eigen::Vector2d> Ys = project(Xs_skeleton, t_best);
	for (int i = 0; i < Ys.size(); ++i) {
		double minErr = DBL_MAX;
		for (int j = 0; j < Y.cols; ++j) for (int k = 0; k < Y.rows; ++k) {
			if (!Y.at<uchar>(k, j))
				continue;
			Eigen::Vector2d y;
			y << ((double)j - i_nx / 2) * i_dx, ((double)k - i_ny / 2)* i_dy;
			double currErr = sqrt((Ys[i](0) - y(0)) * (Ys[i](0) - y(0)) + (Ys[i](1) - y(1)) * (Ys[i](1) - y(1)));
			minErr = std::min(minErr, currErr);
		}
		err += minErr;
	}
	err /= Ys.size();

	std::cout << "配准用时: " << (double)(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << std::endl;
	std::cout << "配准成功占比: " << E_best << std::endl;
	std::cout << "配准误差: " << err << std::endl;

	// 可视化配准结果
	//cv::imwrite("../" + X_title + " & " + Y_title + "_after.png", project(Xs, Xs_skeleton, Y, t_best));
	cv::imshow(X_title + " & " + Y_title + "_after", project(Xs, Xs_skeleton, Y, t_best));

	// 保存配准参数
	std::ofstream outfile("../reg_result.txt", std::ios::app);
	for (int i = 0; i < 7; ++i) {
		outfile << t_best(i) << " ";
	}
	outfile << std::endl;
	outfile.close();
}

VolumeData<short> Reg::fuse(VolumeData<short>& X, std::vector<cv::Mat>& front, std::vector<cv::Mat>& right, double dx, double dy) {
	Eigen::VectorXd t_front(7), t_right(7);

	// 读取配准参数
	std::ifstream infile("../reg_result.txt");
	for (int i = 0; i < 7; ++i) {
		double x = 0;
		infile >> x;
		t_front(i) = x;
	}
	for (int i = 0; i < 7; ++i) {
		double x = 0;
		infile >> x;
		t_right(i) = x;
	}
	infile.close();

	// DSA图像预处理
	int n = std::min(front.size(), right.size());

	// 创建多帧图像
	VolumeData<short> V(X.nx, X.ny, X.nz, X.dx, X.dy, X.dz);
	for (int i = 0; i < V.nvox; ++i) {
		V.data[i] = 0;
	}

	// 融合
	for (int i = 0; i < X.nvox; ++i) {
		if (!X.data[i])
			continue;

		Eigen::Vector3i X_coord = X.coord(i);

		Eigen::Vector3d X_pos;
		X_pos << X.dx * X_coord(0), X.dy * X_coord(1), X.dz * X_coord(2);

		Eigen::Vector2d Y_front = project(X_pos, t_front);
		Eigen::Vector2d Y_right = project(X_pos, t_right);

		for (int j = 0; j < n; ++j) {
			double x_front = Y_front(0) / dx + front[j].cols / 2, y_front = Y_front(1) / dy + front[j].rows / 2;
			double x_right = Y_right(0) / dx + right[j].cols / 2, y_right = Y_right(1) / dy + right[j].rows / 2;
			if ((int)x_front < 0 || (int)x_front >= front[j].cols)
				continue;
			if ((int)y_front < 0 || (int)y_front >= front[j].rows)
				continue;
			if ((int)x_right < 0 || (int)x_right >= right[j].cols)
				continue;
			if ((int)y_right < 0 || (int)y_right >= right[j].rows)
				continue;

			if (front[j].at<uchar>(y_front, x_front) && right[j].at<uchar>(y_right, x_right)) {
				V.data[i] = V.data[i] | (1 << j);
			}
		}
	}

	return V;
}

Eigen::Matrix3d Reg::rotate_vector2matrix(Eigen::Vector3d& v) {
	double x = v(0), y = v(1), z = v(2);
	Eigen::Matrix3d Rx, Ry, Rz;
	Rx << 1.0, 0.0, 0.0, 0.0, cos(x), -sin(x), 0.0, sin(x), cos(x);
	Ry << cos(y), 0.0, sin(y), 0.0, 1.0, 0.0, -sin(y), 0.0, cos(y);
	Rz << cos(z), -sin(z), 0.0, sin(z), cos(z), 0.0, 0.0, 0.0, 1.0;
	return Rx * Ry * Rz;
}

Eigen::Vector2d Reg::project(Eigen::Vector3d& X, Eigen::VectorXd& t) {
	Eigen::Vector3d R_vec;
	R_vec << t(0), t(1), t(2);
	Eigen::Matrix3d R = rotate_vector2matrix(R_vec);

	Eigen::Vector3d T;
	T << t(3), t(4), t(5);

	double N = t(6);

	Eigen::Vector3d X0 = R * X + T;
	Eigen::Vector2d Y;
	Y << X0(0) * N / X0(2), X0(1)* N / X0(2);
	return Y;
}

std::vector<Eigen::Vector2d> Reg::project(std::vector<Eigen::Vector3d>& Xs, Eigen::VectorXd& t) {
	std::vector<Eigen::Vector2d> Ys;
	for (int i = 0; i < Xs.size(); ++i)
		Ys.push_back(project(Xs[i], t));
	return Ys;
}

cv::Mat Reg::project(std::vector<Eigen::Vector3d> &Xs, std::vector<Eigen::Vector3d>& Xs_skeleton, cv::Mat &Y, Eigen::VectorXd &t) {
	cv::Mat fuse(i_ny, i_nx, CV_8UC3, cv::Scalar::all(255));
	for (int y = 0; y < i_ny; ++y) for (int x = 0; x < i_nx; ++x) {
		if (Y.at<uchar>(y, x)) {
			fuse.at<cv::Vec3b>(y, x)[1] = fuse.at<cv::Vec3b>(y, x)[2] = 0;
			fuse.at<cv::Vec3b>(y, x)[0] = 255;
		}
	}

	std::vector<Eigen::Vector2d> Ys = project(Xs, t);
	for (int i = 0; i < Ys.size(); ++i) {
		double x = Ys[i](0) / i_dx + i_nx / 2;
		double y = Ys[i](1) / i_dy + i_ny / 2;
		if ((int)x < 0 || (int)x >= i_nx)
			continue;
		if ((int)y < 0 || (int)y >= i_ny)
			continue;
		fuse.at<cv::Vec3b>(y, x)[1] = 255;
		fuse.at<cv::Vec3b>(y, x)[2] = 0;
	}

	std::vector<Eigen::Vector2d> Ys_skeleton = project(Xs_skeleton, t);
	for (int i = 0; i < Ys_skeleton.size(); ++i) {
		double x = Ys_skeleton[i](0) / i_dx + i_nx / 2;
		double y = Ys_skeleton[i](1) / i_dy + i_ny / 2;
		if ((int)x < 0 || (int)x >= i_nx)
			continue;
		if ((int)y < 0 || (int)y >= i_ny)
			continue;
		fuse.at<cv::Vec3b>(y, x)[2] = 255;
		fuse.at<cv::Vec3b>(y, x)[1] = fuse.at<cv::Vec3b>(y, x)[0] = 0;
	}
	return fuse;
}

double Reg::energy_func(std::vector<Eigen::Vector3d>& Xs, cv::Mat& Y, Eigen::VectorXd& t) {
	int num = 0;

	std::vector<Eigen::Vector2d> Ys = project(Xs, t);
	for (int i = 0; i < Xs.size(); ++i) {
		double x = Ys[i](0) / i_dx + i_nx / 2;
		double y = Ys[i](1) / i_dy + i_ny / 2;
		if ((int)x < 0 || (int)x >= i_nx)
			continue;
		if ((int)y < 0 || (int)y >= i_ny)
			continue;
		if (Y.at<uchar>(y, x))
			num += 1;
	}

	return (double)num / Xs.size();
}

double foo(const Eigen::VectorXd& x, Eigen::VectorXd& grad) {
	double loss = loss_func(x);
	double grad_delta[] = { 0.05, 0.05, 0.05, 0.1, 0.1, 0.1, 1.6 };
	grad = Eigen::VectorXd::Zero(7);
	for (int i = 0; i < 7; ++i) {
		Eigen::VectorXd x_ = x;
		x_(i) += grad_delta[i];
		grad(i) = (loss_func(x_) - loss) / grad_delta[i];
	}
	return loss;
}

double loss_func(const Eigen::VectorXd& t) {
	std::vector<Eigen::Vector2d> Ys;
	for (int i = 0; i < Xs_glo.size(); ++i) {
		double x = t(0), y = t(1), z = t(2);
		Eigen::Matrix3d Rx, Ry, Rz;
		Rx << 1.0, 0.0, 0.0, 0.0, cos(x), -sin(x), 0.0, sin(x), cos(x);
		Ry << cos(y), 0.0, sin(y), 0.0, 1.0, 0.0, -sin(y), 0.0, cos(y);
		Rz << cos(z), -sin(z), 0.0, sin(z), cos(z), 0.0, 0.0, 0.0, 1.0;
		Eigen::Matrix3d R = Rx * Ry * Rz;

		Eigen::Vector3d T;
		T << t(3), t(4), t(5);

		double N = t(6);

		Eigen::Vector3d X0 = R * Xs_glo[i] + T;
		Eigen::Vector2d y_;
		y_ << X0(0) * N / X0(2), X0(1)* N / X0(2);
		Ys.push_back(y_);
	}

	int num = 0;
	for (int i = 0; i < Ys.size(); ++i) {
		double x = Ys[i](0) / dx_glo + Y_glo.cols / 2;
		double y = Ys[i](1) / dy_glo + Y_glo.rows / 2;
		if ((int)x < 0 || (int)x >= Y_glo.cols)
			continue;
		if ((int)y < 0 || (int)y >= Y_glo.rows)
			continue;
		if (Y_glo.at<uchar>(y, x))
			num += 1;
	}
	if ((double)num / Ys.size() > E_best_glo) {
		E_best_glo = (double)num / Ys.size();
		t_best_glo = t;
	}

	return 1.0 - (double)num / Ys.size();
}
