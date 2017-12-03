# Hadoop Single Node Installation
## Java environment setup
```bash
sudo apt-get update
sudo apt-get install default-jdk
```
## Adding a dedicated Hadoop system user
This will add the user hduser and the group hadoop to your local machine.
```bash
sudo addgroup hadoop
sudo adduser --ingroup hadoop hduser
```

## Configuring SSH
```bash
su hduser
ssh-keygen -t rsa -P ""
cat $HOME/.ssh/id_rsa.pub >> $HOME/.ssh/authorized_keys
```

The output looks like this:
```
hduser@hanson:~$ ssh-keygen -t rsa -P ""
Generating public/private rsa key pair.
Enter file in which to save the key (/home/hduser/.ssh/id_rsa):
Created directory '/home/hduser/.ssh'.
Your identification has been saved in /home/hduser/.ssh/id_rsa.
Your public key has been saved in /home/hduser/.ssh/id_rsa.pub.
The key fingerprint is:
SHA256:VJjkPSKe0LsRjcS+avfbA/kolHR5iPP6OVDp1QaT2EM hduser@hanson
The key's randomart image is:
+---[RSA 2048]----+
|     ....*E.     |
|     o.++o*      |
|    ..=.=+o=     |
|     o=**.o.o    |
|     .=OS+ .     |
|      =o=        |
|     o.o +       |
|    o + ooo      |
|   . . +=o..     |
+----[SHA256]-----+

```

**enable SSH access to your local machine with this newly created key.**
```bash
hduser@hanson:~$ s
```
**test the SSH setup by connecting to your local machine with the `hduser` user**
```bash
hduser@hanson:~$ ssh localhost
```
You should install `ssh` first ( sudo apt-get install ssh)

Output should be like this:
```bash
The authenticity of host 'localhost (127.0.0.1)' can't be established.
ECDSA key fingerprint is SHA256:emnk3O6O2N7CK8chUMIThK3CGFUwFOS44kzsa0phArE.
Are you sure you want to continue connecting (yes/no)? yes
Warning: Permanently added 'localhost' (ECDSA) to the list of known hosts.
Welcome to Ubuntu 17.10 (GNU/Linux 4.13.0-17-generic x86_64)

* Documentation:  https://help.ubuntu.com
* Management:     https://landscape.canonical.com
* Support:        https://ubuntu.com/advantage


0 packages can be updated.
0 updates are security updates.


The programs included with the Ubuntu system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Ubuntu comes with ABSOLUTELY NO WARRANTY, to the extent permitted by
applicable law.
```

## Disabling IPv6
One problem with IPv6 on Ubuntu is that using `0.0.0.0` for the various networking-related Hadoop configuration options will result in Hadoop binding to the IPv6 addresses of my Ubuntu box. In my case, I realized that there’s no practical point in enabling IPv6 on a box when you are not connected to any IPv6 network. Hence, I simply disabled IPv6 on my Ubuntu machine. Your mileage may vary.

To disable IPv6 on Ubuntu 10.04 LTS, open `/etc/sysctl.conf` in the editor of your choice and add the following lines to the end of the file:
```bash
# disable ipv6
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
```
You have to reboot your machine in order to make the changes take effect.

You can check whether IPv6 is enabled on your machine with the following command:
```bash
cat /proc/sys/net/ipv6/conf/all/disable_ipv6
```
If output is 1, then ipv6 is disabled.

### Alternative

You can also disable IPv6 only for Hadoop as documented in [HADOOP-3437](https://issues.apache.org/jira/browse/HADOOP-3437). You can do so by adding the following line to `hadoop/etc/hadoop/hadoop-env.sh`:
```bash
export HADOOP_OPTS=-Djava.net.preferIPv4Stack=true
```
## Hadoop installation
Download Hadoop from the `Apache Download Mirrors` and extract the contents of the Hadoop package to a location of your choice. I picked $HADOOP_HOME. Make sure to change the owner of all the files to the hduser user and hadoop group, for example:
- wget http://mirrors.ocf.berkeley.edu/apache/hadoop/common/hadoop-2.7.4/hadoop-2.7.4.tar.gz (download hadoop)
- tar -xzvf hadoop-2.7.4.tar.gz (extract compressed file)
- sudo mv hadoop-2.7.4 $HADOOP_HOME
- sudo chown -R hduser:hadoop hadoop

## Update $HOME/.bashrc
Add the following lines to the `end` of the `$HOME/.bashrc` file of user hduser. If you use a shell other than bash, you should of course update its appropriate configuration files instead of `.bashrc`.
```bash
# Set Hadoop-related environment variables
export HADOOP_PREFIX=~/Program/hadoop #This is where your put your hadoop program
export HADOOP_HOME=$HADOOP_PREFIX
export HADOOP_COMMON_HOME=$HADOOP_PREFIX
export HADOOP_CONF_DIR=$HADOOP_PREFIX/etc/hadoop
export HADOOP_HDFS_HOME=$HADOOP_PREFIX
export HADOOP_MAPRED_HOME=$HADOOP_PREFIX
export HADOOP_YARN_HOME=$HADOOP_PREFIX

# Set JAVA_HOME (we will also configure JAVA_HOME directly for Hadoop later on)
export JAVA_HOME=$(readlink -f /usr/bin/java | sed "s:bin/java::")

# Some convenient aliases and functions for running Hadoop-related commands
unalias fs &> /dev/null
alias fs="hadoop fs"
unalias hls &> /dev/null
alias hls="fs -ls"

# If you have LZO compression enabled in your Hadoop cluster and
# compress job outputs with LZOP (not covered in this tutorial):
# Conveniently inspect an LZOP compressed file from the command
# line; run via:
#
# $ lzohead /hdfs/path/to/lzop/compressed/file.lzo
#
# Requires installed 'lzop' command.
#
lzohead () {
hadoop fs -cat $1 | lzop -dc | head -1000 | less
}

# Add Hadoop bin/ directory to PATH
export PATH=$PATH:$HADOOP_HOME/bin
```
Then you should run 'source .bashrc' to enable the new configuration.

## hadoop-env.sh
The only required environment variable we have to configure for Hadoop in this tutorial is `JAVA_HOME`. Open `hadoop/etc/hadoop/hadoop-env.sh`
- readlink -f /usr/bin/java | sed "s:bin/java::" (find the default Java path)
- sudo vim $HADOOP_HOME/etc/hadoop/hadoop-env.sh
```bash
# $HADOOP_HOME/etc/hadoop/hadoop-env.sh
#export JAVA_HOME=${JAVA_HOME}
export JAVA_HOME=$(readlink -f /usr/bin/java | sed "s:bin/java::")
```

- ` $HADOOP_HOME/bin/hadoop` or `hadoop`(run hadoop, following appears, then hadoop is installed correctly)
```bash
Usage: hadoop [--config confdir] [COMMAND | CLASSNAME]
CLASSNAME            run the class named CLASSNAME
or
where COMMAND is one of:
fs                   run a generic filesystem user client
version              print the version
jar <jar>            run a jar file
note: please use "yarn jar" to launch
YARN applications, not this command.
checknative [-a|-h]  check native hadoop and compression libraries availability
distcp <srcurl> <desturl> copy file or directories recursively
archive -archiveName NAME -p <parent path> <src>* <dest> create a hadoop archive
classpath            prints the class path needed to get the
credential           interact with credential providers
Hadoop jar and the required libraries
daemonlog            get/set the log level for each daemon
```
You can repeat this exercise also for other users who want to use Hadoop.

Create a directory called input in our home directory and copy Hadoop's configuration files into it to use those files as our data.
```bash
mkdir ~/input
cp $HADOOP_HOME/etc/hadoop/*.xml ~/input
```

run cmd:
```bash
hadoop jar $HADOOP_HOME/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.4.jar grep ~/input ~/grep_example 'principal[.]*'
```
You can see output like following:
```bash
Output
. . .
File System Counters
FILE: Number of bytes read=1247674
FILE: Number of bytes written=2324248
FILE: Number of read operations=0
FILE: Number of large read operations=0
FILE: Number of write operations=0
Map-Reduce Framework
Map input records=2
Map output records=2
Map output bytes=37
Map output materialized bytes=47
Input split bytes=114
Combine input records=0
Combine output records=0
Reduce input groups=2
Reduce shuffle bytes=47
Reduce input records=2
Reduce output records=2
Spilled Records=4
Shuffled Maps =1
Failed Shuffles=0
Merged Map outputs=1
GC time elapsed (ms)=61
Total committed heap usage (bytes)=263520256
Shuffle Errors
BAD_ID=0
CONNECTION=0
IO_ERROR=0
WRONG_LENGTH=0
WRONG_MAP=0
WRONG_REDUCE=0
File Input Format Counters
Bytes Read=151
File Output Format Counters
Bytes Written=37
```

If you run cmd:
```bash
cat ~/grep_example/*
```

You will see:
```bash
Output
6       principal
1       principal.
```
But if you run this test again, it will give some error. Don't worry, just delete the `grep_example` folder then everything will work fine.

## Hadoop Pseudo-distributed Mode
### HDFS Configuration
In this section, we will configure the directory where Hadoop will store its data files, the network ports it listens to, etc. Our setup will use Hadoop’s Distributed File System, `HDFS`, even though our little “cluster” only contains our single local machine.

You can leave the settings below “as is” with the exception of the `hadoop.tmp.dir` parameter – this parameter you must change to a directory of your choice. We will use the directory `/app/hadoop/tmp` in this tutorial. Hadoop’s default configurations use `hadoop.tmp.dir` as the base temporary directory both for the local file system and HDFS, so don’t be surprised if you see Hadoop creating the specified directory automatically on HDFS at some later point.

Now we create the directory and set the required ownerships and permissions:
```bash
mkdir -p ~/Program/hadoop/tmp
sudo chown hduser:hadoop ~/Program/hadoop/tmp
# ...and if you want to tighten up security, chmod from 755 to 750...
sudo chmod 750 ~/Program/hadoop/tmp
```

Add the following snippets between the <configuration> ... </configuration> tags in the respective configuration XML file.

HDFS is the distributed file system used by Hadoop to store data in the cluster, capable of hosting very very (very) large files, splitting them over the nodes of the cluster. Theoretically, you don't need to have it running and files could instead be stored elsewhere like S3 or even the local file system (if using a purely local Hadoop installation). However, some applications require interactions with HDFS so you may have to set it up sooner or later if you're using third party modules. HDFS is composed of a`NameNode` which holds all the metadata regarding the stored files, and `DataNodes` (one per node in the cluster) which hold the actual data.

The main HDFS configuration file is located at `$HADOOP_PREFIX/etc/hadoop/hdfs-site.xml`. If you've been following since the beginning, this file should be empty so it will use the default configurations outlined in [this page](http://hadoop.apache.org/docs/current/hadoop-project-dist/hadoop-hdfs/hdfs-default.xml). For a single-node installation of HDFS you'll want to change `hdfs-site.xml` to have, at the very least, the following:

First you should create `dfs` folder, `datanode` & `namenode` folder under `tmp` folder.
tmp
 |-- dfs
   |-- datanode
   |-- namenode

```xml
<configuration>
<property>
<name>dfs.datanode.data.dir</name>
<value>file:///home/osboxes/Program/hadoop/tmp/dfs/datanode</value>
<description>Comma separated list of paths on the local filesystem of a DataNode where it should store its blocks.</description>
</property>

<property>
<name>dfs.namenode.name.dir</name>
<value>file:///home/osboxes/Program/hadoop/tmp/dfs/namenode</value>
<description>Path on the local filesystem where the NameNode stores the namespace and transaction logs persistently.</description>
</property>

<property>
<name>dfs.replication</name>
<value>1</value>
<description>Default block replication.
The actual number of replications can be specified when the file is created.
The default is used if replication is not specified in create time.
</description>
</property>
</configuration>
```

In addition, add the following to `$HADOOP_PREFIX/etc/hadoop/core-site.xml` to let the Hadoop modules know where the `HDFS NameNode` is located.
```xml
<configuration>
<property>
<name>hadoop.tmp.dir</name>
<value>file:///home/osboxes/Program/hadoop/tmp</value>
<description>Abase for other temporary directories.</description>
</property>
<property>
<name>fs.defaultFS</name>
<value>hdfs://localhost:9000/</value>
<description>NameNode URI</description>
</property>
</configuration>
```

`Note`:
If you configured `core-site.xml`, then the bellow example testing will fail.
```bash
# This test will fail in to connection refuse
hadoop jar $HADOOP_HOME/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.4.jar grep ~/input ~/grep_example 'principal[.]*'
```

### YARN on a Single Node
YARN is the component responsible for allocating containers to run tasks, coordinating the execution of said tasks, restart them in case of failure, among other housekeeping. Just like HDFS, it also has 2 main components: a ResourceManager which keeps track of the cluster resources and NodeManagers in each of the nodes which communicates with the ResourceManager and sets up containers for execution of tasks.

You can run a MapReduce job on YARN in a pseudo-distributed mode by setting a few parameters and running ResourceManager daemon and NodeManager daemon in addition.

Configure parameters as follows: etc/hadoop/`mapred-site.xml:`
```bash
<configuration>
<property>
<name>mapreduce.framework.name</name>
<value>yarn</value>
</property>
</configuration>
```

 `$HADOOP_PREFIX/etc/hadoop/yarn-site.xml`. The file should currently be empty which means it's using the default configurations you can find [here](http://hadoop.apache.org/docs/current/hadoop-yarn/hadoop-yarn-common/yarn-default.xml). For a single-node installation of YARN you'll want to add the following to that file:
```bash
<configuration>
    <property>
        <name>yarn.scheduler.minimum-allocation-mb</name>
        <value>128</value>
        <description>Minimum limit of memory to allocate to each container request at the Resource Manager.</description>
        </property>
    <property>
        <name>yarn.scheduler.maximum-allocation-mb</name>
        <value>2048</value>
        <description>Maximum limit of memory to allocate to each container request at the Resource Manager.</description>
    </property>
    <property>
        <name>yarn.scheduler.minimum-allocation-vcores</name>
        <value>1</value>
        <description>The minimum allocation for every container request at the RM, in terms of virtual CPU cores. Requests lower than this won't take effect, and the specified value will get allocated the minimum.</description>
    </property>
    <property>
        <name>yarn.scheduler.maximum-allocation-vcores</name>
        <value>2</value>
        <description>The maximum allocation for every container request at the RM, in terms of virtual CPU cores. Requests higher than this won't take effect, and will get capped to this value.</description>
    </property>
    <property>
        <name>yarn.nodemanager.resource.memory-mb</name>
        <value>4096</value>
        <description>Physical memory, in MB, to be made available to running containers</description>
    </property>
    <property>
        <name>yarn.nodemanager.resource.cpu-vcores</name>
        <value>4</value>
        <description>Number of CPU cores that can be allocated for containers.</description>
    </property>
    
    <property>
    <name>yarn.nodemanager.aux-services</name>
    <value>mapreduce_shuffle</value>
    </property>
    
    <property>
    <name>yarn.nodemanager.vmem-pmem-ratio</name>
    <value>4</value>
    </property>
    
</configuration>
```

# Starting
Now that we've finished configuring everything, it's time to setup the folders and start the daemons:
```bash
## Start HDFS daemons
# Format the namenode directory (DO THIS ONLY ONCE, THE FIRST TIME)
$HADOOP_PREFIX/bin/hdfs namenode -format
# Start the namenode daemon
$HADOOP_PREFIX/sbin/hadoop-daemon.sh start namenode
# Start the datanode daemon
$HADOOP_PREFIX/sbin/hadoop-daemon.sh start datanode
## Start YARN daemons
# Start the resourcemanager daemon
$HADOOP_PREFIX/sbin/yarn-daemon.sh start resourcemanager
# Start the nodemanager daemon
$HADOOP_PREFIX/sbin/yarn-daemon.sh start nodemanager
```
or
```
# Format the namenode directory (DO THIS ONLY ONCE, THE FIRST TIME)
$HADOOP_PREFIX/bin/hdfs namenode -format
# Start NameNode daemon and DataNode daemon, The hadoop daemon log output is written to the $HADOOP_LOG_DIR directory (defaults to $HADOOP_HOME/logs).
$HADOOP_PREFIX/sbin/start-dfs.sh
$HADOOP_PREFIX/sbin/start-yarn.sh
```
Hopefully, everything should be running. Use the command `jps` to see if all daemons are launched. If one is missing, check `$HADOOP_PREFIX/logs/<daemon with problems>.log` for any errors.

The output looks like this:
```bash
hduser@hanson:$HADOOP_HOME/etc/hadoop$ jps
7890 DataNode
12722 ResourceManager
13013 NodeManager
13126 Jps
7703 NameNode
```

Make the HDFS directories required to execute MapReduce jobs:
```bash
$ bin/hdfs dfs -mkdir /user
$ bin/hdfs dfs -mkdir /user/<username>
```

# Testing
To test if everything is working ok, lets run one of the example applications shipped with Hadoop called DistributedShell. This application spawns a specified number of containers and runs a shell command in each of them. Lets run DistributedShell with the 'date' command which outputs the current time:
```bash
# Run Distributed shell with 2 containers and executing the script `date`.
$HADOOP_PREFIX/bin/hadoop jar $HADOOP_PREFIX/share/hadoop/yarn/hadoop-yarn-applications-distributedshell-2.7.4.jar org.apache.hadoop.yarn.applications.distributedshell.Client --jar $HADOOP_PREFIX/share/hadoop/yarn/hadoop-yarn-applications-distributedshell-2.7.4.jar --shell_command date --num_containers 2 --master_memory 1024
```

The output looks like this:
```bash
17/11/26 11:20:20 INFO distributedshell.Client: Initializing Client
17/11/26 11:20:20 INFO distributedshell.Client: Running Client
17/11/26 11:20:20 INFO client.RMProxy: Connecting to ResourceManager at /0.0.0.0:8032
17/11/26 11:20:20 INFO distributedshell.Client: Got Cluster metric info from ASM, numNodeManagers=1
17/11/26 11:20:20 INFO distributedshell.Client: Got Cluster node info from ASM
17/11/26 11:20:20 INFO distributedshell.Client: Got node report from ASM for, nodeId=hanson:35765, nodeAddresshanson:8042, nodeRackName/default-rack, nodeNumContainers0
17/11/26 11:20:20 INFO distributedshell.Client: Queue info, queueName=default, queueCurrentCapacity=0.0, queueMaxCapacity=1.0, queueApplicationCount=0, queueChildQueueCount=0
17/11/26 11:20:20 INFO distributedshell.Client: User ACL Info for Queue, queueName=root, userAcl=SUBMIT_APPLICATIONS
...
17/11/26 11:20:32 INFO distributedshell.Client: Got application report from ASM for, appId=1, clientToAMToken=null, appDiagnostics=, appMasterHost=hanson/10.211.55.3, appQueue=default, appMasterRpcPort=-1, appStartTime=1511716821975, yarnAppState=FINISHED, distributedFinalState=SUCCEEDED, appTrackingUrl=http://hanson:8088/proxy/application_1511716475099_0001/, appUser=hduser
17/11/26 11:20:32 INFO distributedshell.Client: Application has completed successfully. Breaking monitoring loop
17/11/26 11:20:32 INFO distributedshell.Client: Application completed successfully
```
With this command we are telling hadoop to run the `Client class` in the `hadoop-yarn-applications-distributedshell-2.7.4.jar`, passing it the jar containing the definition of the ApplicationMaster (the same jar), the shell command to run in each of the `hosts (date)`, the number of containers to spawn (2) and the memory used by the ApplicationMaster (1024MB). The value of 1024 was set empirically by trying to run the program several times until it stopped failing due to the ApplicationMaster using more memory than that which had been allocated to it. You can `check the entire set of parameters` you can pass to DistributedShell by using the same command `without any arguments`:
```bash
# Check the parameters for the DistributedShell client.
$HADOOP_PREFIX/bin/hadoop jar $HADOOP_PREFIX/share/hadoop/yarn/hadoop-yarn-applications-distributedshell-2.7.4.jar org.apache.hadoop.yarn.applications.distributedshell.Client
```
The output should look like this:
```bash
17/11/26 11:29:46 INFO distributedshell.Client: Initializing Client
No jar file specified for application master
usage: Client
-appname <arg>                                 Application Name. Default
value - DistributedShell
-attempt_failures_validity_interval <arg>      when
attempt_failures_validity_
interval in milliseconds
is set to > 0,the failure
number will not take
failures which happen out
of the validityInterval
into failure count. If
failure count reaches to
maxAppAttempts, the
application will be
failed.
...
```


# Hadoop Web Interfaces
## Web UIs for the Common User
---
The default Hadoop ports are as follows:

Daemon            | Default Port  | Configuration Parameter
---------------   | ------------  | -----------------------
------HDFS-------||
Namenode          |50070          |dfs.http.address
Datanodes         |50075          |dfs.datanode.http.address
Secondarynamenode |50090          |dfs.secondary.http.address
Backup/Checkpoint node? |50105    |dfs.backup.http.address
----MapReduce----||
Jobracker         |50030          |mapred.job.tracker.http.address
Tasktrackers      |50060          |mapred.task.tracker.http.address

Hadoop daemons expose some information over HTTP. All Hadoop daemons expose the following:

- `/logs`:
Exposes, for downloading, log files in the Java system property **hadoop.log.dir**.
- `/logLevel`:
Allows you to dial up or down **log4j** logging levels. This is similar to **hadoop daemonlog** on the command line.
- `/stacks`:
Stack traces for all threads. Useful for debugging.
- `/metrics`:
Metrics for the server. Use /metrics?format=json to retrieve the data in a structured form. `Available in 0.21`.

Individual daemons expose extra daemon-specific endpoints as well. Note that these are not necessarily part of Hadoop’s public API, so they tend to change over time.

The `Namenode` exposes:

- `/`:
Shows information about the namenode as well as the HDFS. There’s a link from here to browse the filesystem, as well.
- `/dfsnodelist.jsp?whatNodes=(DEAD|LIVE)`:
Shows lists of nodes that are disconnected from (DEAD) or connected to (LIVE) the namenode.
- `/fsck`:
Runs the “fsck” command. Not recommended on a busy cluster.
- `/listPaths`:
Returns an XML-formatted directory listing. This is useful if you wish (for example) to poll HDFS to see if a file exists. The URL can include a path (e.g., /listPaths/user/philip) and can take optional GET arguments: /listPaths?recursive=yes will return all files on the file system; /listPaths/user/philip?filter=s.* will return all files in the home directory that start with s; and /listPaths/user/philip?exclude=.txt will return all files except text files in the home directory. Beware that filter and exclude operate on the directory listed in the URL, and they ignore the recursive flag.
- `/data` and `/fileChecksum`
These forward your HTTP request to an appropriate datanode, which in turn returns the data or the checksum.

`Datanodes` expose the following:

- `/browseBlock.jsp`, `/browseDirectory.jsp, tail.jsp`, `/streamFile`, `/getFileChecksum`
These are the endpoints that the namenode redirects to when you are browsing filesystem content. You probably wouldn’t use these directly, but this is what’s going on underneath.
- `/blockScannerReport`
Every datanode verifies its blocks at configurable intervals. This endpoint provides a listing of that check.

The `secondarynamenode` exposes a simple status page with information including which namenode it’s talking to, when the last checkpoint was, how big it was, and which directories it’s using.

The `jobtracker`‘s UI is commonly used to look at running jobs, and, especially, to find the causes of failed jobs. The UI is best browsed starting at `/jobtracker.jsp`. There are over a dozen related pages providing details on tasks, history, scheduling queues, jobs, etc.

`Tasktrackers` have a simple page (`/tasktracker.jsp`), which shows running tasks. They also expose `/taskLog?taskid= `to query logs for a specific task. They use `/mapOutput` to serve the output of map tasks to reducers, but this is an internal API.

## Under the Covers for the Developer and the System Administrator
---
Internally, Hadoop mostly uses Hadoop IPC to communicate amongst servers. (Part of the goal of the Apache Avro project is to replace Hadoop IPC with something that is easier to evolve and more language-agnostic; HADOOP-6170 is the relevant ticket.) Hadoop also uses HTTP (for the secondarynamenode communicating with the namenode and for the tasktrackers serving map outputs to the reducers) and a raw network socket protocol (for datanodes copying around data).

The following table presents the ports and protocols (including the relevant Java class) that Hadoop uses. This table does not include the HTTP ports mentioned above.

Daemon|Default Port|Configuration Parameter|Protocol|Used for
------|------------|-----------------------|--------|--------
Namenode|8020|fs.default.name|IPC: ClientProtocol|Filesystem metadata operations
Datanode|50010|dfs.datanode.address|Custom Hadoop Xceiver: DataNode and DFSClient|DFS data transfer
Datanode|50020|dfs.datanode.ipc.address|IPC: InterDatanodeProtocol, ClientDatanodeProtocolClientProtocol|Block metadata operations and recovery                               
Backupnode|50100|dfs.backup.address|Same as namenode|HDFS Metadata Operations
> This is the port part of hdfs://host:8020/.
  Default is not well-defined. Common values are 8021, 9001, or 8012. See [MAPREDUCE-566](http://issues.apache.org/jira/browse/MAPREDUCE-566).
  Binds to an unused local port.
  
# Reference
- [Running Hadoop on Ubuntu Linux (Single-Node Cluster)](http://www.michael-noll.com/tutorials/running-hadoop-on-ubuntu-linux-single-node-cluster/#installation)
- [Hadoop YARN Installation: The definitive guide](https://www.alexjf.net/blog/distributed-systems/hadoop-yarn-installation-definitive-guide/)
- [How to Install Hadoop in Stand-Alone Mode on Ubuntu 16.04](https://www.digitalocean.com/community/tutorials/how-to-install-hadoop-in-stand-alone-mode-on-ubuntu-16-04)
- [Hadoop安装教程_单机/伪分布式配置_Hadoop2.6.0/Ubuntu14.04](http://www.powerxing.com/install-hadoop/)
- [Hadoop Default Ports Quick Reference](http://blog.cloudera.com/blog/2009/08/hadoop-default-ports-quick-reference/)
- [](https://hadoop.apache.org/docs/current/hadoop-project-dist/hadoop-common/SingleCluster.html)
