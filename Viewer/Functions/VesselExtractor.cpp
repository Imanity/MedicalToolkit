#include <vtkMath.h>
#include <Eigen/Dense>
#include <queue>
#include <array>
#include <vector>

#include "VesselExtractor.h"

VesselExtractor::VesselExtractor() {
}

VesselExtractor::~VesselExtractor() {
}

VolumeData<short> VesselExtractor::getOutput(VolumeData<short> &v1, VolumeData<short> &v2) {
	using std::abs;
	using std::swap;

	if (v1.nx != v2.nx || v1.ny != v2.ny || v1.nz != v2.nz || v1.dx != v2.dx || v1.dy != v2.dy || v1.dz != v2.dz) {
		VolumeData<short> errV;
		return errV;
	}

	VolumeData<short> v(v1.nx, v1.ny, v1.nz, v1.dx, v1.dy, v1.dz);

	int width = v1.nx, height = v1.ny, depth = v1.nz;
	int r = 0;
	short threshold_min = 10, threshold_max = 2000;
	short max_val = 0;
	#pragma omp parallel for schedule(guided)
	for (int i = r; i < width - r; ++i) {
		for (int j = r; j < height - r; ++j) for (int k = r; k < depth - r; ++k) {
			short val1 = v1.at(i, j, k), val2 = v2.at(i, j, k);

			if (val2 < threshold_min || val2 > threshold_max)
				v.set(i, j, k, 0);
			else {
				short post_val = std::max(0, val2 - val1);
				#pragma omp critical
				{
					if (post_val > max_val)
						max_val = post_val;
				}
				v.set(i, j, k, post_val);
			}
		}
	}

	short disp_val = 3999;
	#pragma omp parallel for schedule(guided)
	for (int i = 0; i < width; ++i) {
		for (int j = 0; j < height; ++j) for (int k = 0; k < depth; ++k) {
			short val = v.at(i, j, k);
			short post_val = val * disp_val / max_val;

			v.set(i, j, k, post_val);
		}
	}

	float alpha = 0.5, beta = 0.5, c = 120;
	float _2_alpha_2 = 2.0 * alpha * alpha, _2_beta_2 = 2.0 * beta * beta, _2_c_2 = 2.0 * c * c;

	VolumeData<short> res(v1.nx, v1.ny, v1.nz, v1.dx, v1.dy, v1.dz);

	#pragma omp parallel for schedule(guided)
	for (int i = 0; i < width; ++i) for (int j = 0; j < height; ++j) for (int k = 0; k < depth; ++k)
		res.set(i, j, k, 0);

	float min_sigma = 0.1, max_sigma = 5.0;
	int num_sigma = 4;
	float delta_sigma = (max_sigma - min_sigma) / double(num_sigma - 1);

	for (float sigma = min_sigma; sigma <= max_sigma + 1e-3; sigma += delta_sigma) {
		VolumeData<short> e = IIRGaussianBlur(v, sigma);

		#pragma omp parallel for schedule(guided)
		for (int i = 2; i < width - 2; ++i) for (int j = 2; j < height - 2; ++j) for (int k = 2; k < depth - 2; ++k) {
			short val = v.at(i, j, k);
			if (val < 200 || val > 1000 || res.at(i, j, k) != 0)
				continue;

			float Ixx = (e.at(i + 2, j, k) + e.at(i - 2, j, k) - e.at(i, j, k) - e.at(i, j, k)) * 0.25;
			float Iyy = (e.at(i, j + 2, k) + e.at(i, j - 2, k) - e.at(i, j, k) - e.at(i, j, k)) * 0.25;
			float Izz = (e.at(i, j, k + 2) + e.at(i, j, k - 2) - e.at(i, j, k) - e.at(i, j, k)) * 0.25;
			float Ixy = (e.at(i + 1, j + 1, k) + e.at(i - 1, j - 1, k) - e.at(i + 1, j - 1, k) - e.at(i - 1, j + 1, k)) * 0.25;
			float Iyz = (e.at(i, j + 1, k + 1) + e.at(i, j - 1, k - 1) - e.at(i, j + 1, k - 1) - e.at(i, j - 1, k + 1)) * 0.25;
			float Izx = (e.at(i + 1, j, k + 1) + e.at(i - 1, j, k - 1) - e.at(i - 1, j, k + 1) - e.at(i + 1, j, k - 1)) * 0.25;

			float **A = new float*[3];
			float *w = new float[3];
			float **v = new float*[3];
			for (int p = 0; p < 3; ++p) {
				A[p] = new float[3];
				v[p] = new float[3];
			}
			A[0][0] = Ixx; A[1][1] = Iyy; A[2][2] = Izz;
			A[0][1] = Ixy; A[0][2] = Izx; A[1][2] = Iyz;
			A[1][0] = Ixy; A[2][0] = Izx; A[2][1] = Iyz;
			for (int p = 0; p < 3; ++p) for (int q = 0; q < 3; ++q)
				A[p][q] *= (sigma * sigma);
			vtkMath::Jacobi(A, w, v);

			double lambda1 = w[0], lambda2 = w[1], lambda3 = w[2];
			if (abs(lambda1) > abs(lambda2))
				swap(lambda1, lambda2);
			if (abs(lambda1) > abs(lambda3))
				swap(lambda1, lambda3);
			if (abs(lambda3) < abs(lambda2))
				swap(lambda2, lambda3);

			if (lambda2 < 0.0 && lambda3 < 0.0) {
				float RB2 = abs(lambda1 * lambda1 / (lambda2 * lambda3));
				float RA2 = lambda2 * lambda2 / (lambda3 * lambda3);
				float S2 = lambda1 * lambda1 + lambda2 * lambda2 + lambda3 * lambda3;

				float val = (1.0 - exp(-RA2 / _2_alpha_2)) * exp(-RB2 / _2_beta_2) * (1.0 - exp(-S2 / _2_c_2));

				if (val > 0.02)
					res.set(i, j, k, 1e4);
			}

			for (int i = 0; i < 3; ++i) {
				delete[] A[i];
				delete[] v[i];
			}
			delete[] A;
			delete[] w;
			delete[] v;
		}
	}

	removeSmallCluster(res, 2000);

	return res;
}

void VesselExtractor::computeCoefs3(double sigma, double & B, std::array<double, 4>& b) {
	double q, q2, q3;
	if (sigma >= 2.5)
		q = 0.98711 * sigma - 0.96330;
	else if (sigma >= 0.5 && sigma < 2.5)
		q = 3.97156 - 4.14554 * sqrt(1.0 - 0.26891 * sigma);
	else
		q = 0.1147705018520355224609375;
	q2 = q * q;
	q3 = q * q2;
	b[0] = 1.0 / (1.57825 + 2.44413 * q + 1.4281 * q2 + 0.422205 * q3);
	b[1] = (2.44413 * q + 2.85619 * q2 + 1.26661 * q3) * b[0];
	b[2] = (-1.4281 * q2 - 1.26661 * q3) * b[0];
	b[3] = 0.422205 * q3 * b[0];
	B = 1.0 - b[1] - b[2] - b[3];
}

VolumeData<short> VesselExtractor::IIRGaussianBlur(VolumeData<short> &v, double sigma) {
	int width = v.nx, height = v.ny, depth = v.nz;
	double B;
	std::array<double, 4> b;
	computeCoefs3(sigma, B, b);

	VolumeData<short> res(v.nx, v.ny, v.nz, v.dx, v.dy, v.dz);
	for (int i = 0; i < res.nvox; ++i) {
		res.data[i] = v.data[i];
	}

	#pragma omp parallel for schedule(guided)
	for (int i = 0; i < width; ++i) for (int j = 0; j < height; ++j) {
		std::vector<float> w1(depth + 3), w2(depth + 3);
		w1[0] = w1[1] = w1[2] = res.at(i, j, 0);
		for (int p = 3, k = 0; k < depth; ++p, ++k)
			w1[p] = B * res.at(i, j, k) + (b[1] * w1[p - 1] + b[2] * w1[p - 2] + b[3] * w1[p - 3]);
		w2[depth] = w2[depth + 1] = w2[depth + 2] = w1[depth + 2];
		for (int p = depth - 1; p >= 0; --p) {
			w2[p] = B * w1[p + 3] + (b[1] * w2[p + 1] + b[2] * w2[p + 2] + b[3] * w2[p + 3]);
			res.set(i, j, p, short(w2[p]));
		}
	}

	#pragma omp parallel for schedule(guided)
	for (int j = 0; j < height; ++j) for (int k = 0; k < depth; ++k) {
		std::vector<float> w1(width + 3), w2(width + 3);
		w1[0] = w1[1] = w1[2] = res.at(0, j, k);
		for (int p = 3, i = 0; i < width; ++p, ++i)
			w1[p] = B * res.at(i, j, k) + (b[1] * w1[p - 1] + b[2] * w1[p - 2] + b[3] * w1[p - 3]);
		w2[width] = w2[width + 1] = w2[width + 2] = w1[width + 2];
		for (int p = width - 1; p >= 0; --p) {
			w2[p] = B * w1[p + 3] + (b[1] * w2[p + 1] + b[2] * w2[p + 2] + b[3] * w2[p + 3]);
			res.set(p, j, k, short(w2[p]));
		}
	}

	#pragma omp parallel for schedule(guided)
	for (int k = 0; k < depth; ++k) for (int i = 0; i < width; ++i) {
		std::vector<float> w1(height + 3), w2(height + 3);
		w1[0] = w1[1] = w1[2] = res.at(i, 0, k);
		for (int p = 3, j = 0; j < height; ++p, ++j)
			w1[p] = B * res.at(i, j, k) + (b[1] * w1[p - 1] + b[2] * w1[p - 2] + b[3] * w1[p - 3]);
		w2[height] = w2[height + 1] = w2[height + 2] = w1[height + 2];
		for (int p = height - 1; p >= 0; --p) {
			w2[p] = B * w1[p + 3] + (b[1] * w2[p + 1] + b[2] * w2[p + 2] + b[3] * w2[p + 3]);
			res.set(i, p, k, short(w2[p]));
		}
	}

	return res;
}

void VesselExtractor::removeSmallCluster(VolumeData<short> &v, int threshold) {
	using std::deque;
	using std::vector;
	using Eigen::Vector3d;

	int nx = v.nx;
	int ny = v.ny;
	int nz = v.nz;

	VolumeData<short> isHandled(v.nx, v.ny, v.nz, v.dx, v.dy, v.dz);
	#pragma omp parallel for schedule(guided)
	for (int i = 0; i < nx; ++i) for (int j = 0; j < ny; ++j) for (int k = 0; k < nz; ++k)
		isHandled.set(i, j, k, 0);

	for (int i = 0; i < nx; ++i) for (int j = 0; j < ny; ++j) for (int k = 0; k < nz; ++k) {
		if (v.at(i, j, k) == 0 || isHandled.at(i, j, k) != 0)
			continue;

		vector<Vector3d> indices;
		deque<Vector3d> seeds;
		seeds.push_back(Vector3d(i, j, k));

		while (!seeds.empty()) {
			Vector3d idx = seeds.front();
			seeds.pop_front();

			if (indices.size() <= threshold)
				indices.push_back(idx);

			for (int di = -1; di <= 1; ++di) for (int dj = -1; dj <= 1; ++dj) for (int dk = -1; dk <= 1; ++dk) {
				if (di && dj && dk)
					continue;

				int i0 = int(idx(0)) + di, j0 = int(idx(1)) + dj, k0 = int(idx(2)) + dk;
				if (i0 < 0 || i0 >= nx || j0 < 0 || j0 >= ny || k0 < 0 || k0 >= nz)
					continue;

				if (isHandled.at(i0, j0, k0) == 0) {
					if (v.at(i0, j0, k0))
						seeds.push_back(Vector3d(i0, j0, k0));
					isHandled.set(i0, j0, k0, 1);
				}
			}
		}

		if (indices.size() <= threshold) {
			for (auto it = indices.begin(); it != indices.end(); ++it)
				v.set((*it)(0), (*it)(1), (*it)(2), 0);
		}
	}
}
