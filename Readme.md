# [Install Hadoop](https://www.digitalocean.com/community/tutorials/how-to-install-hadoop-in-stand-alone-mode-on-ubuntu-16-04)
- sudo apt-get update
- sudo apt-get install default-jdk
- wget http://mirrors.ocf.berkeley.edu/apache/hadoop/common/hadoop-2.7.4/hadoop-2.7.4.tar.gz
- tar -xzvf hadoop-2.7.4.tar.gz (extract compressed file)
- sudo mv hadoop-2.7.4 /usr/local/hadoop
- readlink -f /usr/bin/java | sed "s:bin/java::" (find the default Java path)
- sudo vim /usr/local/hadoop/etc/hadoop/hadoop-env.sh
```bash
#export JAVA_HOME=${JAVA_HOME}
export JAVA_HOME=$(readlink -f /usr/bin/java | sed "s:bin/java::")
```
- /usr/local/hadoop/bin/hadoop (run hadoop, following appears, then hadoop is installed correctly)
```bash
Output
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

# First testing
Create a directory called input in our home directory and copy Hadoop's configuration files into it to use those files as our data.
```bash
mkdir ~/input
cp /usr/local/hadoop/etc/hadoop/*.xml ~/input
```

run cmd:
```bash
/usr/local/hadoop/bin/hadoop jar /usr/local/hadoop/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.4.jar grep ~/input ~/grep_example 'principal[.]*'
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


