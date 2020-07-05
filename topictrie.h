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
    TrieNode(QString topicFragment);
    TrieNode();
    std::unordered_map<QString, TrieNode*> children;
    void insert(QString topicFragment);
    void setEnd(bool);
    bool isEnd();
    TrieNode* getChildFromTopic(QString topic);
    int getNumberOfChildren();
    void setTopicFragment(QString topic);
    void setMapToTopic(QString newTopic);
    void printChildren();
    void printChildren(TrieNode* tn);
    QString mapToTopic;
};

class TopicTrie
{
public:
    TopicTrie();
    bool end;
    std::unordered_map<QString, TrieNode*> map;
    void insertNewTopic(QString topic, QString newTopic = "");
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
