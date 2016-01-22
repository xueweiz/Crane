Crane
======================
Crane is a Storm-like stream processing platform. 

For design details, please check out the report (pdf file). 

Code Structure
--------------
### Address.add, AddrIntro.add
Store the IP address or hostname of you machines. One should be put in AddrIntro.add, all the others in Address.add.

### Bolt*
Just like Bolt in Storm.

### Crane
Manager of all the bolts and spouts in current node. 

### Chrono
Timing library. 

### FileSystem
Support for distributed file system manipulation. 

### Spout*
Just like Spout in Storm.

### Supervisor*
Supervisor class is only created in one machine. That machine use Supervisor to assign tasks (spouts and bolts) to other worker machines. 

### Tuple
Just like Tuple in Storm.

### Membership
Membership control module using SWIM-protocol. It detects machine joining/leaving/failure. 

### Connections
Helper methods for TCP and UDP connections, and robust data transfer. 

### crane_types
Type management. 

Authors
--------------
Luis Remis (remis2 at illinois dot edu)

Xuewei Zhang (xzhan160 at illinois dot edu)
