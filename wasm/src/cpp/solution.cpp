#include "../include/solution.h"

const float eps = 1e-3;


int benchmark() {
  int counts[] = {100, 200, 500, 1000, 2000, 5000, 10000};

  for (int count : counts) {
    std::cout << "run with " << count << std::endl;
    Timer timer;

  }

  return 0;
}

int debt(Eigen::MatrixXf preResults, int rounds, int counts) {
  // pre_result's shape: (counts, 100, rounds), 100是p的细分份数
  auto blue = Eigen::MatrixXf::Constant(100, counts, 100); // (100, counts)
  auto black = Eigen::MatrixXf::Constant(100, counts, 300);

  for(int i = 0; i < rounds; ++i) {
    auto r = preResults.transpose().row(i);
    auto someone_loss_or_draw = (blue.array() <= 0).cast<float>() + (black.array() <= 0).cast<float>() + (Eigen::abs(blue.array() - 200.f) < eps).cast<float>();
    #ifdef MY_DEBUG
    if (i == 0) {
      std::cout << "r: " << r.rows() << ", " << r.cols() << std::endl;
      std::cout << "someone_loss_or_draw: " << someone_loss_or_draw.rows() << ", " << someone_loss_or_draw.cols() << std::endl;
    }
    #endif
  }

  return 0;
}

void pregenerate_result(const Eigen::VectorXf& ps, const int counts, Eigen::MatrixXf& results) {
  // ps: (100,)
  // results: (100, counts)
  results = Eigen::Map<Eigen::Matrix<float, -1, -1, Eigen::RowMajor>>((float*)ps.data(), 100, 1);
  results = results.replicate(1, counts); // (100, counts)
  results = (results.array() > Eigen::ArrayXXf::Random(100, counts).array() * 0.5 + 0.5).cast<float>() * 2 - 1; // (100, counts)
}


void run(const int total_count, const int counts, const int rounds) {
  std::cout << "run with total count: " << total_count << ", counts: " << counts << ", rounds: " << rounds << std::endl;

  Eigen::VectorXf ps = Eigen::VectorXf::LinSpaced(100, 0.0f, 1.0f);

  #ifdef MY_DEBUG
  for (int i = 0; i < 100; ++i) {
    std::cout << ps[i] << ", ";
  }
  std::cout << "\n";
  #endif

  const int N = total_count / counts;
  for(int i = 0; i < N; ++i) {
    // Eigen::MatrixXf preResults = Eigen::MatrixXf::Random(counts, 100);

    Eigen::MatrixXf* pre_results = new Eigen::MatrixXf[counts]; // (rounds, 100, counts)
    for(int i = 0; i < rounds; ++i) {
      Eigen::MatrixXf results;  // (100, counts)
      pregenerate_result(ps, counts, results);
      pre_results[i] = results;

      #ifdef MY_DEBUG
      if (i == 0) {
        std::cout << "results: " << results.rows() << ", " << results.cols() << std::endl;
        std::cout << results << std::endl;
      }
      #endif
    }

    debt()

  }




}


int main() {
  run(1000, 100, 100);
}