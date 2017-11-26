# [Install Hadoop](https://www.digitalocean.com/community/tutorials/how-to-install-hadoop-in-stand-alone-mode-on-ubuntu-16-04)
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
```

output should be like:
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
hduser@hanson:~$ cat $HOME/.ssh/id_rsa.pub >> $HOME/.ssh/authorized_keys
```
**test the SSH setup by connecting to your local machine with the `hduser` user**
```bash
hduser@hanson:~$ ssh localhost
```
You should install `ssh` first ( sudo apt-get install ssh)

Output should be like:
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
### Alternative

You can also disable IPv6 only for Hadoop as documented in [HADOOP-3437](https://issues.apache.org/jira/browse/HADOOP-3437). You can do so by adding the following line to `conf/hadoop-env.sh`:
```bash
export HADOOP_OPTS=-Djava.net.preferIPv4Stack=true
```
## Hadoop installation
- wget http://mirrors.ocf.berkeley.edu/apache/hadoop/common/hadoop-2.7.4/hadoop-2.7.4.tar.gz (download hadoop)
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

# Reference
[Running Hadoop on Ubuntu Linux (Single-Node Cluster)](http://www.michael-noll.com/tutorials/running-hadoop-on-ubuntu-linux-single-node-cluster/#installation)
