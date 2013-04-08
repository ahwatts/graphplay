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
