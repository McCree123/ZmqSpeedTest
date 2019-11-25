#ifndef KALMANFILTERSIMPLE1D_HPP
#define KALMANFILTERSIMPLE1D_HPP

class KalmanFilterSimple1D
{
private:
    double X0{0}; // predicted state
    double P0{0}; // predicted covariance

    double F{0}; // factor of real value to previous real value
    double Q{0}; // measurement noise
    double H{0}; // factor of measured value to real value
    double R{0}; // environment noise

    double State{0};
    double Covariance{0};

public:
    KalmanFilterSimple1D(double q, double r, double f = 1, double h = 1);
    void SetState(double state, double covariance);
    void Correct(double data);
    double getX0() const;
    void setX0(double value);
    double getP0() const;
    void setP0(double value);
    double getF() const;
    void setF(double value);
    double getQ() const;
    void setQ(double value);
    double getH() const;
    void setH(double value);
    double getR() const;
    void setR(double value);
    double getState() const;
    void setState(double value);
    double getCovariance() const;
    void setCovariance(double value);
};

#endif // KALMANFILTERSIMPLE1D_HPP
