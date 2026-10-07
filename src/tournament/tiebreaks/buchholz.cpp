// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Manuel Alcaraz Zambrano <manuel@alcarazzam.dev>

#include "buchholz.h"

double Buchholz::contribution(double pointsForResult, double value) const
{
    Q_UNUSED(pointsForResult);
    return value;
}
