#include &amp;amp;amp;amp;amp;amp;lt;omnetpp.h&amp;amp;amp;amp;amp;amp;gt;
class Sink: public cSimpleModule
{ Module_Class_Members(Sink, cSimpleModule, 8192);
virtual void activity(); };
Define_Module(Sink);
void Sink::activity()
{ while(1)
{ cMessage *msg = receive(); int pkt_type = msg -&amp;amp;amp;amp;amp;amp;gt; kind();
if (pkt_type ==1) ev &amp;amp;amp;amp;amp;amp;lt;&amp;amp;amp;amp;amp;amp;lt; “Received data packet\n”;
else ev &amp;amp;amp;amp;amp;amp;lt;&amp;amp;amp;amp;amp;amp;lt; “Received voice packet\n”; delete msg;} }
