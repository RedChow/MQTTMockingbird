#include "topictrie.h"
#include <QDebug>

/*
 *	TrieNode & TopicTrie
 *  	Each string separated by a forward slash is a node. E.g.,
 * 		this/is/a/topic becomes:
 * 		this
 * 		  |
 *       is
 *        |
 *        a
 *        |
 *      topic
 * and then this/is/another/topic becomes:
 *      this
 * 	      |
 *        is
 *       /  \
 *      a   another
 *      |       |
 *    topic   topic
 *  Each topic than comes in gets mapped to a new topic. So at the end of the Trie, we keep the mapped topic.
 *
 * For example, suppose this/is/a/topic gets mapped to this/topic/a and this/is/another/topic maps to this/topic/another,
 * the function getMapToTopic("this/is/a/topic") returns this/topic/a and getMapToTopic("this/is/another/topic") returns
 * this/topic/another. These two return values are the topics that are sent to the broker.
 */

TrieNode::TrieNode() {

}

TrieNode::TrieNode(QString topicFragment) {
    children[topicFragment] = new TrieNode();
}

void TrieNode::insert(QString topicFragment) {
    children[topicFragment] = new TrieNode();
}

void TrieNode::setEnd(bool b) {
    end = b;
}

bool TrieNode::isEnd() {
    return end;
}

TrieNode* TrieNode::getChildFromTopic(QString topic) {
    if (children.find(topic) != children.end()) {
        return children[topic];
    }
    return nullptr;
}

int TrieNode::getNumberOfChildren() {
    return children.size();
}

void TrieNode::setTopicFragment(QString topic) {
    topicFragment = topic;
}

void TrieNode::setMapToTopic(QString newTopic) {
    mapToTopic = newTopic;
}

void TrieNode::printChildren() {
    for (auto t: children) {
        qDebug() << " " << t.first;
        this->printChildren(t.second);
    }
    qDebug() << "";
}

void TrieNode::printChildren(TrieNode* tn) {
    if (!tn->end ) {
        for (auto t: tn->children) {
            qDebug() << " " << t.first;
            tn->printChildren(t.second);
        }
    }
}

/*
 * 	TopicTrie
 */

TopicTrie::TopicTrie() {
    root = new TrieNode();
}

void TopicTrie::insertNewTopic(QString topic, QString newTopic) {
    //weak_ptr<int> weakPtr(sharedPtr);
    TrieNode* current = root;
    QStringList explodedTopic = topic.split("/");
    for (auto t: explodedTopic) {
        if (current->getChildFromTopic(t) == NULL) {
            current->insert(t);
        }
        current = current->getChildFromTopic(t);
    }
    current->setEnd(true);
    current->setMapToTopic(newTopic);
}

bool TopicTrie::search(QString topic) {
    TrieNode* current = root;
    QStringList explodedTopic = topic.split("/");
    for (auto t: explodedTopic) {
        if (current->getChildFromTopic(t) == NULL) {
            return false;
        }
        current = current->getChildFromTopic(t);
    }
    if (current->isEnd()) {
        return true;
    }
    return false;
}

bool TopicTrie::deleteTopic(QString topic) {
    TrieNode* current = root;
    TrieNode* deleteNode = root;
    QString lastTopic = "";
    QStringList explodedTopic = topic.split("/");
    for (auto t: explodedTopic) {
        //if there's more than one child, there's another path that needs to stay
        //essentially finding shortest path that is not a part of another path
        if (current->getNumberOfChildren() > 1) {
            deleteNode = current;
            lastTopic = t;
        }
        if (current->getChildFromTopic(t) == NULL) {
            return false;
        }
        current = current->getChildFromTopic(t);
    }
    //This will delete the path
    delete deleteNode->children[lastTopic];
    deleteNode->children.erase(lastTopic);
    return true;
}

QString TopicTrie::getMapToTopic(QString topic) {
    TrieNode* current = root;
    QStringList explodedTopic = topic.split("/");
    for (auto t: explodedTopic) {
        if (current->getChildFromTopic(t) == NULL) {
            return QString("");
        }
        current = current->getChildFromTopic(t);
    }
    if (current->isEnd()) {
        return current->mapToTopic;
    }
    return QString("");
}

void TopicTrie::setMappingRule(QString p_mappingRule) {
    mappingRule = p_mappingRule;
}

void TopicTrie::setRemapRule(QString p_remapRule) {
    remapRule = p_remapRule;
}
