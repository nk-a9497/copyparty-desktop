/*
 * Copyright (C) Fabian Müller <fmueller@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include "abstractsetupwizardpage.h"

class QLineEdit;
class QLabel;

namespace OCC::Wizard {

/**
 * Page used to enter HTTP Basic credentials. Used for plain WebDAV servers
 * (e.g. copyparty) where no OpenCloud / OAuth login flow exists.
 */
class BasicCredentialsSetupWizardPage : public AbstractSetupWizardPage
{
    Q_OBJECT

public:
    explicit BasicCredentialsSetupWizardPage(const QString &initialUser);

    QString user() const;
    QString password() const;

    bool validateInput() const override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    QLineEdit *_userLineEdit = nullptr;
    QLineEdit *_passwordLineEdit = nullptr;
};
}
