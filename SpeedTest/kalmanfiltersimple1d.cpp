#include "kalmanfiltersimple1d.hpp"

double
KalmanFilterSimple1D::getX0() const
{
    return X0;
}

void
KalmanFilterSimple1D::setX0(double value)
{
    X0 = value;
}

double
KalmanFilterSimple1D::getP0() const
{
    return P0;
}

void
KalmanFilterSimple1D::setP0(double value)
{
    P0 = value;
}

double
KalmanFilterSimple1D::getF() const
{
    return F;
}

void
KalmanFilterSimple1D::setF(double value)
{
    F = value;
}

double
KalmanFilterSimple1D::getQ() const
{
    return Q;
}

void
KalmanFilterSimple1D::setQ(double value)
{
    Q = value;
}

double
KalmanFilterSimple1D::getH() const
{
    return H;
}

void
KalmanFilterSimple1D::setH(double value)
{
    H = value;
}

double
KalmanFilterSimple1D::getR() const
{
    return R;
}

void
KalmanFilterSimple1D::setR(double value)
{
    R = value;
}

double
KalmanFilterSimple1D::getState() const
{
    return State;
}

void
KalmanFilterSimple1D::setState(double value)
{
    State = value;
}

double
KalmanFilterSimple1D::getCovariance() const
{
    return Covariance;
}

void
KalmanFilterSimple1D::setCovariance(double value)
{
    Covariance = value;
}

KalmanFilterSimple1D::KalmanFilterSimple1D(double q, double r, double f,
                                           double h)
{
    Q = q;
    R = r;
    F = f;
    H = h;
}

void
KalmanFilterSimple1D::SetState(double state, double covariance)
{
    State      = state;
    Covariance = covariance;
}

void
KalmanFilterSimple1D::Correct(double data)
{
    // time update - prediction
    X0 = F * State;
    P0 = F * Covariance * F + Q;

    // measurement update - correction
    double K   = H * P0 / (H * P0 * H + R);
    State      = X0 + K * (data - H * X0);
    Covariance = (1 - K * H) * P0;
}
