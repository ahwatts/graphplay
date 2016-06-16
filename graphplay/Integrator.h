// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_INTEGRATOR_H_
#define _GRAPHPLAY_GRAPHPLAY_INTEGRATOR_H_

namespace graphplay {
    template <typename Y, typename T>
    class FirstOrderDE {
    public:
        typedef Y dependent_type;
        typedef T independent_type;

        typedef std::unique_ptr<FirstOrderDE<Y, T> > uptr_type;
        typedef std::shared_ptr<FirstOrderDE<Y, T> > sptr_type;
        typedef std::weak_ptr<FirstOrderDE<Y, T> > wptr_type;

        FirstOrderDE() {}
        ~FirstOrderDE() {}

        virtual Y operator()(Y pos, T time) const = 0;
    };

    template <typename Y, typename T>
    class Rk4 {
    public:
        typedef Y dependent_type;
        typedef T independent_type;

        Rk4(typename FirstOrderDE<Y, T>::sptr_type equation)
            : m_equation(equation)
        {}

        ~Rk4() {}

        Y operator()(Y yn, T tn) const {
            T h = tn;
            T half_h = h / 2.0;
            T sixth_h = h / 6.0;
            T two = 2.0;

            Y k1 = (*m_equation)(yn,             tn);
            Y k2 = (*m_equation)(yn + half_h*k1, tn + half_h);
            Y k3 = (*m_equation)(yn + half_h*k2, tn + half_h);
            Y k4 = (*m_equation)(yn + h*k3,      tn + h);

            return (k1 + k2*two + k3*two + k4)*sixth_h;
        }

    protected:
        typename FirstOrderDE<Y, T>::sptr_type m_equation;
    };
}

#endif
