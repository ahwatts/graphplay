// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_INTEGRATOR_H_
#define _GRAPHPLAY_GRAPHPLAY_INTEGRATOR_H_

namespace graphplay {
    template <typename Y, typename T>
    class FirstOrderODE {
    public:
        typedef std::unique_ptr<FirstOrderODE<Y, T> > uptr_type;
        typedef std::shared_ptr<FirstOrderODE<Y, T> > sptr_type;
        typedef std::weak_ptr<FirstOrderODE<Y, T> > wptr_type;

        FirstOrderODE() {}
        virtual ~FirstOrderODE() {}

        virtual Y operator()(const Y &y) const = 0;
    };

    template <typename Y, typename T>
    using Integrator = Y (*)(const Y &y0, const T &h, const FirstOrderODE<Y, T> &eqn);

    template <typename Y, typename T>
    Y euler(const Y &y0, const T &h, const FirstOrderODE<Y, T> &eqn) {
        return eqn(y0) * h;
    }

    template <typename Y, typename T>
    Y rk4(const Y &y0, const T &h, const FirstOrderODE<Y, T> &eqn) {
        T half_h = h / static_cast<T>(2.0);
        T sixth_h = h / static_cast<T>(6.0);
        T two = 2.0;

        Y k1 = eqn(y0);             // , t0
        Y k2 = eqn(y0 + k1*half_h); // , t0 + half_h
        Y k3 = eqn(y0 + k2*half_h); // , t0 + half_h
        Y k4 = eqn(y0 + k3*h);      // , t0 + h

        return y0 + (k1 + k2*two + k3*two + k4)*sixth_h;
    }
}

#endif
