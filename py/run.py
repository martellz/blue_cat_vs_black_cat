import numpy as np
import matplotlib.pyplot as plt
import time
from tqdm import tqdm

eps = 1e-3

def pregenerate_result(ps: np.ndarray, count: int):
  result = ps[..., np.newaxis].repeat(count, axis=1)
  result = np.where(np.random.rand(*result.shape) <= result , 1, -1)
  return result

def debt(pre_result: np.ndarray):
  # pre_result.shape = (100, rounds)
  black = np.ones(pre_result.shape[0]) * 300
  blue = np.ones(pre_result.shape[0]) * 100

  for r in pre_result.T:
    # r.shape = black.shape = blue.shape = (100, )
    # 假设黑猫资金为正时可以借qwq
    someone_loss_or_draw = (blue <= 0).astype(np.float32) + (black <= 0).astype(np.float32) + (blue == 200).astype(np.float32)
    debt_count = np.ceil(np.where(someone_loss_or_draw.astype(np.bool_), np.zeros_like(blue), blue) / 2)
    blue = blue + r * debt_count
    black = black - r * debt_count

  return blue

def debt_3d(pre_result: np.ndarray):
  # pre_result.shape = (counts, 100, rounds)
  black = np.ones(pre_result.T.shape[1:]) * 300
  blue = np.ones(pre_result.T.shape[1:]) * 100

  for r in pre_result.T:
    # r.shape = black.shape = blue.shape = (100, counts)
    # 还是假设黑猫资金为正时可以借qwq
    someone_loss_or_draw = (blue <= 0).astype(np.float32) + (black <= 0).astype(np.float32) + (np.abs(blue - 200) < eps).astype(np.float32)
    debt_count = np.ceil(np.where(someone_loss_or_draw.astype(np.bool_), np.zeros_like(blue), blue) / 2)
    blue = blue + r * debt_count
    black = black - r * debt_count

  return blue

def run():
  st = time.time()

  p = np.linspace(0, 1, 100)  # (100, )
  rounds = 100
  counts = 1_000
  results = []
  for _ in range(counts):
    pre_result = pregenerate_result(p, rounds)

    blue = debt(pre_result)
    blue = (blue - 200).astype(np.int32)
    blue = np.where(np.abs(blue) < eps, np.zeros_like(blue), blue / np.abs(blue + eps))
    results.append(blue)
  results = np.array(results) # (counts, 100)
  blue_win = np.count_nonzero(results > 0, axis=0)
  black_win = np.count_nonzero(results < 0, axis=0)
  draw = np.count_nonzero(results == 0, axis=0)

  et = time.time()
  print('cost time for {} counts: {}'.format(counts, et - st))
  plt.plot(p, blue_win / counts, label='blue win')
  plt.plot(p, black_win / counts, label='black win')
  plt.legend()
  plt.show()

  plt.plot(p, draw / counts, 'o', label='draw')
  plt.show()

def run3d():
  st = time.time()

  # 尽管我想一次性生成所有的结果，但是内存不够qwq
  # 具体每次counts为多少，请跑一下benchmark
  results = []
  total_counts = 1_000_000
  p = np.linspace(0, 1, 100)  # (100, )
  rounds = 100

  # 这一步for循环还可以加速，比如 map-reduce
  counts = 100
  N = total_counts // counts
  for i in tqdm(range(N)):
    pre_results = []
    for j in range(counts):
      pre_results.append(pregenerate_result(p, rounds))
    pre_results = np.array(pre_results) # (counts, 100, rounds)

    blue = debt_3d(pre_results)  # (100, counts)
    results.append(blue.T)
  results = np.array(results) # (N, counts, 100)
  results = results.reshape(-1, 100)  # (total_counts, 100)

  match_result = (results - 200).astype(np.int32)
  match_result = np.where(np.abs(match_result) < eps, np.zeros_like(match_result), match_result / np.abs(match_result + eps))  # (total_counts, 100)

  blue_win = np.count_nonzero(match_result > 0, axis=0)
  black_win = np.count_nonzero(match_result < 0, axis=0)
  draw = np.count_nonzero(match_result == 0, axis=0)

  et = time.time()
  print('cost time for {} counts: {}'.format(total_counts, et - st))

  plt.plot(p, blue_win / total_counts, label='blue win')
  plt.plot(p, black_win / total_counts, label='black win')
  plt.legend()
  plt.show()

  plt.plot(p, draw / total_counts, 'o', label='draw')
  plt.show()

def benchmark():
  counts = [100, 200, 500, 1_000, 2_000, 5_000, 10_000, 20_000, 50_000, 100_000]
  for count in counts:
    st = time.time()

    p = np.linspace(0, 1, 100)  # (100, )
    rounds = 100
    results = []
    for _ in range(count):
      pre_result = pregenerate_result(p, rounds)

      blue = debt(pre_result)
      blue = (blue - 200).astype(np.int32)
      blue = np.where(np.abs(blue) < eps, np.zeros_like(blue), blue / np.abs(blue + eps))
      results.append(blue)
    results = np.array(results) # (counts, 100)
    blue_win = np.count_nonzero(results > 0, axis=0)
    black_win = np.count_nonzero(results < 0, axis=0)
    draw = np.count_nonzero(results == 0, axis=0)

    et = time.time()
    print('cost time for {} counts: {}'.format(count, et - st))

if __name__ == '__main__':
  run3d()
  # benchmark()