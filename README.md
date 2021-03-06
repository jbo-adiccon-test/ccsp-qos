 
 Application usage 
-----------------------------------------------------------------

** Special Branch for exercises
* Run the ccsp qos binary:

  > ccsp_qos -subsys eRT.

* There are two cases when the data model is applied to the system:

  * After changing the Enable property of Classification or Queue nodes
  * After removing the Classification or Queue nodes from data model tree

* Data is stored in json files:

  * /usr/ccsp/qos/QoS_defaults.json response for default setup values
  * /usr/ccsp/qos/QoS_config_current.json response for storing working data model values

  It's possible to remove QoS_config_current.json file and restart the ccsp_qos to restore the data model to the default state.

We will use "cake" with diffserv3 instead of htb prioritiy queueing.
e.g. tc qdisc add dev erouter0 root cake bandwidth 40Mbit overhead 0 mpu 0 diffserv4


 Data model usage
-----------------------------------------------------------------

* Add new class and set parameters example:

  > dmcli eRT addtable Device.QoS.Classification.

  > dmcli eRT setv Device.QoS.Classification.1.TrafficClass int 5

  > dmcli eRT setv Device.QoS.Classification.1.ChainName string "OUTPUT"

  > dmcli eRT setv Device.QoS.Classification.1.IfaceOut string "erouter0"

  > dmcli eRT setv Device.QoS.Classification.1.SourcePort int 40000

  > dmcli eRT setv Device.QoS.Classification.1.SourcePortRangeMax int 40016

  > dmcli eRT setv Device.QoS.Classification.1.DestPort int -1

  > dmcli eRT setv Device.QoS.Classification.1.DestPortRangeMax int -1

  > dmcli eRT setv Device.QoS.Classification.1.Protocol int 17

  > dmcli eRT setv Device.QoS.Classification.1.DSCPMark int 46

  > dmcli eRT setv Device.QoS.Classification.1.Enable bool true

* Add new queue and set parameters example:
* Since we used "cake" as queueing discipline, we only need to add single Queue and specify Interface and ShapingRate

  > dmcli eRT addtable Device.QoS.Queue.

  > dmcli eRT setv Device.QoS.Queue.1.Interface string "erouter0"
  
  > dmcli eRT setv Device.QoS.Queue.1.ShapingRate int 25

  > dmcli eRT setv Device.QoS.Queue.1.Enable bool true

* Delete existing nodes example:

  > dmcli eRT deltable Device.QoS.Queue.N.

  > dmcli eRT deltable Device.QoS.Classification.N.

    where N is the instance number of the corresponding node 

 Data model description
-----------------------------------------------------------------

 * Device.QoS.Classification.{i}.

    Enable              - Enables or disables this classifier.

    DestIP              - Classification criterion. Destination IP address. An empty string indicates this criterion is not used 
                          for classification.

    DestMask            - Destination IP address mask, represented as an IP routing prefix using CIDR notation

    SourceIP            - Classification criterion. Source IP address. An empty string indicates this criterion is not used for 
                          classification.

    SourceMask          - Source IP address mask, represented as an IP routing prefix using CIDR notation 

    SourcePort          - Classification criterion. Source port number. A value of -1 indicates this criterion is not used for 
                          classification.

    SourcePortRangeMax  - Rule criterion. If specified, indicates the Rule criterion is to include the port range from SourcePort 
                          through SourcePortRangeMax (inclusive). If specified, SourcePortRangeMax MUST be greater than or equal to 
                          SourcePort. A value of -1 indicates that no port range is specified.

    DestPort            - Classification criterion. Destination port number. A value of -1 indicates this criterion is not used for 
                          classification.

    DestPortRangeMax    - Classification criterion. If specified, indicates the classification criterion is to include the port 
                          range from  DestPort through DestPortRangeMax (inclusive). If specified, DestPortRangeMax MUST be greater 
                          than or equal to DestPort. A value of -1 indicates that no port range is specified.

    Protocol            - Classification criterion. Protocol number. A value of -1 indicates this criterion is not used for 
                          classification.

    DSCPMark            - Classification result. DSCP to mark traffic with that falls into this classification entry. A value of -1
                          indicates no change from the incoming packet. A value of -2 indicates automatic marking of DSCP based 
                          upon the EthernetPriority

    TrafficClass        - Classification result. Identifier of the traffic class associated with traffic that falls in this 
                          classification. Should be the same as Precedence of corresponding Queue; not applicable with cake as queueing discipline

    ChainName           - Chain: PREROUTING, INPUT, FORWARD, OUTPUT, POSTROUTING

    IfaceIn             - Input interface

    IfaceOut            - Output interface

    TcpFlags            - Flags: SYN,ACK,FIN,RST ACK except PSH

    TcpPsh              - Flags: ALL, PSH


 * Device.QoS.Queue.{i}.

    Enable              - Enables or disables this queue.

    TrafficClasses      - Comma-separated list (maximum list length 256) of unsigned integers. Each list item identifies the set of 
                          traffic classes associated with this queue. Traffic is sent to this queue if a Classification, App or Flow table 
                          entry specifies a traffic class, e.g. via the Classification.{i}.TrafficClass parameter. If more than one queue 
                          on a given egress interface is associated with a given traffic class, the implementation will choose which queue 
                          to send traffic of this class to.

    Interface           - The value MUST be the Path Name of a table row. If the referenced object is deleted, the parameter value 
                          MUST be  set to an empty string. Specifies the egress interface for which the specified queue MUST exist. 
                          This MAY be a  layer 1, 2 or 3 interface, however, the types of interfaces for which Queues can be instantiated is a local matter to the CPE.

    Weight              - Weight of this queue in case of WFQ or WRR, but only used for queues of equal precedence.

    Precedence          - Precedence of this queue relative to others. Lower numbers imply greater precedence.

    SchedulerAlgorithm  - Scheduling Algorithm used by scheduler. Enumeration of: WFQ (Weighted Fair Queueing), WRR (Weighted Round 
                          Robin) SP (Strict Priority)

    ShapingRate         - Rate to shape this queue's traffic to. For leaky bucket (constant rate shaping), this is the constant 
                          rate. For token bucket (variable rate shaping), this is the average rate.
                          If <= 100, in percent of the rate of the highest rate-constrained layer over which the packet will travel 
                          on egress. If > 100, in bits per second. A value of -1 indicates no shaping.
                          For example, for packets destined for a WAN DSL interface, if the egress will be on a PPP or IP link with 
                          a specified ShapingRate, the percentage is calculated relative to this rate. Otherwise, if the ATM layer 
                          is rate-constrained, then the rate is calculated relative to this rate. Otherwise, the rate is calculated 
                          relative to the physical-layer DSL rate.


 Example of a test setup (configuring prioritization to Voioce and Video Queue of "Cake DiffServ4")
-----------------------------------------------------------------

dmcli eRT addtable Device.QoS.Queue.

dmcli eRT setv Device.QoS.Queue.1.Interface string "erouter0"

dmcli eRT setv Device.QoS.Queue.1.ShapingRate int 25

dmcli eRT setv Device.QoS.Queue.1.Enable bool true


dmcli eRT addtable Device.QoS.Classification.

# dmcli eRT setv Device.QoS.Classification.1.TrafficClass int 2

dmcli eRT setv Device.QoS.Classification.1.ChainName string "postrouting_qos"

dmcli eRT setv Device.QoS.Classification.1.IfaceOut string "erouter0"

dmcli eRT setv Device.QoS.Classification.1.SourcePort int -1

dmcli eRT setv Device.QoS.Classification.1.SourcePortRangeMax int -1

dmcli eRT setv Device.QoS.Classification.1.DestPort int -1

dmcli eRT setv Device.QoS.Classification.1.DestPortRangeMax int -1

dmcli eRT setv Device.QoS.Classification.1.Protocol int 17

dmcli eRT setv Device.QoS.Classification.1.DSCPMark int 34

dmcli eRT setv Device.QoS.Classification.1.DestIP string "192.168.177.114"

dmcli eRT setv Device.QoS.Classification.1.DestMask string "32"

dmcli eRT setv Device.QoS.Classification.1.Enable bool true


dmcli eRT addtable Device.QoS.Classification.

dmcli eRT setv Device.QoS.Classification.2.TrafficClass int 2

dmcli eRT setv Device.QoS.Classification.2.ChainName string "postrouting_qos"

dmcli eRT setv Device.QoS.Classification.2.IfaceOut string "erouter0"

dmcli eRT setv Device.QoS.Classification.2.SourcePort int -1

dmcli eRT setv Device.QoS.Classification.2.SourcePortRangeMax int -1

dmcli eRT setv Device.QoS.Classification.2.DestPort int -1

dmcli eRT setv Device.QoS.Classification.2.DestPortRangeMax int -1

dmcli eRT setv Device.QoS.Classification.2.Protocol int 17

dmcli eRT setv Device.QoS.Classification.2.DSCPMark int 32

dmcli eRT setv Device.QoS.Classification.2.DestIP string "192.168.177.115"

dmcli eRT setv Device.QoS.Classification.2.DestMask string "32"

dmcli eRT setv Device.QoS.Classification.2.Enable bool true

