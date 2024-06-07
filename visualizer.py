import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

signals_df = pd.read_csv('../../CLionProjects/learn/trading_signals.csv', header=None, names=['Price', 'Signal'])

signals_df['Return'] = signals_df['Price'].pct_change()

signals_df['Cumulative Return'] = (1 + signals_df['Return']).cumprod() - 1

sharpe_ratio = signals_df['Return'].mean() / signals_df['Return'].std()
max_drawdown = (signals_df['Cumulative Return'] - signals_df['Cumulative Return'].cummax()).max()
cumulative_return = signals_df['Cumulative Return'].iloc[-1]

plt.figure(figsize=(14, 7))
plt.plot(signals_df['Price'], label='Price')
plt.scatter(signals_df.index[signals_df['Signal'] == 1], signals_df['Price'][signals_df['Signal'] == 1], color='green', marker='^', label='Buy Signal')
plt.scatter(signals_df.index[signals_df['Signal'] == -1], signals_df['Price'][signals_df['Signal'] == -1], color='red', marker='v', label='Sell Signal')
plt.legend()
plt.title('Price Data with Trading Signals')
plt.xlabel('Date')
plt.ylabel('Price')
plt.grid(True)
plt.show()

print(f"Sharpe Ratio: {sharpe_ratio:.2f}")
print(f"Maximum Drawdown: {max_drawdown:.2%}")
print(f"Cumulative Return: {cumulative_return:.2%}")
