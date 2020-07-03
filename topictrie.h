#ifndef TOPICTRIE_H
#define TOPICTRIE_H
#include <QVector>
#include <QString>
#include <unordered_map>

class TrieNode {
private:
    std::unordered_map<QString, TrieNode*>::iterator childrenIterator;
    QString topicFragment;
    bool end = false;
public:
    TrieNode(QString);
    TrieNode();
    std::unordered_map<QString, TrieNode*> children;
    void insert(QString);
    void setEnd(bool);
    bool isEnd();
    TrieNode* getChildFromTopic(QString);
    int getNumberOfChildren();
    void setTopicFragment(QString);
    void setMapToTopic(QString);
    void printChildren();
    void printChildren(TrieNode*);
    QString mapToTopic;
};

class TopicTrie
{
public:
    TopicTrie();
    bool end;
    std::unordered_map<QString, TrieNode*> map;
    void insertNewTopic(QString, QString newTopic = "");
    bool search(QString topic);
    bool deleteTopic(QString topic);
    QString getMapToTopic(QString topic);

    void setMappingRule(QString p_mappingRule);
    QString mappingRule;

    void setRemapRule(QString p_remapRule);
    QString remapRule;
private:
    TrieNode *root;
};

#endif // TOPICTRIE_H
