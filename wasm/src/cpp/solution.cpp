#include "../include/solution.h"

const float eps = 1e-3;

void run(const int total_count, const int counts, const int rounds);
int benchmark() {
  int counts[] = {100, 200, 500, 1000, 2000, 5000, 10000};

  for (int count : counts) {
    std::cout << "run with " << count << std::endl;
    Timer timer;
    run(10000, count, 100);
  }

  return 0;
}

// TODO: maybe I don't need to use new to create pre_results(as maybe it's not a big size matrix)
int debt(Eigen::MatrixXf* pre_results, int rounds, int counts, Eigen::MatrixXf& result) {
  // pre_result's shape: (rounds, 100, counts), 100是p的细分份数
  // result's shape: (100, counts)
  Eigen::MatrixXf blue = Eigen::MatrixXf::Constant(100, counts, 100); // (100, counts)
  Eigen::MatrixXf black = Eigen::MatrixXf::Constant(100, counts, 300);

  for(int i = 0; i < rounds; ++i) {
    Eigen::MatrixXf r = pre_results[i];  // (100, counts)
    Eigen::MatrixXf someone_loss_or_draw = (blue.array() <= 0).cast<float>().matrix() + (black.array() <= 0).cast<float>().matrix() + (Eigen::abs(blue.array() - 200.f) < eps).cast<float>().matrix();
    Eigen::MatrixXf debt_count = (someone_loss_or_draw.array() < eps).cast<float>().matrix().cwiseProduct(blue / 2.0f);
    Eigen::MatrixXf ceil_debt_count = debt_count.array().ceil().matrix();

    blue = blue + r.cwiseProduct(ceil_debt_count);
    black = black - r.cwiseProduct(ceil_debt_count);
  }

  result.block(0, 0, 100, counts) = blue;
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
  std::vector<int> blue_win_counts(100);
  std::vector<int> black_win_counts(100);
  std::vector<int> draw_counts(100);
  std::vector<float> ps_vector;

  {
    // timer scope
    Timer timer;
    std::cout << "run with total count: " << total_count << ", counts: " << counts << ", rounds: " << rounds << std::endl;

    Eigen::VectorXf ps = Eigen::VectorXf::LinSpaced(100, 0.0f, 1.0f);

    #ifdef MY_DEBUG
    for (int i = 0; i < 100; ++i) {
      std::cout << ps[i] << ", ";
    }
    std::cout << "\n";
    #endif

    const int N = total_count / counts;
    Eigen::MatrixXf results = Eigen::MatrixXf::Zero(100, total_count);
    for(int i = 0; i < N; ++i) {
      Eigen::MatrixXf* pre_results = new Eigen::MatrixXf[counts]; // (rounds, 100, counts)
      for(int i = 0; i < rounds; ++i) {
        Eigen::MatrixXf results;  // (100, counts)
        pregenerate_result(ps, counts, results);
        pre_results[i] = results;

        #ifdef MY_DEBUG
        if (i == 0) {
          // std::cout << results << std::endl;
        }
        #endif
      }

      Eigen::MatrixXf result = Eigen::MatrixXf::Zero(100, counts); // (100, counts)
      debt(pre_results, rounds, counts, result);
      // release pre_results
      delete[] pre_results;

      #ifdef MY_DEBUG
      std::cout << "results: " << results.rows() << ", " << results.cols() << std::endl;
      std::cout << result << std::endl;
      #endif
      results.block(0, i * counts, 100, counts) = result;
    }

    auto blue_results = results.array() - 200.f;
    auto match_results = (Eigen::abs(blue_results) > eps).cast<float>() * (blue_results / Eigen::abs(blue_results + eps).cast<float>());
    auto blue_win = match_results > eps;
    auto black_win = match_results < -eps;
    auto draw = Eigen::abs(match_results) < eps;

    ps_vector = std::vector<float>(ps.data(), ps.data() + ps.size());

    std::cout << "match result:\n";
    for(int i = 0; i < 100; ++i) {
      blue_win_counts.at(i) = blue_win.row(i).count();
      black_win_counts.at(i) = black_win.row(i).count();
      draw_counts.at(i) = draw.row(i).count();
      std::cout << "p: " << ps[i] << ", win counts: " << blue_win_counts[i] << ", " << black_win_counts[i] << ", " << draw_counts[i] << std::endl;
    }
  } // end of timer scope

  plt::plot(ps_vector, blue_win_counts, "b");
  plt::plot(ps_vector, black_win_counts, "r");
  plt::show();
}


int main() {
  run(1e5, 1000, 100);
}