<shadow>
  <!-- our network -->
  <topology>
    <![CDATA[<?xml version="1.0" encoding="utf-8"?><graphml xmlns="http://graphml.graphdrawing.org/xmlns" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://graphml.graphdrawing.org/xmlns http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd">
             <key attr.name="packetloss" attr.type="double" for="edge" id="d9" />
             <key attr.name="jitter" attr.type="double" for="edge" id="d8" />
             <key attr.name="latency" attr.type="double" for="edge" id="d7" />
             <key attr.name="asn" attr.type="int" for="node" id="d6" />
             <key attr.name="type" attr.type="string" for="node" id="d5" />
             <key attr.name="bandwidthup" attr.type="int" for="node" id="d4" />
             <key attr.name="bandwidthdown" attr.type="int" for="node" id="d3" />
             <key attr.name="geocode" attr.type="string" for="node" id="d2" />
             <key attr.name="ip" attr.type="string" for="node" id="d1" />
             <key attr.name="packetloss" attr.type="double" for="node" id="d0" />
             <graph edgedefault="undirected">
               <node id="poi-1">
                 <data key="d0">0.0</data>
                 <data key="d1">0.0.0.0</data>
                 <data key="d2">US</data>
                 <data key="d3">10000020000040</data>
                 <data key="d4">10000240</data>
                 <data key="d5">net</data>
                 <data key="d6">0</data>
               </node>
               <edge source="poi-1" target="poi-1">
                 <data key="d7">50.0</data>
                 <data key="d8">0.0</data>
                 <data key="d9">0.0</data>
               </edge>
             </graph>
             </graphml>]]>
  </topology>

  <!-- the plug-ins we will be using -->
  <plugin id="NODE" path="~/.bleep/plugins/libNODE_BL_TEST.so" />


  <!-- the length of our experiment in seconds  1404101800-->
  <kill time="300" />
  <!-- our nodes -->

  <node id="bleep0">
    <application plugin="NODE" time="5" arguments="-id=bleep0" />
    <!-- <command id="setMyId" starttime="5" arguments="bleep0" /> -->
  </node>

  <node id="bleep1">
    <application plugin="NODE" time="13" arguments="-id=bleep1 -connect=bleep0" />
  </node>

  <!-- <node id="bleep1"> -->
  <!--   <application plugin="PEER_NONBLOCK_IO" time="13" arguments="bleep0"/> -->
  <!-- </node> -->

</shadow>
