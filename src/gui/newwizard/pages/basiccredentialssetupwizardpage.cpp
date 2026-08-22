/*
 * Copyright (C) Fabian Müller <fmueller@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "gui/newwizard/pages/basiccredentialssetupwizardpage.h"

#include "libsync/copyparty.h"
#include "libsync/theme.h"

#include <QFormLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

namespace OCC::Wizard {

BasicCredentialsSetupWizardPage::BasicCredentialsSetupWizardPage(const QString &initialUser)
{
    auto *outerLayout = new QVBoxLayout(this);

    auto *logoLabel = new QLabel(this);
    logoLabel->setPixmap(Theme::instance()->wizardHeaderLogo().pixmap(200, 200));
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setAccessibleName(tr("%1 logo").arg(Theme::instance()->appNameGUI()));
    outerLayout->addWidget(logoLabel);

    auto *welcomeLabel = new QLabel(tr("Enter your credentials"), this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    outerLayout->addWidget(welcomeLabel);

    auto *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setFormAlignment(Qt::AlignHCenter | Qt::AlignTop);

    _userLineEdit = new QLineEdit(initialUser, this);
    _userLineEdit->setMinimumWidth(300);
    auto *userLabel = new QLabel(tr("Username"), this);
    userLabel->setBuddy(_userLineEdit);
    formLayout->addRow(userLabel, _userLineEdit);

    _passwordLineEdit = new QLineEdit(this);
    _passwordLineEdit->setEchoMode(QLineEdit::Password);
    _passwordLineEdit->setMinimumWidth(300);
    auto *passwordLabel = new QLabel(tr("Password"), this);
    passwordLabel->setBuddy(_passwordLineEdit);
    formLayout->addRow(passwordLabel, _passwordLineEdit);

    outerLayout->addLayout(formLayout);
    outerLayout->addStretch();

    connect(_userLineEdit, &QLineEdit::textChanged, this, &AbstractSetupWizardPage::contentChanged);
    connect(_passwordLineEdit, &QLineEdit::textChanged, this, &AbstractSetupWizardPage::contentChanged);

    connect(this, &AbstractSetupWizardPage::pageDisplayed, this, [this]() {
        if (_userLineEdit->text().isEmpty()) {
            _userLineEdit->setFocus();
        } else {
            _passwordLineEdit->setFocus();
        }
    });
}

QString BasicCredentialsSetupWizardPage::user() const
{
    return _userLineEdit->text().trimmed();
}

QString BasicCredentialsSetupWizardPage::password() const
{
    return _passwordLineEdit->text();
}

bool BasicCredentialsSetupWizardPage::validateInput() const
{
    return !user().isEmpty() && !password().isEmpty();
}

void BasicCredentialsSetupWizardPage::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (validateInput()) {
            Q_EMIT requestNext();
        }
    }
    AbstractSetupWizardPage::keyPressEvent(event);
}

} // namespace OCC::Wizard

#include "basiccredentialssetupwizardpage.moc"
