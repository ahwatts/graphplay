// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _GRAPHPLAY_GRAPHPLAY_FZX_BODY_H_
#define _GRAPHPLAY_GRAPHPLAY_FZX_BODY_H_

#include "../graphplay.h"

#include <deque>
#include <iostream>
#include <memory>
#include <vector>

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "BBox.h"
#include "Constraint.h"
#include "Integrator.h"

namespace graphplay {
    namespace fzx {
        class Phase {
        public:
            Phase();
            Phase(const glm::vec3 &pos, const glm::vec3 &momentum);
            glm::vec3 position, momentum;
        };

        Phase operator+(const Phase &p1, const Phase &p2);
        Phase operator*(const Phase &p, float t);
        Phase& operator+=(Phase &p, const Phase &dp);
        Phase& operator*=(Phase &p, float dt);
        std::ostream& operator<<(std::ostream &stream, const Phase &body);

        class Body
        {
        public:
            typedef std::unique_ptr<Body> uptr_type;
            typedef std::shared_ptr<Body> sptr_type;
            typedef std::weak_ptr<Body> wptr_type;

            Body();
            Body(float mass, const glm::vec3 &pos, const glm::vec3 &vel, const BBox &bbox);

            float mass() const;
            void mass(float new_mass);

            glm::vec3 position() const;
            glm::vec3 position(float alpha) const;
            void position(const glm::vec3& new_pos);

            glm::vec3 velocity() const;
            glm::vec3 velocity(float alpha) const;
            void velocity(const glm::vec3 &new_vel);

            glm::vec3 netForce() const;
            glm::vec3 netForce(float alpha) const;
            void addForce(const glm::vec3 &force);

            glm::vec3 constraintForce(const glm::vec3 &self_pos, const glm::vec3 &other_pos) const;
            void addConstraint(AttachedSpring constraint);

            BBox boundingBox() const;
            void boundingBox(const BBox &bbox);

            void update(float dt);

            glm::mat4x4 modelTransformation(float alpha, const glm::mat4x4 &base_transform) const;

            friend class BodyStateEquation;
            friend std::ostream& operator<<(std::ostream &stream, const Body &body);

        protected:
            float m_mass;
            std::deque<Phase> m_states;
            std::deque<glm::vec3> m_forces;
            typename FirstOrderODE<Phase, float>::uptr_type m_equation;
            Integrator<Phase, float> m_integrator;
            std::vector<AttachedSpring> m_constraints;
            BBox m_bbox, m_aa_bbox;
        };

        std::ostream& operator<<(std::ostream &stream, const Body &body);

        class BodyStateEquation : public FirstOrderODE<Phase, float> {
        public:
            BodyStateEquation(const Body &body);
            virtual Phase operator()(const Phase &pos) const;

        protected:
            const Body &m_body;
        };
    }
}

#endif
