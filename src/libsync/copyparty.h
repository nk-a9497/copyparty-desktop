/*
 * copyparty compatibility helpers.
 *
 * The OpenCloud desktop client can be pointed at a plain copyparty WebDAV
 * server (HTTP Basic auth, no OpenCloud API). To avoid disturbing the default
 * OpenCloud behaviour, all copyparty-specific code paths are gated behind the
 * OPENCLOUD_COPPYPARTY environment variable. Credentials may be supplied via
 * COPPYPARTY_USER / COPPYPARTY_PASS (or entered in the setup wizard).
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "opencloudsynclib.h"

#include <QString>

namespace OCC::Copyparty {

/** Whether copyparty mode is enabled (OPENCLOUD_COPPYPARTY set). */
OPENCLOUD_SYNC_EXPORT bool isEnabled();

/** Username from the COPPYPARTY_USER environment variable (may be empty). */
OPENCLOUD_SYNC_EXPORT QString user();

/** Password from the COPPYPARTY_PASS environment variable (may be empty). */
OPENCLOUD_SYNC_EXPORT QString password();

} // namespace OCC::Copyparty
