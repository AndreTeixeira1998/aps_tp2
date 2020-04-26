from pandas import read_csv
import matplotlib
import matplotlib.pyplot as plt
series = read_csv('data_out.txt', header=0, index_col=0, parse_dates=True, squeeze=True)
series = series.astype(float)
series.plot()
plt.savefig("grafico.png")