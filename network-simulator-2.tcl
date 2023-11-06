# Create nodes in the network
set node(s1) [$ns node]
set node(s2) [$ns node]
set node(r1) [$ns node]
set node(r2) [$ns node]
set node(s3) [$ns node]
set node(s4) [$ns node]

# Define duplex links between nodes
$ns duplex-link $node(s1) $node(r1) 15Mb 2.5ms DropTail
$ns duplex-link $node(s2) $node(r1) 15Mb 3.2ms DropTail
$ns duplex-link $node(r1) $node(r2) 2.5Mb 22ms RED

# Set queue limits for routers r1 and r2
$ns queue-limit $node(r1) $node(r2) 28
$ns queue-limit $node(r2) $node(r1) 28

# Define more duplex links
$ns duplex-link $node(s3) $node(r2) 15Mb 4.2ms DropTail
$ns duplex-link $node(s4) $node(r2) 15Mb 5ms DropTail

# Configure link orientation and queue positions
$ns duplex-link-op $node(s1) $node(r1) orient right-down
$ns duplex-link-op $node(s2) $node(r1) orient right-up
$ns duplex-link-op $node(r1) $node(r2) orient right
$ns duplex-link-op $node(r1) $node(r2) queuePos 0
$ns duplex-link-op $node(r2) $node(r1) queuePos 0
$ns duplex-link-op $node(s3) $node(r2) orient left-down
$ns duplex-link-op $node(s4) $node(r2) orient left-up

# Create TCP connections and configure window size
set tcp1 [$ns create-connection TCP/Reno $node(s1) TCPSink $node(s3) 
