# MQTTMockingbird

<h1>Introduction</h1>
This is a personal project that was aimed at (1) learning and implemmenting Myers algorithm and (2) learning about MQTT. Note that this is neither an MQTT client nor a broker. Rather it acts as a "bridge" between the two. The purpose of the program is to give more control over topic names. Some devices and clients have  <i>really bad</i> topics and give the user very little control on how they are constructed.

This program can handle multiple clients, but currently can only handle one topic mapping per client. I think it may be possible in the future to handle multiple mappings per client, but I haven't delved too far into it yet. The program connects to only one broker.

<h2>Environment</h2>
This was deveoped using Qt 5.14.2 using Qt Creator 4.12.3 on Fedora 31 and openSUSE Tumbleweed. Algorithms and data structures were developed in KDevelop 5.5.2 using GCC 9.3.1 and then converted into Qt.

<h1>Topic Mappings</h1>
<h2>Overview</h2>
The program generally passes most MQTT messages to a broker and only intercepts those that deal with topic names. If a mapping has been defined for a client, the program will apply diff to all incoming and outgoing topics and rewrite parts of the MQTT message that deal with topic names. It will keep all QoS, Will, and Duplicate settings.

There are several examples of how diff.cpp works and is applied, but the TLDR is this: a topic name needs patterns that identify parts of the topic that are to be extracted and either re-mapped in new positions or taken out completely. It is a specialized 

Here are some different examples than those in diff.cpp:
<h2>Examples</h2> 
<ol>
  <li> <i>stringWithMapping</i>: this/is/a/{0}-{1}/{2}, <i>stringMapAgainst</i>: this/is/a/bad-topic/reallybad, <i>stringNewMapping</i>: this/{0}/{2}:
    This will create the new topic this/bad/reallybad
   </li>
  <li><i>stringWithMapping</i>:new/CAP1thereCAP2, <i>stringMapAgainst</i>:new/hellotheremaam, <i>stringNewMapping</i>: CAP1/CAP2:
  This will create the new topic: hello/maam
   </li>
  <li><i>stringWithMapping</i>:device/uglytopic/{0}, <i>stringMapAgainst</i>:uglytopic/PIT301, <i>stringNewMapping</i>: device/{0}:
  This inadvertently turns "device" in a capture group since it doesn't exactly match anything in stringMapAgainst. Thus the resulting new topic is just "PIT301."
  </li>
</ol>
<h1>Topic Trie</h1>
<p>
There are dozens of ways to implement a trie in C++. This approach is similar to one that I have taken previously in Python. Although there are Python packages for tries, a topic trie is very specialized in that it must deal with wildcards, so you're left having to implement a custom trie yourself. A very easy and straight-forward approach in Python is to make a trie out of nested dictionaries. Here in Qt we use an unordered_map. We could have just as easily used a QHash. 
</p>
<p>
Because the program maps all incoming publish and subscribe topics, if you need wildcard in a subscription it would be best to include in the mapping definitions, namely in the remap topic rule. In the future I plan to have diff disregard topic fragments that contain '*' or '#.' It will probably apply diff to each topic fragment, i.e. each string between forward slashes.
</p>
<h1>Screenshots</h1>
![Alt text](/screenshots/main_window_rule_for_node_red.png?raw=true "Main Window")

![Alt text](/screenshots/main_windows_clients_tab.png?raw=true "Shows incoming topic mapped")
Subclass QWidget to make custom widget show up in QListWidget.

![Alt text](/screenshots/mapped_topic_publish.png?raw=true "Shows incoming topic mapped")
Show incoming topic mapped to a new topic.  
  

