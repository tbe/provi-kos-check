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

#include "cvakosapi.h"

#include <QtCore/QDebug>
#include <QtNetwork/QNetworkReply>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValueIterator>

const QString apiURL("http://kos-api.terra-nanotech.de:8080/");

using namespace pkos;

CvaKosAPI::CvaKosAPI(QObject* parent, Network* net)
    :QObject(parent),net(net)
{
    if ( this->net == 0)
        this->net = new Network(this);
}

void CvaKosAPI::checkUnits(kos_state_t& state)
{
    this->check(state.checkname,state);
}

void CvaKosAPI::recheckUnit(kos_state_t& state, int type)
{
    QString unit;
    switch(type) {
    case 1:
        unit = state.corp_name;
        state.corp = -2;
        break;
    case 2:
        unit = state.alliance_name;
        state.alliance = -2;
        break;
    default:
        qWarning() << "unknown recheck type" << type;
        return;
        break;
    }
    this->check(unit,state,type);
}


void CvaKosAPI::check(const QString& unit, kos_state_t& state, int type)
{
    QMap<QString,QString> params;
    bool recheck;
    if ( type > 0 )
      recheck = true;
    else
      recheck = false;

    params["c"]    = "json";
    switch(type) {
    case 1:
        params["type"] = "corp";
        break;
    case 2:
        params["type"] = "alliance";
        break;
    default:
        params["type"] = "unit";

    };
    params["q"]    = unit;
    QNetworkReply* reply = this->net->httpSyncRequest(apiURL,params);
    if ( reply == 0 )
        return;

    QString data(reply->readAll());
    delete reply;
    this->parseResult(unit,data,state,true);
}


void CvaKosAPI::parseResult(const QString& unit,const QString& data, kos_state_t& state, bool recheck)
{

    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+data+")");
    QScriptValueIterator it(result.property("results"));

    while(it.hasNext()) {
        it.next();
        if ( it.value().isObject() ) {
            if ( it.value().property("label").toString().toLower().compare(unit.toLower()) != 0 )
                continue;
            if ( !recheck )
                state.unit_icon = it.value().property("icon").toString();
            QString type = it.value().property("type").toString();
            if ( type == "pilot" )
                parsePilot(state,it.value());
            else if ( type == "corp" )
                parseCorp(state,it.value());
            else if ( type == "alliance" )
                parseAlliance(state,it.value());
            else
                qWarning() << "unkown unit type" << type;
            break;
        }
    }
}

void CvaKosAPI::parsePilot(kos_state_t& state, const QScriptValue& root)
{
    state.pilot_name = root.property("label").toString();
    state.pilot      = root.property("kos").toInteger();
    state.pilot_id   = root.property("eveid").toInteger();
    parseCorp(state,root.property("corp"));
}

void CvaKosAPI::parseCorp(kos_state_t& state, const QScriptValue& root)
{
    state.corp_name  = root.property("label").toString();
    state.corp       = root.property("kos").toInteger();
    state.corp_npc   = root.property("npc").toBool();
    state.corp_id   = root.property("eveid").toInteger();
    parseAlliance(state,root.property("alliance"));
}

void CvaKosAPI::parseAlliance(kos_state_t& state, const QScriptValue& root)
{
    state.alliance_name  = root.property("label").toString();
    if ( state.alliance_name == "None" )
        state.alliance = -2;
    else {
        state.alliance       = root.property("kos").toInteger();
        state.alliance_id    = root.property("eveid").toInteger();
    }
}


#include "cvakosapi.moc"
