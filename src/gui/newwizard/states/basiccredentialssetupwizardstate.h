/*
 * Copyright (C) Fabian Müller <fmueller@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "abstractsetupwizardstate.h"

namespace OCC::Wizard {

/**
 * Collects HTTP Basic credentials. Used for plain WebDAV servers (e.g.
 * copyparty) where no OpenCloud / OAuth login flow exists.
 */
class BasicCredentialsSetupWizardState : public AbstractSetupWizardState
{
    Q_OBJECT

public:
    explicit BasicCredentialsSetupWizardState(SetupWizardContext *context);

    SetupWizardState state() const override;

    void evaluatePage() override;
};

} // namespace OCC::Wizard
