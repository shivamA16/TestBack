#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <numeric>
using namespace std;

bool check(vector<int>&signals, int i, int compare){
    for (int j = 1; j <= 7; j++){
        if (signals[i-j] == compare){
            return false;
        }
    }

    return true;
}

class MarketData{
public:
    vector<double> prices;

    bool loadFromFile(){
        ifstream file("filepath");
        if (!file){
            cerr<<"Error opening file"<<endl;
            return false;
        }

        double price;
        while (file >> price){
            prices.push_back(price);
        }
        file.close();
        return true;
    }
};


class Strategy{
public:
    virtual void applyStrategy(const vector<double>&prices, vector<int>&signals) = 0;
};

class SMACrossoverStrategy : public Strategy{
private:
    int shortWindow;
    int longWindow;

    double movingAverage(const vector<double> & prices, int start, int end){
        double sum = accumulate(prices.begin() + start, prices.begin() + end, 0.0);
        return sum / (end - start);
    }

public:
    SMACrossoverStrategy(int shortWindow, int longWindow) : shortWindow(shortWindow), longWindow(longWindow) {}

    void applyStrategy(const vector<double>&prices, vector<int>&signals) override {
        for (int i = longWindow; i < prices.size(); ++i){
            double longMA = movingAverage(prices, i - longWindow, i);
            double shortMA = movingAverage(prices, i - shortWindow, i);

            if (shortMA > longMA && check(signals, i, 1)){
                signals[i] = 1;
            } else if (shortMA < longMA && check(signals, i, -1)){
                signals[i] = -1;
            } else{
                signals[i] = 0;
            }
        }
    }
};


class EMACrossoverStrategy : public Strategy{
private:
    int shortWindow;
    int longWindow;
    int N;

    double exponentialMovingAverage(const vector<double> & prices, int start, int end){
        double alpha = 2.0 / (N + 1);
        double ema = prices[start];

        for (int i = start + 1; i < end; ++i) {
            ema = (prices[i] - ema) * alpha + ema;
        }

        return ema;
    }

public:
    EMACrossoverStrategy(int shortWindow, int longWindow, int N) : shortWindow(shortWindow), longWindow(longWindow), N(N){}

    void applyStrategy(const vector<double>&prices, vector<int>&signals) override {
        for (int i = longWindow; i < prices.size(); ++i){
            double longMA = exponentialMovingAverage(prices, i - longWindow, i);
            double shortMA = exponentialMovingAverage(prices, i - shortWindow, i);

            if (shortMA > longMA && check(signals, i, 1)){
                signals[i] = 1;
            } else if (shortMA < longMA && check(signals, i, -1)){
                signals[i] = -1;
            } else{
                signals[i] = 0;
            }
        }
    }
};


class PerformanceMetrics {
public:
    static double sharpe(const vector<double>&returns, double stdDev){
        double avgret = accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
        return avgret/stdDev;
    }

    static double drawdown(const vector<double>&prices){
        double peak = prices[0];
        double maxDrawdown = 0.0;

        for (auto& price: prices){
            if (price>peak){
                peak = price;
            }
            double drawdown = (peak - price) / peak;
            if (drawdown > maxDrawdown){
                maxDrawdown = drawdown;
            }
        }

        return maxDrawdown;
    }

    static double cumulativeReturn(const vector<double>&prices){
        return (prices.back() - prices.front()) / prices.front();
    }

};


class Backtester{
public:
    void runBackTest(Strategy& strategy, MarketData& data){
        vector<int>signals (data.prices.size(), 0);
        strategy.applyStrategy(data.prices, signals);

        ofstream outFile("C:\\Users\\cashi\\CLionProjects\\learn\\trading_signals.csv");
        for (int i = 0; i < signals.size(); i++){
            outFile << data.prices[i] << " , " << signals[i]<<endl;
        }
        outFile.close();
    }
};


int main(){

    MarketData data;
    if (!data.loadFromFile()){
        cout<<"Failed to load data." << endl;
        return 1;
    }

    int shortWindow, longWindow, N;
    cout<<"Enter short window: ";
    cin>>shortWindow;
    cout<<"Enter long window: ";
    cin>>longWindow;
    cout<<"Enter N: ";
    cin>>N;

    EMACrossoverStrategy strat(shortWindow, longWindow, N);
    Backtester tester;
    PerformanceMetrics metric;

    tester.runBackTest(strat, data);
    cout<<metric.drawdown(data.prices);

    return 0;
}
