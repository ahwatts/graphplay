#include "Body.h"

Body::Body()
    : mw_pos(),
      mw_vel_dir(1, 0, 0),
      m_vel_mag(0),
      m_ang_pos(0),
      m_ang_vel(0),
      mw_ang_vel_dir(0, 1, 0)
{ }

Body::~Body(void)
{ }

void Body::update(float dt)
{
    mw_pos = mw_pos + (m_vel_mag*mw_vel_dir*dt);
    m_ang_pos = m_ang_pos + m_ang_vel*dt;
    if (m_ang_pos > 360.0f) {
        m_ang_pos = m_ang_pos - 360.0f;
    }
}
