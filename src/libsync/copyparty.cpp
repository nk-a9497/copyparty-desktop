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
    return qEnvironmentVariableIsSet("OPENCLOUD_COPPYPARTY");
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
