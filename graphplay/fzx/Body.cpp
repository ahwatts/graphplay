// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "../graphplay.h"
#include "Body.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

namespace graphplay {
    namespace fzx {
        // Class Phase.
        Phase::Phase()
            : position(),
              momentum()
        {}

        Phase::Phase(const glm::vec3 &pos, const glm::vec3 &mom)
            : position(pos),
              momentum(mom)
        {}

        Phase operator+(const Phase &p1, const Phase &p2) {
            Phase rv(p1);
            return rv += p2;
        }

        Phase operator*(const Phase &p, float t) {
            Phase rv(p);
            return rv *= t;
        }

        Phase& operator+=(Phase &p, const Phase &dp) {
            p.position += dp.position;
            p.momentum += dp.momentum;
            return p;
        }

        Phase& operator*=(Phase &p, float dt) {
            p.position *= dt;
            p.momentum *= dt;
            return p;
        }

        std::ostream& operator<<(std::ostream &stream, const Phase &body) {
            return stream << "Phase(position = " << body.position
                          << ", momentum = " << body.momentum << ")";
        }

        // Class BodyStateEquation.
        BodyStateEquation::BodyStateEquation(const Body &body)
            : m_body(body)
        {}

        Phase BodyStateEquation::operator()(const Phase &y) const {
            glm::vec3 force = m_body.netForce();
            for (auto&& c : m_body.m_constraints) {
                force += c.force(y.position, c.attachedTo().position());
            }
            return Phase(y.momentum / m_body.mass(), force);
        }

        const int FUTURE_INDEX = 0;
        const int CURRENT_INDEX = 1;
        const int KEPT_STATES = 2;

        // Class Body.
        Body::Body()
            : m_mass(1.0),
              m_states(KEPT_STATES),
              m_forces(KEPT_STATES),
              m_equation(),
              m_integrator(rk4<Phase, float>),
              m_constraints(),
              m_bbox({ -1.0, -1.0, -1.0 }, { 1.0, 1.0, 1.0 }),
              m_aa_bbox({ -1.0, -1.0, -1.0 }, { 1.0, 1.0, 1.0 })
        {
            m_equation.reset(new BodyStateEquation(*this));
        }

        Body::Body(float _mass, const glm::vec3 &pos, const glm::vec3 &vel, const BBox &bbox)
            : Body()
        {
            mass(_mass);
            position(pos);
            velocity(vel);
            boundingBox(bbox);
            m_aa_bbox = bbox.axisAlignedAfterTransform(glm::mat4x4(1));
        }

        float Body::mass() const {
            return m_mass;
        }

        void Body::mass(float new_mass) {
            if (new_mass > 0) {
                m_mass = new_mass;
            }
        }

        glm::vec3 Body::position() const {
            return m_states[FUTURE_INDEX].position;
        }

        glm::vec3 Body::position(float alpha) const {
            const glm::vec3 &curr = m_states[CURRENT_INDEX].position;
            const glm::vec3 &next = m_states[FUTURE_INDEX].position;
            return next*alpha + curr*(1 - alpha);
        }

        void Body::position(const glm::vec3 &new_pos) {
            m_states[CURRENT_INDEX].position = new_pos;
            m_states[FUTURE_INDEX].position = new_pos;
        }

        glm::vec3 Body::velocity() const {
            return m_states[FUTURE_INDEX].momentum / m_mass;
        }

        glm::vec3 Body::velocity(float alpha) const {
            const glm::vec3 &curr = m_states[CURRENT_INDEX].momentum;
            const glm::vec3 &next = m_states[FUTURE_INDEX].momentum;
            return (next*alpha + curr*(1 - alpha)) / m_mass;
        }

        void Body::velocity(const glm::vec3 &new_vel) {
            glm::vec3 new_momentum = new_vel * m_mass;
            m_states[CURRENT_INDEX].momentum = new_momentum;
            m_states[FUTURE_INDEX].momentum = new_momentum;
        }

        glm::vec3 Body::netForce() const {
            return m_forces[FUTURE_INDEX];
        }

        // Not sure how useful this is...
        glm::vec3 Body::netForce(float alpha) const {
            const glm::vec3 &curr = m_forces[CURRENT_INDEX];
            const glm::vec3 &next = m_forces[FUTURE_INDEX];
            return next*alpha + curr*(1 - alpha);
        }

        void Body::addForce(const glm::vec3 &force) {
            m_forces[FUTURE_INDEX] += force;
        }

        void Body::addConstraint(AttachedSpring constraint) {
            m_constraints.emplace_back(constraint);
        }

        BBox Body::boundingBox() const {
            BBox rv(m_bbox);
            glm::vec3 pos = position();
            rv.min += pos;
            rv.max += pos;
            return rv;
        }

        void Body::boundingBox(const BBox &bbox) {
            m_bbox = bbox;
        }

        void Body::update(float dt) {
            Phase next = (*m_integrator)(m_states[FUTURE_INDEX], dt, *m_equation);

            m_states.push_front(next);
            m_states.pop_back();

            m_forces.push_front({ 0.0, 0.0, 0.0 });
            m_forces.pop_back();
        }

        glm::mat4x4 Body::modelTransformation(float alpha, const glm::mat4x4 &base_modelview) const {
            return glm::translate(base_modelview, position(alpha));
        }

        std::ostream& operator<<(std::ostream &stream, const Body &body) {
            stream << "Body:" << std::endl;
            stream << "  m_mass = " << body.m_mass << std::endl;

            int i = 0;
            stream << "  m_states = " << std::endl;
            for (auto&& state : body.m_states) {
                stream << "    " << i << ": " << state << std::endl;
                i += 1;
            }

            i = 0;
            stream << "  m_forces = " << std::endl;
            for (auto&& force : body.m_forces) {
                stream << "    " << i << ": " << force << std::endl;
                i += 1;
            }

            return stream;
        }
    }
}
