/*
 * ProvidenceKOSLookup - a KOS Checker
 * Copyright (C) 2014 Terra Nanotech <info@terra-nanotech.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PKOS_CVAKOSAPI_H
#define PKOS_CVAKOSAPI_H

#include <QtCore/QObject>
#include <QtScript/QScriptValue>
#include "network.h"
#include "../include/pkostypes.h"

namespace pkos {


/** @class CvaKosAPI
 *  @brief wrapper around the JSON CVA API
 */
class CvaKosAPI : public QObject
{
    Q_OBJECT

public:
    CvaKosAPI(QObject* parent, Network* net = 0);

    /**
     * @brief check a unit for KOS
     * @param unit Name of the Unit
     * @returns the state of the unit. not found values are -2
     */
    void checkUnits(kos_state_t& state);

    /**
     * @brief recheck a unit for KOS if we need more subchecks
     * @param unit Name of the NEW Unit to check against ( corp for example )
     * @param state the current state if the check, will be changed!
     * @param type  the type of the recheck 1 = corp, 2 = alliance
     * @returns the state of the unit. not found values are -2
     */
    void recheckUnit(kos_state_t& state, int type = 1);


private:
    void check(const QString& unit, kos_state_t& state, int type = 0);
    void parseResult(const QString& unit, const QString& data, pkos::kos_state_t& state, bool recheck);

    void parsePilot(kos_state_t& state, const QScriptValue& root);
    void parseCorp(kos_state_t& state, const QScriptValue& root);
    void parseAlliance(kos_state_t& state, const QScriptValue& root);

private:
    Network *net;
};

}

#endif // PKOS_CVAKOSAPI_H
