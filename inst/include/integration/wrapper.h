// Copyright (C) 2016 Yixuan Qiu <yixuan.qiu@cos.name>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef INTEGRATION_WRAPPER_H
#define INTEGRATION_WRAPPER_H

#include "GaussKronrodNodesWeights.h"
#include "Integrator.h"
#include "../Func.h"

namespace Numer
{


inline double integrate(
    const Func& f, const double& lower, const double& upper,
    double& err_est, int& err_code,
    const int subdiv = 100, const double& eps_abs = 1e-8, const double& eps_rel = 1e-6,
    const Integrator<double>::QuadratureRule rule = Integrator<double>::GaussKronrod41
)
{
    // If lower > upper, change the limits
    double ilower = lower, iupper = upper;
    bool change_sign = false;
    if(lower > upper)
    {
        ilower = upper;
        iupper = lower;
        change_sign = true;
    }

    Integrator<double> intgr(subdiv);
    double res = intgr.quadratureAdaptive(f, ilower, iupper, eps_abs, eps_rel, rule);
    err_est = intgr.estimatedError();
    err_code = intgr.errorCode();
    return change_sign ? -res : res;
}


// Integrate R function
class RFunc: public Func
{
private:
    Rcpp::Function fun;
    Rcpp::RObject  args;
public:
    RFunc(Rcpp::Function fun_, Rcpp::RObject args_) :
        fun(fun_),
        args(args_)
    {}

    double operator()(const double& x) const
    {
        Rcpp::NumericVector xv = Rcpp::NumericVector::create(x);
        Rcpp::NumericVector res = fun(xv, args);
        if(res.length() != 1)
            Rcpp::stop("integrand must return a vector of the same length of x");

        return Rcpp::as<double>(res);
    }

    void   operator()(std::vector<double>& x) const
    {
        const int len = x.size();
        Rcpp::NumericVector xv(len);
        std::copy(x.begin(), x.end(), xv.begin());
        Rcpp::NumericVector res = fun(xv, args);
        if(res.length() != len)
            Rcpp::stop("integrand must return a vector of the same length of x");

        std::copy(res.begin(), res.end(), x.begin());
    }
};

inline double integrate(
    Rcpp::Function f, Rcpp::RObject args, const double& lower, const double& upper,
    double& err_est, int& err_code,
    const int subdiv = 100, const double& eps_abs = 1e-8, const double& eps_rel = 1e-6,
    const Integrator<double>::QuadratureRule rule = Integrator<double>::GaussKronrod41
)
{
    // If lower > upper, change the limits
    double ilower = lower, iupper = upper;
    bool change_sign = false;
    if(lower > upper)
    {
        ilower = upper;
        iupper = lower;
        change_sign = true;
    }

    Integrator<double> intgr(subdiv);
    RFunc rfun(f, args);
    double res = intgr.quadratureAdaptive(rfun, ilower, iupper, eps_abs, eps_rel, rule);
    err_est = intgr.estimatedError();
    err_code = intgr.errorCode();
    return change_sign ? -res : res;
}


}


#endif // INTEGRATION_WRAPPER_H
