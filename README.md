# MQTTMockingbird

<h1>Introduction</h1>
This is a personal project that was aimed at (1) learning and implemmenting Myers algorithm and (2) learning about MQTT. Note that this is neither an MQTT client nor a broker. Rather it acts as a "bridge" between the two. The purpose of the program is to give more control over topic names. Some devices and clients have  <i>really bad</i> topics and give the user very little control on how they are constructed.

This program can handle multiple clients, but currently can only handle one topic mapping per client. I think it may be possible in the future to handle multiple mappings per client, but I haven't delved too far into it yet. The program connects to only one broker.

<h1>Topic Mappings</h1>
<h2>Overview</h2>
The program generally passes most MQTT messages to a broker and only intercepts those that deal with topic names. If a mapping has been defined for a client, the program will apply diff to all incoming and outgoing topics and rewrite parts of the MQTT message that deal with topic names. It will keep all QoS, Will, and Duplicate settings.

There are several examples of how diff.cpp works and is applied, but the TLDR is this: a topic name needs patterns that identify parts of the topic that are to be extracted and either re-mapped in new positions or taken out completely. It is a specialized 

Here are some different examples than those in diff.cpp:
<h2>Examples</h2> 
<ol>
  <li> stringWithMapping: this/is/a/{0}-{1}/{2}, stringMapAgainst: this/is/a/bad-topic/reallybad, stringNewMapping: this/{0}/{2}

</ol>
