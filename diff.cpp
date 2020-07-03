#include "diff.h"
#include <QDebug>
#include <algorithm>

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
    //int N = stringWithMapping.size(); old_string
    //int M = stringMapAgainst.size();  new_string
    //std::unordered_map<QString, QString> map;
    //QHash<QString, QString> map;
    //QVector<Point> points;
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
        //qDebug() << "x, y: " << x << " " << y << " " << d;
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
         * Diagonal points are found by iterating down on
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
        //qDebug() << "key, value: " << key << " " << value;
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
