/*
 * Copyright (C) 2020 brian DOT l DOT miller DOT ttu AT gmail DOT com
 * This file is part of MQTTMockingbird.
 *
 * This program comes with ABSOLUTELY NO WARRANTY
 * MQTTMockingbird is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MQTTMockingbird is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MQTTMockingbird.  If not, see <https://www.gnu.org/licenses/.
 */
#ifndef DIFF_H
#define DIFF_H
#include <QObject>
#include <QHash>
#include <unordered_map>


class Diff
{
public:
    Diff(QString p_stringWithMapping, QString p_stringMapAgainst, QString p_stringNewMapping);
    QString topicName;
    QString oldTopicName;
    QString stringWithMapping;
    QString stringMapAgainst;
    QString stringNewMapping;
    QVector<QVector<int>> allVs;
    QHash<QString, QString> map;

    int mapIndex(int, int);
    int shortestEditScriptLength();
    void createMap();
    QString createNewTopic();
    QString getMappedTopic();
};

#endif // DIFF_H
