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
#include "diff.h"
#include <QDebug>
#include <algorithm>

/*
 *	Implementation of Myers algorithm to find the diff between two strings.
 *  How it's used:
 *  	stringWithMapping: string that contains an identifying pattern, such as {0}, {PATT}, #HTOPIC#, UNUSED, etc;
 * 			Needs to be something that has no chance of being matched in stringMapAgainst
 * 		stringMapAgainst: Topic from device/client that will be re-mapped to a new topic
 *
 * 	Diff is applied to stringWithMappging and stringMapAgainst. The identifying patterns in stringWithMapping will "capture"
 *  a string in stringMapAgainst. These strings that are captured are then re-arranged in stringNewMapping. The capture groups that
 *  appear in stringWithMapping need to appear in stringNewMapping in the places you want to rearrange the topic, or leave them out
 * 	if you'd like to remove that portion. The curly braces aren't needed, I just tend to include them because it makes it easier for
 *  me to see the capture groups. Example 4 gives an example with no {}.
 *
 *  NOTE: This is very similar to https://docs.python.org/3/library/difflib.html and using difflib.ndiff(stringWithMapping, stringMapAgainst).
 *
 * 	Examples:
 * 		1. stringWithMapping: this/is/a/{0}-{1}/{2}
 * 		   stringMapAgainst: this/is/a/bad-topic/reallybad
 *         stringNewMapping: this/{0}/{2}
 * 		 	Making a Diff object and then calling getMappedTopic will return "this/bad/reallybad" as the new topic
 * 		2. stringWithMapping: this/is/a/{pass}-{DELETEME}/{REARRANGE}
 * 		   stringMapAgainst: this/is/a/bad-topic/reallybad
 *         stringNewMapping: this/is/{pass}/{REARRANGE}
 * 			This will yield the same as Example 1: "this/bad/reallybad" gets returned
 * 		3. stringWithMapping: this/is/a/{pass}-{DELETEME}/{REARRANGE}
 * 		   stringMapAgainst: this/is/a/bass-topic/reallybad
 *         stringNewMapping: this/{pass}/{REARRANGE}
 * 			This will return this/is/bass}/reallybad, but replacing {pass} with {PASS} will result in this/is/bad/reallybad
 *          NOTE: The algorithm is case sensitive!
 * 		4. stringWithMapping: this/is/a/?-@/!
 * 		   stringMapAgainst: this/is/a/bad-topic/test
 * 		   stringNewMapping: this/is/?/!
 * 			This will return this/is/bad/test
 */

struct Point {
    int x;
    int y;
    Point(int x_coord, int y_coord) : x(x_coord), y(y_coord) {}
};

Diff::Diff(QString p_stringWithMapping, QString p_stringMapAgainst, QString p_stringNewMapping) :
    stringWithMapping(p_stringWithMapping),
    stringMapAgainst(p_stringMapAgainst),
    stringNewMapping(p_stringNewMapping)
{

}

int Diff::mapIndex(int k, int vectorLength) {
    if (k < 0) {
        return k + vectorLength;
    }
    return k;
}

int Diff::shortestEditScriptLength() {
    int N = stringWithMapping.size();
    int M = stringMapAgainst.size();
    int x{0};
    int y{0};
    int const maxLength = N + M;
    QVector<int> V(maxLength + 2);
    int d;
    int k;
    for (d = 0; d <= maxLength; d++) {
        allVs.insert(allVs.begin(), V);
        for (k = -(d-2*std::max(0, d-M)); k <= d - 2*std::max(0, d-N); k+=2) {
            if (k == -d || (k != d and V[mapIndex(k-1, maxLength + 2)] < V[mapIndex(k+1, maxLength + 2)])) {
                x = V[mapIndex(k+1, maxLength + 2)];
            }
            else {
                x = V[mapIndex(k - 1, maxLength + 2)] + 1;
            }
            y = x - k;
            while (x < N && y < M && stringWithMapping[x] == stringMapAgainst[y]) {
                x = x + 1;
                y = y + 1;
            }
            V[mapIndex(k, maxLength + 2)] = x;
            if (x == N && y == M) {
                return d;
            }
        }
    }
    return -1;
}

void Diff::createMap() {
    std::vector<Point> points;
    int x = stringWithMapping.length();
    int y = stringMapAgainst.length();
    int maxLength = x + y;
    int x_mid{0};
    int y_mid{0};
    int x_temp{0};
    int y_temp{0};
    bool down = false;
    int d = allVs.size() - 1;
    int k{0};
    int prev_k{0};
    for (auto v: allVs) {
        points.emplace_back(x, y);
        k = x - y;
        down = (k == -d || (k != d && v[mapIndex(k-1, maxLength + 2)] < v[mapIndex(k+1, maxLength + 2)]));
        prev_k = down ? k + 1 : k - 1;
        x_temp = v[mapIndex(prev_k, maxLength + 2)];
        y_temp = x_temp - prev_k;
        x_mid = down ? x_temp : x_temp + 1;
        y_mid = x_mid - k;
        //could find diagonal coordinates here if desired
        /*
         * Diagonal points are found by iterating down on equal coordinates
         * But, we don't care where the two strings are equal, we only care where they aren't.
         *
         */
        if (x_mid != x and y_mid != y) {
            points.emplace_back(x_mid, y_mid);
        }
        x = x_temp;
        y = y_temp;
        d--;
    }
    QString key = "";
    QString value = "";
    for (int i = points.size() - 1; i > 0; i--) {
        Point point_1 = points[i];
        Point point_2 = points[i-1];
        if (point_1.x == point_2.x) {
            value += stringMapAgainst[point_1.y];
        }
        else if (point_1.y == point_2.y) {
            key += stringWithMapping[point_1.x];
        }
        else {
            if (!(key.isEmpty() || value.isEmpty())) {
                map[key] = value;
                key = "";
                value = "";
            }
        }
    }
    if (!(key.isEmpty() || value.isEmpty())) {
        map[key] = value;
    }
}

QString Diff::createNewTopic() {
    int pos{0};
    for (QHash<QString, QString>::const_iterator it = map.cbegin(), end=map.cend(); it != end; ++it) {
        pos = stringNewMapping.indexOf(it.key());
        if (pos > -1) {
            stringNewMapping.replace(it.key(), it.value());
        }
    }
    return stringNewMapping;
}

QString Diff::getMappedTopic() {
    int d = shortestEditScriptLength();
    Q_UNUSED(d);
    createMap();
    return createNewTopic();
}
