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
