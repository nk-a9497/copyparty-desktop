/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "copypartytheme.h"

#include <QCoreApplication>
#include <QIcon>
#include <QString>
#include <QVariant>

namespace OCC {

CopypartyTheme::CopypartyTheme()
    : Theme()
{
}

QColor CopypartyTheme::wizardHeaderBackgroundColor() const
{
    return QColor("#20434F");
}

QColor CopypartyTheme::wizardHeaderTitleColor() const
{
    return Qt::white;
}

QIcon CopypartyTheme::wizardHeaderLogo() const
{
    return Resources::themeUniversalIcon(QStringLiteral("wizard_logo"));
}

QmlButtonColor CopypartyTheme::primaryButtonColor() const
{
    return {"#E2BAFF", "#19353F", "#DADADA"};
}

QmlButtonColor CopypartyTheme::secondaryButtonColor() const
{
    return {"#CA8DF5", "#19353F", "#B0B0B0"};
}
}
