/*
 * Copyright (C) Fabian Müller <fmueller@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "gui/newwizard/states/basiccredentialssetupwizardstate.h"

#include "gui/newwizard/pages/basiccredentialssetupwizardpage.h"
#include "gui/newwizard/setupwizardwidget.h"
#include "libsync/copyparty.h"

namespace OCC::Wizard {

BasicCredentialsSetupWizardState::BasicCredentialsSetupWizardState(SetupWizardContext *context)
    : AbstractSetupWizardState(context)
{
    // pre-fill the username from the environment, if configured
    auto *page = new BasicCredentialsSetupWizardPage(Copyparty::user());
    _page = page;
    connect(page, &AbstractSetupWizardPage::requestNext, context->window(), &SetupWizardWidget::nextButtonClicked);
}

SetupWizardState BasicCredentialsSetupWizardState::state() const
{
    return SetupWizardState::CredentialsState;
}

void BasicCredentialsSetupWizardState::evaluatePage()
{
    auto *page = qobject_cast<BasicCredentialsSetupWizardPage *>(_page);
    Q_ASSERT(page != nullptr);

    _context->accountBuilder().setBasicAuthentication(page->user(), page->password());
    Q_EMIT evaluationSuccessful();
}

} // namespace OCC::Wizard

#include "basiccredentialssetupwizardstate.moc"
