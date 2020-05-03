from pandas import read_csv
import sys
import matplotlib
import matplotlib.pyplot as plt
series = read_csv(sys.argv[1], header=0, index_col=0, parse_dates=True, squeeze=True)
series = series.astype(float)
series.plot()
plt.savefig("grafico.png")