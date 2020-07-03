#ifndef DIFF_H
#define DIFF_H
#include <QObject>
#include <QHash>
#include <unordered_map>


class Diff
{
public:
    Diff(QString, QString, QString);
    QString topicName;
    QString oldTopicName;
    QString stringWithMapping;
    QString stringMapAgainst;
    QString stringNewMapping;
    QVector<QVector<int>> allVs;
    //std::unordered_map<QString, QString> oldToNewMapping;
    QHash<QString, QString> map;

    int mapIndex(int, int);
    int shortestEditScriptLength();
    //std::unordered_map<QString, QString> createMap();
    void createMap();
    QString createNewTopic();
    QString getMappedTopic();
};

#endif // DIFF_H
