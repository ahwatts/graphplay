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

        virtual Y operator()(Y pos, T base_time, T step_time) const = 0;
    };

    template <typename Y, typename T>
    class Integrator {
    public:
        typedef std::unique_ptr<Integrator<Y, T> > uptr_type;
        typedef std::shared_ptr<Integrator<Y, T> > sptr_type;
        typedef std::weak_ptr<Integrator<Y, T> > wptr_type;

        typedef FirstOrderODE<Y, T> eqn_type;
        typedef typename FirstOrderODE<Y, T>::sptr_type eqn_ptr;

        Integrator(eqn_ptr equation) : m_equation(equation) {}
        virtual ~Integrator() {}

        virtual Y step(T h) = 0;

        inline const eqn_ptr equation() const { return m_equation; }

        inline const Y& dependent() const { return m_dep; }
        inline void dependent(const Y &new_dep) { m_dep = new_dep; }

        inline T independent() const { return m_indep; }
        inline void independent(T new_indep) { m_indep = new_indep; }

    protected:
        eqn_ptr m_equation;
        Y m_dep;
        T m_indep;
    };

    template <typename Y, typename T>
    class Euler : public Integrator<Y, T> {
    public:
        Euler(typename Integrator<Y, T>::eqn_ptr equation)
            : Integrator<Y, T>(equation) {}
        virtual ~Euler() {}

        virtual Y step(T h) {
            Y y = this->dependent();
            T t = this->independent();
            Y ydot = (*this->equation())(y, t, 0.0);

            y += ydot * h;
            t += h;

            this->dependent(y);
            this->independent(t);
            return this->dependent();
        }
    };

    template <typename Y, typename T>
    class Rk4 : public Integrator<Y, T> {
    public:
        Rk4(typename FirstOrderODE<Y, T>::sptr_type equation)
            : Integrator<Y, T>(equation) {}
        virtual ~Rk4() {}

        virtual Y step(T h) {
            Y y = this->dependent();
            T t = this->independent();
            T half_h = h / 2.0;
            T sixth_h = h / 6.0;
            T two = 2.0;

            Y k1 = (*this->equation())(y,             t, 0.0);
            Y k2 = (*this->equation())(y + k1*half_h, t, half_h);
            Y k3 = (*this->equation())(y + k2*half_h, t, half_h);
            Y k4 = (*this->equation())(y + k3*h,      t, h);

            this->dependent(y + (k1 + k2*two + k3*two + k4)*sixth_h);
            this->independent(t + h);
            return this->dependent();
        }

    protected:
        typename FirstOrderODE<Y, T>::sptr_type m_equation;
    };
}

#endif
