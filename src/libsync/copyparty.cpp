/*
 * copyparty compatibility helpers - implementation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "copyparty.h"

#include <QByteArray>

namespace OCC::Copyparty {

bool isEnabled()
{
    // This build is the copyparty client - copyparty behaviour is always on.
    return true;
}

QString user()
{
    return QString::fromUtf8(qgetenv("COPPYPARTY_USER"));
}

QString password()
{
    return QString::fromUtf8(qgetenv("COPPYPARTY_PASS"));
}

} // namespace OCC::Copyparty
